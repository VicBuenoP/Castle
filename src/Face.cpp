/**
* Implementación de la case Face.
* Clase encargada de realizar el pintado de una cara del objeto
*/

#include "Face.h"
#include <stdio.h>
#include <GL/freeglut.h>

using namespace std;

Face::Face(Vertex *vertexList, int numVertex)
{
    this->vertex = vertexList;
    this->numVertex = numVertex;
    this->normal = NULL;
}

void Face::render()
{
    int i = 0;
    glBegin(GL_TRIANGLES);
    this->normal->render();
    for(i = 0; i < numVertex; i++)
    {
        /*if(i = 0){
            glTexCoord2f(0,0);
        }else if(i == 1){
            glTexCoord2f(0.5, 1);
        }else{
            glTexCoord2f(1, 1);
        }*/

        this->vertex[i].render();
    }
    glEnd();
}

void Face::setNormal(Normal *normal)
{
    this->normal = normal;
}

Face::~Face()
{
}
