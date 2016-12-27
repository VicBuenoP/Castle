/**
* Implementación de la clase Vertex
*/

#include <GL/freeglut.h>
#include "Vertex.h"
#include <stdio.h>

Vertex::Vertex(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Vertex::render()
{
   // printf("Pintando vertice %lf, %lf, %lf\n", this->x, this->y, this->z);
    glVertex3d(this->x, this->y, this->z);
}

Vertex::~Vertex()
{
    //dtor
}

void Vertex::printVertex(){
    printf("Vertice %lf, %lf, %lf\n", this->x, this->y, this->z);
}
