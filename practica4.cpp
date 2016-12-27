/*****************************************************/
/*                                                   */
/*                 PRACTICA 4 MAIN                   */
/*                                                   */
/*****************************************************/

#include <GL/freeglut.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <fstream>

/** MACROS **/
#define INITIAL_HEIGHT 1

using namespace std;

GLdouble eyex = -5, eyey;

/** Function headers **/
void init(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
//void specialKeys(int key, int x, int y);
void idle(void);

/** Main Function **/
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Hola Mundo OpenGL");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    //glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}

/** Internal function defintion **/
void init(void)
{
    GLfloat difusa[] = { 1.0f, 1.0f, 1.0f, 1.0f}; // luz blanca
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa); // Se asignan los parámetros
    glLightfv(GL_LIGHT1, GL_DIFFUSE, difusa);
    glEnable(GL_LIGHT0); // Se “enciende” la luz 0
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING); // Se activan los cálculos de la iluminación
    glShadeModel(GL_SMOOTH);
    glClearColor(1, 1, 1, 1);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glutFullScreen();
}

void display(void)
{
    GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 0}; // posición en la escena
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); // a la luz 0

        GLdouble atx = 0, aty = 0, atz = 0;

     // Posicionado de la cámara
    gluLookAt(eyex, // Coordenada X de la cámara
      eyey, // Coordenada Y de la cámara
      INITIAL_HEIGHT, // Coordenada Z de la cámara
      atx, /* Puntos donde mira la cámara */
      aty,
      atz,
      0.0, /* Orientación de la cámara */
      0.0,
      1);

    // Referecias
    glPushMatrix();
    glTranslatef(0,0,0.7f);
    glRotatef(90, 1, 0,0);
    glRotatef(90, 0,1,0);
    glColor3ub(0, 255,0);
    glutSolidTeapot(1);
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, w/(GLdouble)h, 0.1, 1000);
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27:
        exit(0); /* tecla escape */
        break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    glutPostRedisplay();
}

void idle(void)
{
    glutPostRedisplay();
}


