/**
* Implementación de la clase normal.
*/

#include <GL/freeglut.h>
#include "Normal.h"

Normal::Normal(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Normal::render(){
    glNormal3f(this->x, this->y, this->z);
}

Normal::~Normal()
{
    //dtor
}
