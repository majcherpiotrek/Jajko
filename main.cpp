/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacją osi
//  układu współrzędnych dla rzutowania perspektywicznego

/*************************************************************************************/
#include <GL/glut.h>
#include <iostream>
#include <math.h>

typedef float point3[3];

static GLfloat viewer[]= {0.0, 0.0, 10.0};
static GLfloat R = 10.0;
static const GLfloat R_min = 2.6;
static const GLfloat R_max = 23.0;

// inicjalizacja położenia obserwatora

static GLfloat theta = 0.0;   // kąt obrotu obiektu
static GLfloat fi = 0.0;   // kąt obrotu obiektu
static GLfloat pix2angle_X;     // przelicznik pikseli na stopnie
static GLfloat pix2angle_Y;

static GLint status = 0;       // stan klawiszy myszy
// 0 - nie naciśnięto żadnego klawisza
// 1 - naciśnięty został lewy klawisz
// 2 - naciśnięty został prawy klawisz

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy

static int delta_x = 0;        // różnica pomiędzy pozycją bieżącą
// i poprzednią kursora myszy

static int y_pos_old = 0;

static int delta_y = 0;

/*************************************************************************************/

typedef float point3[3];
typedef float point2[2];
GLint N = 10;


// Funkcja rysująca osie układu współrzędnych
enum Model{
    POINTS, NET, TRIANGLES
};

Model model = POINTS;
point3** colors;

void Egg(GLint n)
{
    /*Tworzenie siatki dziedziny parametrycznej*/
    point2** param = new point2*[n];

    for(int i = 0; i < n; i++)
    {
        param[i] = new point2[n];

        for(int k = 0; k < n; k++)
        {
            GLfloat u = (float)i/(n-1);
            GLfloat v = (float)k/(n-1);

            param[i][k][0] = u;
            param[i][k][1] = v;
        }
    }

    point3** egg_p = new point3*[n];

    for(int i = 0; i < n; i++)
    {
        egg_p[i] = new point3[n];
        for(int k = 0; k < n; k++)
        {
            egg_p[i][k][0] = (float)((-90*pow(param[i][k][0], 5) + 225*pow(param[i][k][0], 4) - 270*pow(param[i][k][0], 3) + 180*pow(param[i][k][0],2) - 45*param[i][k][0])*cos(M_PI*param[i][k][1]));
            egg_p[i][k][1] = (float)(160*pow(param[i][k][0], 4) - 320*pow(param[i][k][0], 3) + 160*pow(param[i][k][0], 2));
            egg_p[i][k][2] = (float)((-90*pow(param[i][k][0], 5) + 225*pow(param[i][k][0], 4) - 270*pow(param[i][k][0], 3) + 180*pow(param[i][k][0],2) - 45*param[i][k][0])*sin(M_PI*param[i][k][1]));
        }
    }

    switch(model)
    {
        case POINTS:
        {

            for(int i = 0; i < n; i++)
                for(int k = 0; k < n; k++)
                {
                    glBegin(GL_POINTS);
                    glColor3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(egg_p[i][k][0], egg_p[i][k][1]-5.0f, egg_p[i][k][2]);
                    glEnd();
                }
            break;
        }

        case NET:
        {
            for(int i = 0; i < n-1; i++) //Przechodzimy przez wszystkie poziomy jajka od dołu do góry
                for(int k = 0; k < n-1; k++) //Przechodzimy przez wszystkie punkty na jednym poziomie (piersćieniu)
                {
                    glBegin(GL_LINES); // Rysujemy poziome okręgi

                    glColor3f(0.0f, 0.0f, 0.85f);
                    glVertex3f(egg_p[i][k][0], egg_p[i][k][1]-5.0f, egg_p[i][k][2]);
                    glVertex3f(egg_p[i][k+1][0], egg_p[i][k+1][1]-5.0f, egg_p[i][k+1][2]);
                    glEnd();

                    glBegin(GL_LINES); //Rysujemy pionowe linie między poziomymi okręgami

                    glColor3f(0.0f, 0.0f, 0.85f);
                    glVertex3f(egg_p[i][k][0], egg_p[i][k][1]-5.0f, egg_p[i][k][2]);
                    glVertex3f(egg_p[i+1][k][0], egg_p[i+1][k][1]-5.0f, egg_p[i+1][k][2]);
                    glEnd();

                    glBegin(GL_LINES); //Rysujemy skośne linie między poziomymi okrgami

                    glColor3f(0.0f, 0.0f, 0.85f);
                    glVertex3f(egg_p[i+1][k][0], egg_p[i+1][k][1]-5.0f, egg_p[i+1][k][2]);
                    glVertex3f(egg_p[i][k+1][0], egg_p[i][k+1][1]-5.0f, egg_p[i][k+1][2]);
                    glEnd();

                }
            break;
        }

        case TRIANGLES:
        {
            for(int i = 0; i < n-1; i++)//Przechodzimy przez wszystkie poziomy jajka od dołu do góry
            {
                for(int k = 0; k < n-1; k++) //Przechodzimy przez wszystkie punkty na jednym poziomie (piersćieniu)
                {
                    if(k != n-2) //wszystkie trójkaty na i-tej półobręczy oprócz ostatniego!
                    {
                        glBegin(GL_TRIANGLES);

                        glColor3f(colors[i][k][0], colors[i][k][1], colors[i][k][2]);
                        glVertex3f(egg_p[i][k][0], egg_p[i][k][1]-5.0f, egg_p[i][k][2]);

                        glColor3f(colors[i][k+1][0], colors[i][k+1][1], colors[i][k+1][2]);
                        glVertex3f(egg_p[i][k+1][0], egg_p[i][k+1][1]-5.0f, egg_p[i][k+1][2]);

                        glColor3f(colors[i+1][k][0], colors[i+1][k][1], colors[i+1][k][2]);
                        glVertex3f(egg_p[i+1][k][0], egg_p[i+1][k][1]-5.0f, egg_p[i+1][k][2]);

                        glEnd();

                        glBegin(GL_TRIANGLES);

                        glColor3f(colors[i][k+1][0], colors[i][k+1][1], colors[i][k+1][2]);
                        glVertex3f(egg_p[i][k+1][0], egg_p[i][k+1][1]-5.0f, egg_p[i][k+1][2]);

                        glColor3f(colors[i+1][k+1][0], colors[i+1][k+1][1], colors[i+1][k+1][2]);
                        glVertex3f(egg_p[i+1][k+1][0], egg_p[i+1][k+1][1]-5.0f, egg_p[i+1][k+1][2]);

                        glColor3f(colors[i+1][k][0], colors[i+1][k][1], colors[i+1][k][2]);
                        glVertex3f(egg_p[i+1][k][0], egg_p[i+1][k][1]-5.0f, egg_p[i+1][k][2]);

                        glEnd();
                    }
                    else
                    {   /*
                        Na jedną obręcz jajka składają się dwie półobręcze o numerach i oraz n-i-2.
                        Aby usunąć linię łączenia na jajku należy zadbac o to, aby kolor w wierzcholkach,
                        w których łączą się półobręcze był taki sam.
                        Punkty dla x = i oraz y = k = 0 odpowiadają punktom dla x = n-i-2 i k = n-1.
                        */
                        glBegin(GL_TRIANGLES);

                        glColor3f(colors[i][k][0], colors[i][k][1], colors[i][k][2]);
                        glVertex3f(egg_p[i][k][0], egg_p[i][k][1]-5.0f, egg_p[i][k][2]);

                        glColor3f(colors[n-i-1][0][0], colors[n-i-1][0][1], colors[n-i-1][0][2]);
                        glVertex3f(egg_p[i][k+1][0], egg_p[i][k+1][1]-5.0f, egg_p[i][k+1][2]);

                        glColor3f(colors[i+1][k][0], colors[i+1][k][1], colors[i+1][k][2]);
                        glVertex3f(egg_p[i+1][k][0], egg_p[i+1][k][1]-5.0f, egg_p[i+1][k][2]);

                        glEnd();

                        glBegin(GL_TRIANGLES);

                        glColor3f(colors[n-i-1][0][0], colors[n-i-1][0][1], colors[n-i-1][0][2]);
                        glVertex3f(egg_p[i][k+1][0], egg_p[i][k+1][1]-5.0f, egg_p[i][k+1][2]);

                        glColor3f(colors[n-i-2][0][0], colors[n-i-2][0][1], colors[n-i-2][0][2]);
                        glVertex3f(egg_p[i+1][k+1][0], egg_p[i+1][k+1][1]-5.0f, egg_p[i+1][k+1][2]);

                        glColor3f(colors[i+1][k][0], colors[i+1][k][1], colors[i+1][k][2]);
                        glVertex3f(egg_p[i+1][k][0], egg_p[i+1][k][1]-5.0f, egg_p[i+1][k][2]);

                        glEnd();
                    }
                }
            }

            break;
        }
    }
}
/*************************************************************************************/
// Funkcja "bada" stan myszy i ustawia wartości odpowiednich zmiennych globalnych

void Mouse(int btn, int state, int x, int y)
{


    if(btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        x_pos_old=x;        // przypisanie aktualnie odczytanej pozycji kursora
        y_pos_old=y;                     // jako pozycji poprzedniej
        status = 1;          // wcięnięty został lewy klawisz myszy
    }
    else
    if(btn==GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        y_pos_old = y;
        status = 2;
    }
    else
        status = 0;          // nie został wcięnięty żaden klawisz

}



/*************************************************************************************/
// Funkcja "monitoruje" położenie kursora myszy i ustawia wartości odpowiednich
// zmiennych globalnych

void Motion( GLsizei x, GLsizei y )
{


    delta_x = x - x_pos_old;     // obliczenie różnicy położenia kursora myszy

    x_pos_old = x;            // podstawienie bieżącego położenia jako poprzednie

    delta_y = y - y_pos_old;

    y_pos_old = y;

    glutPostRedisplay();     // przerysowanie obrazu sceny
}

/*************************************************************************************/

// Funkcja rysująca osie układu wspó?rz?dnych
void Axes(void)
{

    point3  x_min = {-5.0, 0.0, 0.0};
    point3  x_max = { 5.0, 0.0, 0.0};
    // pocz?tek i koniec obrazu osi x

    point3  y_min = {0.0, -5.0, 0.0};
    point3  y_max = {0.0,  5.0, 0.0};
    // pocz?tek i koniec obrazu osi y

    point3  z_min = {0.0, 0.0, -5.0};
    point3  z_max = {0.0, 0.0,  5.0};
    //  pocz?tek i koniec obrazu osi y

    glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
    glBegin(GL_LINES); // rysowanie osi x

    glVertex3fv(x_min);
    glVertex3fv(x_max);

    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
    glBegin(GL_LINES);  // rysowanie osi y

    glVertex3fv(y_min);
    glVertex3fv(y_max);

    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
    glBegin(GL_LINES); // rysowanie osi z

    glVertex3fv(z_min);
    glVertex3fv(z_max);

    glEnd();

}

/*************************************************************************************/

// Funkcja określająca co ma być rysowane (zawsze wywoływana, gdy trzeba
// przerysować scenę)



void RenderScene(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Czyszczenie okna aktualnym kolorem czyszczącym

    glLoadIdentity();
    // Czyszczenie macierzy bie??cej

    gluLookAt(viewer[0],viewer[1],viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    // Zdefiniowanie położenia obserwatora



    glColor3f(1.0f, 1.0f, 1.0f);
    // Ustawienie koloru rysowania na biały

    if(status == 1)                     // jeśli lewy klawisz myszy wcięnięty
    {


        theta += delta_x*pix2angle_X;
        fi += delta_y*pix2angle_Y;   // modyfikacja kąta obrotu o kat proporcjonalny
        // do różnicy położeń kursora myszy

        //viewer[0] = R * cos(theta) * cos(fi);
        //viewer[1] = R * sin(fi);
        //viewer[2] = R * sin(theta) * cos(fi);
    }
    if(status == 2)
    {
        viewer[2] += delta_y*0.05;
        if (viewer[2] > R_max)
            viewer[2] = R_max;

        if(viewer[2] < R_min)
            viewer[2] = R_min;

        std::cout<<viewer[2]<<std::endl;
    }

    gluLookAt(viewer[0],viewer[1],viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(theta, 0.0, 1.0, 0.0);  //obrót obiektu o nowy kąt
    glRotatef(fi, 1.0, 0.0, 0.0);

    Axes();
    // Narysowanie osi przy pomocy funkcji zdefiniowanej powyżej


    Egg(N);

    glFlush();
    // Przekazanie poleceń rysujących do wykonania

    glutSwapBuffers();



}
/*************************************************************************************/

// Funkcja ustalająca stan renderowania

void keys(unsigned char key, int x, int y)
{
    if(key == 'p') model = POINTS;
    if(key == 'w') model = NET;
    if(key == 's') model = TRIANGLES;
    if(key == '+')
    {
        N += 5;
        delete[] colors;
        colors = new point3*[N];

        for(int i = 0; i < N; i++)
        {
            colors[i] = new point3[N];

            for(int k = 0; k < N; k++)
            {
                colors[i][k][0] = (float)(rand()%1001)/1000;
                colors[i][k][1] = (float)(rand()%1001)/1000;
                colors[i][k][2] = (float)(rand()%1001)/1000;
            }
        }
    }

    if(key == '-' && N >= 10)
    {
        N = N - 5;
        delete[] colors;
        colors = new point3*[N];

        for(int i = 0; i < N; i++)
        {
            colors[i] = new point3[N];

            for(int k = 0; k < N; k++)
            {
                colors[i][k][0] = (float)(rand()%1001)/1000;
                colors[i][k][1] = (float)(rand()%1001)/1000;
                colors[i][k][2] = (float)(rand()%1001)/1000;
            }
        }
    }
    RenderScene(); // przerysowanie obrazu sceny
}


void MyInit(void)
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
// Kolor czyszczący (wypełnienia okna) ustawiono na czarny

}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

    pix2angle_X = 360/(float)horizontal;  // przeliczenie pikseli na stopnie
    pix2angle_Y = 360/(float)vertical;  // przeliczenie pikseli na stopnie

    glMatrixMode(GL_PROJECTION);
    // Przełączenie macierzy bieżącej na macierz projekcji

    glLoadIdentity();
    // Czyszcznie macierzy bieżącej

    gluPerspective(70, 1.0, 1.0, 50.0);
    // Ustawienie parametrów dla rzutu perspektywicznego


    if(horizontal <= vertical)
        glViewport(0, (vertical-horizontal)/2, horizontal, horizontal);

    else
        glViewport((horizontal-vertical)/2, 0, vertical, vertical);
    // Ustawienie wielkości okna okna widoku (viewport) w zależności
    // relacji pomiędzy wysokością i szerokością okna

    glMatrixMode(GL_MODELVIEW);
    // Przełączenie macierzy bieżącej na macierz widoku modelu

    glLoadIdentity();
    // Czyszczenie macierzy bieżącej

}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli



int main(int argc, char** argv)
{
    srand(time(NULL));

    colors = new point3*[N];

    for(int i = 0; i < N; i++)
    {
        colors[i] = new point3[N];

        for(int k = 0; k < N; k++)
        {
            colors[i][k][0] = (float)(rand()%1001)/1000;
            colors[i][k][1] = (float)(rand()%1001)/1000;
            colors[i][k][2] = (float)(rand()%1001)/1000;
        }
    }

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);

    glutInitWindowSize(300, 300);

    glutCreateWindow("Obserwator jajka");

    glutDisplayFunc(RenderScene);
    // Określenie, że funkcja RenderScene będzie funkcją zwrotną
    // (callback function).  Będzie ona wywoływana za każdym razem
    // gdy zajdzie potrzeba przerysowania okna

    glutKeyboardFunc(keys);

    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
    // za zmiany rozmiaru okna

    glutMouseFunc(Mouse);
    // Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

    glutMotionFunc(Motion);
    // Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy

    MyInit();
    // Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
    // inicjalizacje konieczne  przed przystąpieniem do renderowania

    glEnable(GL_DEPTH_TEST);
    // Włączenie mechanizmu usuwania niewidocznych elementów sceny

    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT
    return 0;
}

/*************************************************************************************/