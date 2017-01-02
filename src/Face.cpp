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
