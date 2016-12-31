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
#include <IL/ilut.h>

#include "Vertex.h"
#include "Face.h"
#include "Normal.h"

/** MACROS **/
#define DISTANCE 1
#define ANGLE_INCREMENT 0.1
#define STEP_INCREMENT 1
#define INITIAL_HEIGHT 1
#define INITIAL_ALPHA 0
#define INITIAL_BETA 0
#define I_360_DG (M_PI * 2)
#define I_90_DG (M_PI / 2)
#define I_270_DG (3*M_PI/2)

/** COORDS CALC MACROS **/
#define SIN_INCREMENT (STEP_INCREMENT * sin(alpha))
#define COS_INCREMENT (STEP_INCREMENT * cos(alpha))

using namespace std;

GLdouble alpha = 0, beta = 0;
GLdouble eyex = -5, eyey;

/** Function headers **/
void init(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
int parseFile(const char *filename, Face **faces);
void specialKeys(int key, int x, int y);
int lista;
void idle(void);

GLuint texturaCielo;

/** Main Function **/
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    ilInit();
    iluInit();
    ilutRenderer(ILUT_OPENGL);
    glutCreateWindow("Fortaleza");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
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
    glEnable(GL_TEXTURE_2D);
    glClearColor(1, 1, 1, 1);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glutFullScreen();

    /* Carga de texturas */
    ILuint imgid;
    ilGenImages(1, &imgid);
    ilBindImage(imgid);
    ilLoadImage((const ILstring)"stone.jpg");
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    ILuint ancho = ilGetInteger(IL_IMAGE_WIDTH);
    ILuint alto = ilGetInteger(IL_IMAGE_HEIGHT);
    ILubyte *data = ilGetData();
    ilDeleteImages(1, &imgid);
    glGenTextures(1, &texturaCielo);
    glBindTexture(GL_TEXTURE_2D, texturaCielo);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ancho, alto, 0,  GL_RGBA, GL_UNSIGNED_BYTE, data);

    Face *facesFortaleza;
    int numFacesFortaleza = parseFile("fortaleza.ase", &facesFortaleza);
    lista = glGenLists(1);
    glNewList(lista, GL_COMPILE);
        glPushMatrix();
        glScalef(0.1f, 0.1f, 0.1f);
        for (int i = 0; i<numFacesFortaleza; i++){
             facesFortaleza[i].render();
        }
        glPopMatrix();
    glEndList();
}

void display(void)
{
    GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 0}; // posición en la escena
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); // a la luz 0

    GLdouble atx = 0, aty = 0, atz = 0;

    /* Recalculo de las coordenadas dependiendo de los nuevos ángulos */
    atx = eyex + DISTANCE * cos(alpha) * cos(beta);
    aty = eyey + DISTANCE * cos(beta) * sin(alpha);
    atz = INITIAL_HEIGHT + DISTANCE * sin(beta);

     // Posicionado de la cámara
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

    glColor3ub(255, 255, 255);
    glCallList(lista);
    glutSwapBuffers();
}

/* Funcion que se llamara cada vez que se redimensione la ventana */
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, w/(GLdouble)h, 0.1, 1000);
}

/**
* Funcion de teclas especiales (UP, DOWN, etc...)
*/
void  specialKeys(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_UP:
        if (beta < (I_90_DG - ANGLE_INCREMENT)){
            beta+=ANGLE_INCREMENT;
        }
        break;
    case GLUT_KEY_DOWN:
        if (beta > (-I_90_DG + ANGLE_INCREMENT)){
            beta-=ANGLE_INCREMENT;
        }
        break;
    case GLUT_KEY_LEFT:
        alpha+=ANGLE_INCREMENT;
        break;
    case GLUT_KEY_RIGHT:
        alpha-=ANGLE_INCREMENT;
        break;
    }

    glutPostRedisplay();
}

/* Funcion que controla los eventos de teclado */
void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27:
        exit(0); /* tecla escape */
        break;
    case 'w':
        eyex += COS_INCREMENT;
        eyey += SIN_INCREMENT;
        break;
    case 's':
        eyex -= COS_INCREMENT;
        eyey -= SIN_INCREMENT;
        break;
    case 'd':
        eyex += SIN_INCREMENT;
        eyey -= COS_INCREMENT;
        break;
    case 'a':
        eyex -= SIN_INCREMENT;
        eyey += COS_INCREMENT;
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

// Funcion de extracción de datos de la figura a partir del fichero
int parseFile(const char *filename, Face **faces){
    fstream fich;
    *faces = NULL;
    fich.open(filename, ios::in); // Apertura del fichero
    char command[200];
    Vertex *vertex = NULL;
    int numFaces = 0;
    while(!fich.eof()){
        fich >> command;
        if (!strcmp(command, "*MESH_NUMVERTEX")){ // Extracción del numero de vertices
            int numVertex;
            fich >> numVertex;
            vertex = (Vertex *)malloc(numVertex * sizeof(Vertex)); // Creamos la lista de vértices
        }else if (!strcmp(command, "*MESH_NUMFACES")){ // Extracción del numero de caras
            fich >> numFaces;
            *faces = (Face *)malloc(numFaces * sizeof(Face));
            if (*faces == NULL){
                return -1;
            }
        }else if (!strcmp(command, "*MESH_VERTEX")){ // Extracción de los vértices
            int index;
            double x, y, z;
            fich >> index >> x >> y >> z;
            vertex[index] = Vertex(x, y, z); // Instanciamos el vértice
        }else if (!strcmp(command, "*MESH_FACE")){ // Extracción de las caras
            int index, vertexIndexA, vertexIndexB, vertexIndexC;
            char vertexA[3], vertexB[3], vertexC[3], dp;
            fich >> index >> dp >> vertexA >> vertexIndexA >> vertexB >> vertexIndexB >> vertexC >> vertexIndexC;
            Vertex *vertices = (Vertex *)malloc(sizeof(Vertex) * 3);
            // Copiado de los vértices de la figura para asignárselos
            memcpy(&vertices[0], &vertex[vertexIndexA], sizeof(Vertex));
            memcpy(&vertices[1], &vertex[vertexIndexB], sizeof(Vertex));
            memcpy(&vertices[2], &vertex[vertexIndexC], sizeof(Vertex));
            (*faces)[index] = Face(vertices, 3); // Instanciamos la clase de la cara

        }else if (!strcmp(command, "*MESH_FACENORMAL")){ // Extracción de las normales
            int faceIndex;
            float x, y, z;
            fich >> faceIndex >> x >> y >> z;
            (*faces)[faceIndex].setNormal(new Normal(x, y, z)); // Establecemos la normal de la cara
        }
    }
    fich.close(); //Cerrado del fichero
    return numFaces;
}


