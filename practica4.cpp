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
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "Vertex.h"
#include "Face.h"
#include "Normal.h"


/** MACROS **/
#define ANGLE_INCREMENT 0.01
#define STEP_INCREMENT 0.1
#define INITIAL_HEIGHT 2
#define INITIAL_ALPHA 0
#define INITIAL_BETA 0
#define I_360_DG (M_PI * 2)
#define I_90_DG (M_PI / 2)
#define I_270_DG (3*M_PI/2)
#define COORDS_ITERATIONS(coords) (sizeof(coords)/(sizeof(GLfloat)*3))
#define MURALLA_ELEMENTS (20)
#define FRENTE_ELEMENTS (15)
#define DISTANCIA_FRONTAL (((MURALLA_ELEMENTS) * 5) - 20)
#define DISTANCIA_LATERAL ((FRENTE_ELEMENTS / 2) * 5)

/** File paths **/

#define STONE_TEX "img/stone.jpg"
#define ARENISCA_TEX "img/arenisca.jpg"
#define CIELO_TEX "img/nube.jpg"
#define TIERRA_TEX "img/tierra.jpg"

#define CASA1_ASE "ases/casa1.ase"
#define CASA2_ASE "ases/casa2.ase"
#define FORTALEZA_ASE "ases/fortaleza.ase"
#define POZO_ASE "ases/pozo.ase"

#define MUSIC_SOUND "sound/camelot.mp3"
#define PASOS_SOUND "sound/pasos.wav"


/** COORDS CALC MACROS **/
#define SIN_INCREMENT (STEP_INCREMENT * sin(alpha))
#define COS_INCREMENT (STEP_INCREMENT * cos(alpha))

using namespace std;

struct CameraCoords {
    /* Coordenadas de la cámara */
    GLdouble eye_x;
    GLdouble eye_y;
    GLdouble eye_z;
    /* Objetivo de la cámara */
    GLdouble at_x;
    GLdouble at_y;
    GLdouble at_z;
};

enum CameraType {
    FP_CAM = 0,
    C1_CAM,
    C2_CAM,
    C3_CAM,
    C4_CAM,
    ROT_CAM,
    MULTICAM,
    NUM_CAMS // Elemento para contar las cámaras disponibles
};


/** VARIABLES GLOBALES **/
GLdouble alpha = 0, beta = 0, alpha_rot = 0, beta_rot = 0;
bool plane_mode = false;
int screen_width, screen_height,width1,height1;
int mouse_x, mouse_y;
GLdouble eyex = -5, eyey;
GLuint listaMuralla,listaTorre,listaFortaleza,listaPozo,listaCasa1,listaCasa2, listaSuelo, listaCielo;
GLuint texturaPiedra, texturaCielo, texturaTierra, texturaArenisca;
CameraType camara = FP_CAM;
struct CameraCoords fp_coords = {0, 0, INITIAL_HEIGHT, 0, 0, 0},
    rot_coords = {15, -19, INITIAL_HEIGHT, 0, 0, 0},
    c1_coords = {-20, DISTANCIA_LATERAL, 30, DISTANCIA_LATERAL - 20, 0, 0},
    c2_coords = {DISTANCIA_FRONTAL, DISTANCIA_LATERAL, 30, DISTANCIA_LATERAL - 20, 0, 0},
    c3_coords = {DISTANCIA_FRONTAL, -DISTANCIA_LATERAL, 30, DISTANCIA_LATERAL - 20, 0, 0},
    c4_coords = {-20, -DISTANCIA_LATERAL, 30, DISTANCIA_LATERAL - 20, 0, 0};
bool keyPressed[256];
bool specialPressed[256];
Mix_Music *musica;
bool playing_music = true;
Mix_Chunk *pasos;
int canal;
int HUDD=0;


/** OPENGL HANDLERS **/
void init(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void keyops();
void mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void HUD();

/** FUNCTION HEADERS **/
void showObjects();
void loadImage(const char *filename, GLuint *width, GLuint *height, ILubyte **data);
GLuint loadTexture(const char *filename);
int parseFile(const char *filename, Face **faces);
int getListAse(const char *filename);
void generateListMuralla();
void generateListTorre();
void generateListSuelo();
void generateListCielo();
void idle(void);
void calcNormal(GLfloat *normal, GLfloat *vertex1, GLfloat *vertex2, GLfloat *vertex3);
struct CameraCoords recalculateCamera();
void playPasos();
void showMulticam();



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
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(mouseMotion);
    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}

/** Internal function defintion **/
void init(void)
{
    GLfloat difusa[] = { 1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, difusa);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, difusa);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, difusa);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);

    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(22050, AUDIO_S16, 2, 512);

    glClearColor(1, 1, 1, 1);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glutFullScreen();
    glutIgnoreKeyRepeat(1);
    memset(keyPressed, 0, sizeof(keyPressed));
    memset(specialPressed, 0, sizeof(specialPressed));
    glutSetCursor(GLUT_CURSOR_NONE);

    /* Carga de texturas */
    texturaPiedra = loadTexture(STONE_TEX);
    texturaCielo = loadTexture(CIELO_TEX);
    texturaTierra = loadTexture(TIERRA_TEX);
    texturaArenisca = loadTexture(ARENISCA_TEX);

    /* Generacion de listas de objetos */
    generateListMuralla();
    generateListTorre();
    generateListSuelo();
    generateListCielo();
    listaFortaleza = getListAse(FORTALEZA_ASE);
    listaPozo = getListAse(POZO_ASE);
    listaCasa1 = getListAse(CASA1_ASE);
    listaCasa2 = getListAse(CASA2_ASE);

    /* Musica */
    musica = Mix_LoadMUS(MUSIC_SOUND);
    Mix_PlayMusic(musica,-1);
    pasos = Mix_LoadWAV(PASOS_SOUND);

    /*Transparencia panel */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

}

/**
* Funcion de carga de una imagen a partir de un fichero
*/
void loadImage(const char *filename, GLuint *width, GLuint *height, ILubyte **data){
    ILuint imgid;
    ilGenImages(1, &imgid);
    ilBindImage(imgid);
    ilLoadImage((const ILstring)filename);
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    *width = ilGetInteger(IL_IMAGE_WIDTH);
    *height = ilGetInteger(IL_IMAGE_HEIGHT);
    *data = ilGetData();
    ilDeleteImages(1, &imgid);
}

/**
* Funcion de carga de una textura.
* Carga y configura una textura a partir de un fichero de imagen
*/
GLuint loadTexture(const char *filename){
    GLuint width, height, textura;
    ILubyte *data;
    loadImage(filename, &width, &height, &data);
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,  GL_RGBA, GL_UNSIGNED_BYTE, data);
    return textura;
}

/**
* Funcion de dibujado de los objetos en pantalla
*/
void showObjects(){
    // Posición de los focos de luz
    GLfloat lightPosition[][4] = {
    { -20.0f, DISTANCIA_LATERAL, 20.0f, 0},
    { DISTANCIA_FRONTAL, DISTANCIA_LATERAL, 20.0f, 0},
    { DISTANCIA_FRONTAL, -DISTANCIA_LATERAL, 20.0f, 0},
    { -20.0f, -DISTANCIA_LATERAL, 20.0f, 0}
    };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition[0]); // a la luz 0
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition[1]); // a la luz 1
    glLightfv(GL_LIGHT2, GL_POSITION, lightPosition[2]); // a la luz 2
    glLightfv(GL_LIGHT3, GL_POSITION, lightPosition[3]); // a la luz 3

    // Dibujado del cielo y el suelo
    glEnable(GL_TEXTURE_2D);
    glColor3ub(255, 255, 255);
    glPushMatrix();
    glCallList(listaSuelo);
    glCallList(listaCielo);
    glPopMatrix();

    // Dibujado de la muralla
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texturaPiedra);
    glTranslatef(-20, DISTANCIA_LATERAL, 0);
    for(int i = 0; i < 4; i++){
        int lim = MURALLA_ELEMENTS;
        if(i%2 > 0){
            lim = FRENTE_ELEMENTS;
        }
        for(int j = 0; j < lim; j++){
            glCallList(listaMuralla);
            glTranslatef(5, 0, 0);
        }
        glCallList(listaTorre);
        glRotatef(-90, 0, 0, 1);
    }
    glPopMatrix();

    // Dibujado del castillo
    glDisable(GL_TEXTURE_2D);
    glColor3ub(104, 105, 91);
    glPushMatrix();
    glTranslatef(DISTANCIA_FRONTAL - 25, DISTANCIA_LATERAL / 4, 0);
    glScalef(0.03,0.03,0.03);
    glRotatef(-90, 0, 0, 1);
    glCallList(listaFortaleza);
    glPopMatrix();

    // Dibujado de los edificios
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(5,DISTANCIA_LATERAL-10, 0);
    glRotatef(-30, 0, 0, 1);
    glScalef(0.02, 0.02, 0.02);
    glCallList(listaCasa2);
    glPopMatrix();

    glColor3ub(130, 82, 1);
    glPushMatrix();
    glTranslatef(40,DISTANCIA_LATERAL-6, 0);
    glScalef(0.03, 0.03, 0.03);
    glCallList(listaCasa1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(30, -DISTANCIA_LATERAL+3, 0);
    glRotatef(180, 0 , 0, 1);
    glScalef(0.03, 0.03, 0.03);
    glCallList(listaCasa1);
    glPopMatrix();

    // Dibujado de la fuente
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaArenisca);
    glColor3ub(255, 255, 255);
    glPushMatrix();
    glTranslatef(10, -20, 0);
    glScalef(0.03,0.03,0.03);
    glCallList(listaPozo);
    glPopMatrix();

    // Ejes de coordenadas
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glLineWidth(3);
    glPushMatrix();
    glBegin(GL_LINES);
        glColor3ub(255, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(3, 0, 0);
        glColor3ub(0, 255, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0,3,0);
        glColor3ub(0, 0, 255);
        glVertex3f(0, 0, 0);
        glVertex3f(0,0,3);
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}
/**
*Funcion para imprimir texto por pantalla
*/
void displayString(char *s)
{
    for (int i = 0; i < strlen (s); i++)
    {
        glutBitmapCharacter (GLUT_BITMAP_HELVETICA_10, s[i]);
    }
}
/**
*Vista Ortogonal
*/
void OrthographicProjection()
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,width1,height1,0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}
void resetPerspectiveProjection()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}
/**
*Panel de Informacion
*/
void HUD(){
    if(HUDD==1)
    {
        width1 = glutGet(GLUT_WINDOW_WIDTH);
        height1  = glutGet(GLUT_WINDOW_HEIGHT);
        char cadena[]="Salida";
        OrthographicProjection();
        glEnd();
        glColor4f(0.0,0.0,0.0,0.7);
        glBegin(GL_QUADS);
        glVertex2f(10,10);
        glVertex2f(220.0,10);
        glVertex2f(220.0,150.0);
        glVertex2f(10.0,150.0);
        glEnd();
        glColor4f(1.0,1.0,1.0,1.0);
        glRasterPos2i(30,30);
        displayString("TECLAS:\n");
        glRasterPos2i(30,40);
        displayString("W: avance");
        glRasterPos2i(30,50);
        displayString("S: retorceso");
        glRasterPos2i(30,60);
        displayString("A: movimiento a la izquierda");
        glRasterPos2i(30,70);
        displayString("D: movimiento a la derecha");
        glRasterPos2i(30,80);
        displayString("C: alternar vista entre camaras");
        glRasterPos2i(30,90);
        displayString("F: guardar imagen");
        glRasterPos2i(30,100);
        displayString("R: volver a posicion inicial");
        glRasterPos2i(30,110);
        displayString("Esc: salir");
        glRasterPos2i(30,120);
        displayString("M: deterner musica y reanudar la musica");
        glRasterPos2i(30,130);
        displayString("H: cerrar panel informacion");
        glRasterPos2i(30,140);
        displayString("P: cambiar vista");

        resetPerspectiveProjection();
    }


}

/**
* Handler de display
*/
void display(void){
    keyops();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    struct CameraCoords cam = recalculateCamera();
    if (camara == MULTICAM){
        // multicamara
        showMulticam();
    }else{
        glViewport(0, 0, screen_width, screen_height);
        // Posicionado de la cámara
        // Posicionado de la cámara
        gluLookAt(
            cam.eye_x, // Coordenada X de la cámara
            cam.eye_y, // Coordenada Y de la cámara
            cam.eye_z, // Coordenada Z de la cámara
            cam.at_x, /* Puntos donde mira la cámara */
            cam.at_y,
            cam.at_z,
            0.0, /* Orientación de la cámara */
            0.0,
            1);
        showObjects();
    }
    HUD();
    glutSwapBuffers();
}

/**
* Funcion que muestra la multicamara en varios viewports
*/
void showMulticam(){
    glViewport(0, 0, screen_width / 2, screen_height / 2);
    glLoadIdentity ();
    gluLookAt(
        c1_coords.eye_x, // Coordenada X de la cámara
        c1_coords.eye_y, // Coordenada Y de la cámara
        c1_coords.eye_z, // Coordenada Z de la cámara
        c1_coords.at_x, /* Puntos donde mira la cámara */
        c1_coords.at_y,
        c1_coords.at_z,
        0.0, /* Orientación de la cámara */
        0.0,
        1);
    showObjects();

    glViewport(screen_width / 2, 0, screen_width / 2, screen_height / 2);
    glLoadIdentity ();
    gluLookAt(
        c2_coords.eye_x, // Coordenada X de la cámara
        c2_coords.eye_y, // Coordenada Y de la cámara
        c2_coords.eye_z, // Coordenada Z de la cámara
        c2_coords.at_x, /* Puntos donde mira la cámara */
        c2_coords.at_y,
        c2_coords.at_z,
        0.0, /* Orientación de la cámara */
        0.0,
        1);
    showObjects();

    glViewport(0, screen_height / 2, screen_width / 2, screen_height / 2);
    glLoadIdentity ();
    gluLookAt(
        c3_coords.eye_x, // Coordenada X de la cámara
        c3_coords.eye_y, // Coordenada Y de la cámara
        c3_coords.eye_z, // Coordenada Z de la cámara
        c3_coords.at_x, /* Puntos donde mira la cámara */
        c3_coords.at_y,
        c3_coords.at_z,
        0.0, /* Orientación de la cámara */
        0.0,
        1);
    showObjects();
    glViewport(screen_width / 2, screen_height / 2, screen_width / 2, screen_height / 2);
    glLoadIdentity ();
    gluLookAt(
        c4_coords.eye_x, // Coordenada X de la cámara
        c4_coords.eye_y, // Coordenada Y de la cámara
        c4_coords.eye_z, // Coordenada Z de la cámara
        c4_coords.at_x, /* Puntos donde mira la cámara */
        c4_coords.at_y,
        c4_coords.at_z,
        0.0, /* Orientación de la cámara */
        0.0,
        1);
    showObjects();
}

/**
* Funcion que se llamara cada vez que se redimensione la ventana
*/
void reshape(int w, int h)
{
    screen_width = w;
    screen_height = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, w/(GLdouble)h, 0.1, 1000);
}

/**
* Funcion de almacenamiento de las pulsaciones de teclas.
*/
void keyboard(unsigned char key, int x, int y){
    keyPressed[key] = true;
}

/**
* Funcion que maneja el fin de la pulsacion de teclas.
*/
void keyboardUp(unsigned char key, int x, int y){
    keyPressed[key] = false;
}

/**
* Funcion de almacenamiento de pulsaciones de teclas especiales
*/
void specialKey(int key, int x, int y){
    specialPressed[key] = true;
}

/**
* Funcion que maneja el fin de la pulsación
*/
void specialKeyUp(int key, int x, int y){
    specialPressed[key] = false;
}

/**
* Manejo avanzado del teclado
*/
void keyops(){
    // Tecla ESC
    if (keyPressed[27]){
        exit(0);
    }

    // Funciones de movimiento con teclas
    if (keyPressed['w'] || keyPressed['W']){
        fp_coords.eye_x += COS_INCREMENT;
        fp_coords.eye_y += SIN_INCREMENT;
        playPasos();
    }
    if (keyPressed['s'] || keyPressed['S']){
        fp_coords.eye_x -= COS_INCREMENT;
        fp_coords.eye_y -= SIN_INCREMENT;
        playPasos();
    }
    if (keyPressed['d'] || keyPressed['D']){
        fp_coords.eye_x += SIN_INCREMENT;
        fp_coords.eye_y -= COS_INCREMENT;
        playPasos();
    }
    if (keyPressed['a'] || keyPressed['A']){
        fp_coords.eye_x -= SIN_INCREMENT;
        fp_coords.eye_y += COS_INCREMENT;
        playPasos();
    }

    // Cambio de camara
    if (keyPressed['c'] || keyPressed['C']){
        camara = (CameraType)((camara + 1) % NUM_CAMS);
        keyPressed['c'] = false;
        keyPressed['C'] = false;
    }

    if (keyPressed['p'] || keyPressed['P']){
        plane_mode = true;
        keyPressed['p'] = false;
        keyPressed['P'] = false;
    }

    if (keyPressed['m'] || keyPressed['M']){
        keyPressed['m'] = false;
        keyPressed['M'] = false;
        if(playing_music){
            Mix_HaltMusic();
        }else{
            Mix_PlayMusic(musica , -1);
        }
        playing_music = !playing_music;
    }
    // Panel de informacion
    if (keyPressed['h'] || keyPressed['H']){
            if(HUDD==1){ HUDD=0; }
                else if(HUDD==0)
                    { HUDD=1; }
    }

    /* Teclas especiales */
    // Movimiento del observador
    if(specialPressed[GLUT_KEY_UP]){
        if (beta < (I_90_DG - ANGLE_INCREMENT)){
            beta+=ANGLE_INCREMENT;
        }
    }
    if(specialPressed[GLUT_KEY_DOWN]){
        if (beta > (-I_90_DG + ANGLE_INCREMENT)){
            beta-=ANGLE_INCREMENT;
        }
    }
    if(specialPressed[GLUT_KEY_LEFT]){
        alpha+=ANGLE_INCREMENT;
        if (alpha > I_360_DG){
            alpha -= I_360_DG;
        }
    }
    if(specialPressed[GLUT_KEY_RIGHT]){
        alpha-=ANGLE_INCREMENT;
        if (alpha < I_360_DG){
            alpha += I_360_DG;
        }
    }

}

/**
* Manejo de botones de raton
*/
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON){
        beta = INITIAL_BETA;
    }
    if (button == GLUT_RIGHT_BUTTON){
        alpha = INITIAL_ALPHA;
    }
    glutPostRedisplay();
}

/**
* Manejo de movimiento de raton
*/
void mouseMotion(int x, int y){
    if (camara == FP_CAM){
        if (x < mouse_x || x == 0){
            alpha += ANGLE_INCREMENT * 2;
        }else if (x >  mouse_x || x == screen_width - 1){
            alpha -= ANGLE_INCREMENT * 2;
        }

        if (y < mouse_y || y == 0){
            beta += ANGLE_INCREMENT;
        }else if (y > mouse_y || y == screen_height - 1){
            beta -= ANGLE_INCREMENT;
        }
        mouse_x = x;
        mouse_y = y;
    }else{
        mouse_x = 0;
        mouse_y = 0;
    }
    glutPostRedisplay();
}

void idle(void)
{
    glutPostRedisplay();
}

/**
* Generación de una lista de dibujado de un fichero ASE
*/
int getListAse(const char *filename) {

    GLuint aseList = glGenLists(1);

    Face *facesAse;

    int numFaces = parseFile(filename, &facesAse);
    glNewList(aseList, GL_COMPILE);
        glPushMatrix();
        for (int i = 0; i<numFaces; i++){
             facesAse[i].render();
        }
        glPopMatrix();
    glEndList();

    free(facesAse);

    return aseList;
}

/**
* Extraccion de los datos de las figuras de un fichero ASE
*/
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

/**
* Calculo de la normal de una superficie
*/
void calcNormal(GLfloat *normal, GLfloat *vertex1, GLfloat *vertex2, GLfloat *vertex3){
    float px = vertex2[0] - vertex1[0];
    float py = vertex2[1] - vertex1[1];
    float pz = vertex2[2] - vertex1[2];
    float qx = vertex3[0] - vertex1[0];
    float qy = vertex3[1] - vertex1[1];
    float qz = vertex3[2] - vertex1[2];
    float nx = py*qz - pz*qy;
    float ny = -px*qz + pz*qx;
    float nz = px*qy - py*qx;
    float nmod = sqrt(nx*nx+ny*ny+nz*nz);
    normal[0] = nx / nmod;
    normal[1] = ny / nmod;
    normal[2] = nz / nmod;
}

/**
* Generacion de la lista de dibujado del fragmento de muralla.
*/
void generateListMuralla(){

    listaMuralla = glGenLists(1);

    /** Coordenadas de muro **/
    GLfloat coordsMuro1[][3] = {
        {0, 0, 0},
        {0, 0, 10},
        {5, 0, 10},
        {5, 0, 0}
    };

    GLfloat coordsMuro2[][3] = {
        {0, 0, 0},
        {0, 0, 10},
        {1, 0, 10},
        {1, 0, 0}
    };

    GLfloat coordsMuro3[][3] = {
        {0,0,0},
        {0,5,0},
        {1,5,0},
        {1,0,0}
    };


    GLfloat normalMuro1[3];
    GLfloat normalMuro2[3];
    GLfloat normalMuro3[3];
    calcNormal(normalMuro1, coordsMuro1[0], coordsMuro1[1], coordsMuro1[2]);
    calcNormal(normalMuro2, coordsMuro2[0], coordsMuro2[1], coordsMuro2[2]);
    calcNormal(normalMuro3, coordsMuro3[0], coordsMuro3[1], coordsMuro3[2]);

    /** Coordenadas de la almena **/
    GLfloat coordsAlmena1[][3] = {
        {0,0,0},
        {0,0,2},
        {2.5,0,2},
        {2.5,0,0},
    };

    GLfloat coordsAlmena2[][3] = {
        {0, 0, 0},
        {0, 0, 2},
        {1, 0, 2},
        {1, 0, 0}
    };

    GLfloat coordsAlmena3[][3] = {
        {0, 0, 0},
        {0, 0, 2},
        {1, 0, 2},
        {1, 0, 0}
    };

    GLfloat normalAlmena1[3];
    GLfloat normalAlmena2[3];
    GLfloat normalAlmena3[3];
    calcNormal(normalAlmena1, coordsAlmena1[0], coordsAlmena1[1], coordsAlmena1[2]);
    calcNormal(normalAlmena2, coordsAlmena2[0], coordsAlmena2[1], coordsAlmena2[2]);
    calcNormal(normalAlmena3, coordsAlmena3[0], coordsAlmena3[1], coordsAlmena3[2]);
    glNewList(listaMuralla, GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, texturaPiedra);
        /** Primitivas del muro **/
        glPushMatrix();
            for(int i = 0; i < 2; i++){
                glBegin(GL_QUADS);
                glNormal3fv(normalMuro1);
                for (int j = 0; j < COORDS_ITERATIONS(coordsMuro1); j++){
                    glTexCoord2f(coordsMuro1[j][0]/2, coordsMuro1[j][2]/2);
                    glVertex3fv(coordsMuro1[j]);
                }
                glEnd();
                glTranslatef(5, -1, 0);
                glRotatef(180, 0, 0, 1);
            }
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, -1, 0);
            glRotatef(90, 0, 0, 1);
            for(int i = 0; i < 2; i++){
                glBegin(GL_QUADS);
                glNormal3fv(normalMuro2);
                for (int j = 0; j < COORDS_ITERATIONS(coordsMuro2); j++){
                    glTexCoord2f(coordsMuro2[j][0]/2, coordsMuro2[j][2]/2);
                    glVertex3fv(coordsMuro2[j]);
                }
                glEnd();
                glTranslatef(1, -5, 0);
                glRotatef(180, 0, 0, 1);
            }
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, 0, 10);
            glRotatef(180, 1, 0, 0);
            glRotatef(-90, 0, 0, 1);
            glTranslatef(-1, 0, 0);
            glBegin(GL_QUADS);
            glNormal3fv(normalMuro3);
            for (int j = 0; j < COORDS_ITERATIONS(coordsMuro3); j++){
                glTexCoord2f(coordsMuro3[j][0]/2, coordsMuro3[j][1]/2);
                glVertex3fv(coordsMuro3[j]);
            }
            glEnd();
        glPopMatrix();

        /** Primitivas de la almena **/
        glPushMatrix();
            glTranslatef(0, 0, 10);
            for(int i = 0; i < 2; i++){
                glBegin(GL_QUADS);
                glNormal3fv(normalAlmena1);
                for (int j = 0; j < COORDS_ITERATIONS(coordsAlmena1); j++){
                    glTexCoord2f(coordsAlmena1[j][0]/2, coordsAlmena1[j][2]/2);
                    glVertex3fv(coordsAlmena1[j]);
                }
                glEnd();
                glTranslatef(2.5, -1, 0);
                glRotatef(180, 0, 0, 1);
            }
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0, -1, 10);
            glRotatef(90, 0, 0, 1);
            for(int i = 0; i < 2; i++){
                glBegin(GL_QUADS);
                glNormal3fv(normalAlmena2);
                for (int j = 0; j < COORDS_ITERATIONS(coordsAlmena2); j++){
                    glTexCoord2f(coordsAlmena2[j][0]/2, coordsAlmena2[j][2]/2);
                    glVertex3fv(coordsAlmena2[j]);
                }
                glEnd();
                glTranslatef(1, -2.5, 0);
                glRotatef(180, 0, 0, 1);
            }
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0, 0, 12);
            glRotatef(180, 1, 0, 0);
            glRotatef(-90, 0, 0, 1);
            glTranslatef(-1, 0, 0);
            glBegin(GL_QUADS);
            glNormal3fv(normalAlmena3);
            for (int j = 0; j < COORDS_ITERATIONS(coordsAlmena3); j++){
                glTexCoord2f(coordsAlmena3[j][0]/2, coordsAlmena3[j][1]/2);
                glVertex3fv(coordsAlmena3[j]);
            }
            glEnd();
        glPopMatrix();
    glEndList();
}

/**
* Generacion de la lista de dibujado de la torre lateral de la muralla.
*/
void generateListTorre(){
    listaTorre = glGenLists(1);

    GLfloat coordsMuro[][3] = {
        {0, 0, 0},
        {0, 0, 20},
        {5, 0, 20},
        {5, 0, 0}
    };

    GLfloat coordsTapa[][3] = {
        {0, 0, 0},
        {0, 5, 0},
        {5, 5, 0},
        {5, 0, 0}
    };

    GLfloat normalMuro[3];
    GLfloat normalTapa[3];
    calcNormal(normalMuro, coordsMuro[0], coordsMuro[1], coordsMuro[2]);
    calcNormal(normalTapa, coordsTapa[0], coordsTapa[1], coordsTapa[2]);
    glNewList(listaTorre, GL_COMPILE);
        glPushMatrix();
            glTranslatef(-5, 0, 0);
            for (int i = 0; i < 4;  i++){
                glBegin(GL_QUADS);
                glNormal3fv(normalMuro);
                for(int j = 0; j < COORDS_ITERATIONS(coordsMuro); j++){
                    glTexCoord2f(coordsMuro[j][0]/2, coordsMuro[j][2]/2);
                    glVertex3fv(coordsMuro[j]);
                }
                glEnd();
                glTranslatef(5, 0, 0);
                glRotatef(-90, 0, 0, 1);
            }
            glTranslatef(0,0,20);
            glRotatef(180, 1, 0, 0);
            glRotatef(-90, 0, 0, 1);
            glTranslatef(-5, 0, 0);
            glBegin(GL_QUADS);
            glNormal3fv(normalTapa);
            for(int j = 0; j < COORDS_ITERATIONS(coordsTapa); j++){
                glTexCoord2f(coordsTapa[j][0]/2, coordsTapa[j][1]/2);
                glVertex3fv(coordsTapa[j]);
            }
            glEnd();
        glPopMatrix();
    glEndList();
}

/**
* Generacion de la lista de dibujado del suelo.
*/
void generateListSuelo(){
    listaSuelo = glGenLists(1);
    GLfloat coordsSuelo[][3] = {
        {0,0,0},
        {0,-100000,0},
        {100000,100000,0},
        {-100000,0,0},
    };
    GLfloat normalSuelo[3];
    calcNormal(normalSuelo, coordsSuelo[0], coordsSuelo[1], coordsSuelo[2]);
    glNewList(listaSuelo, GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, texturaTierra);
        glPushMatrix();
        glBegin(GL_QUADS);
        glNormal3fv(normalSuelo);
         for(int j = 0; j < COORDS_ITERATIONS(coordsSuelo); j++){
            glTexCoord2f(coordsSuelo[j][0]/2, coordsSuelo[j][1]/2);
            glVertex3fv(coordsSuelo[j]);
        }
        glEnd();
        glPopMatrix();
    glEndList();
}

/**
* Generacion de la lista de dibujado del cielo
*/
void generateListCielo(){
    listaCielo = glGenLists(1);
    GLfloat coordsCielo[][3] = {
        {0,0,0},
        {0,100000,0},
        {-100000,-100000,0},
        {100000,0,0},
    };
    GLfloat normalCielo[3];
    calcNormal(normalCielo, coordsCielo[0], coordsCielo[1], coordsCielo[2]);
    glNewList(listaCielo, GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, texturaCielo);
        glPushMatrix();
        glTranslatef(0,0,100);
        glBegin(GL_QUADS);
        glNormal3fv(normalCielo);
         for(int j = 0; j < COORDS_ITERATIONS(coordsCielo); j++){
            glTexCoord2f(coordsCielo[j][0]/100, coordsCielo[j][1]/100);
            glVertex3fv(coordsCielo[j]);
        }
        glEnd();
        glPopMatrix();
    glEndList();
}

/**
* Recalcula las coordenadas de las camaras moviles
*/
struct CameraCoords recalculateCamera(){
    // Recalculo de las coordenadas de la cámara en primera persona
    fp_coords.at_x = fp_coords.eye_x + cos(alpha) * cos(beta);
    fp_coords.at_y = fp_coords.eye_y + cos(beta) * sin(alpha);
    fp_coords.at_z = INITIAL_HEIGHT + sin(beta);


    // Recalculo de las coordenadas de la camara giratoria
    alpha_rot += ANGLE_INCREMENT;
    if (alpha_rot > I_360_DG){
        alpha_rot -= I_360_DG;
    }
    rot_coords.at_x = rot_coords.eye_x + cos(alpha_rot);
    rot_coords.at_y = rot_coords.eye_y + sin(alpha_rot);
    rot_coords.at_z = INITIAL_HEIGHT;

    switch (camara){
    case FP_CAM:
    default:
        return fp_coords;
    case C1_CAM:
        return c1_coords;
    case C2_CAM:
        return c2_coords;
    case C3_CAM:
        return c3_coords;
    case C4_CAM:
        return c4_coords;
    case ROT_CAM:
        return rot_coords;
    }
}

/**
* Reproduce el sonido de pasos
*/
void playPasos(){
    if (!Mix_Playing(canal)){
        canal = Mix_PlayChannel(-1, pasos, 0);
    }
}
