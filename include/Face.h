/**
* Clase Face
* Encargada de almacenar los datos necesarios para realizar el pintado de la
* cara del objeto.
**/

#ifndef FACE_H
#define FACE_H

#include "Vertex.h"
#include "Normal.h"

class Face
{
    private:
        Vertex *vertex; // V�rtices que definen la cara
        int numVertex; // N�mero de v�rtices de la cara
        Normal *normal; // Normal de la cara
    public:
        Face(Vertex *vertex, int numVertex); // Constructor
        void setNormal(Normal *normal); // Establece la normal de la cara
        virtual ~Face(); // Destructor
        void render(); // Pinta la cara
};

#endif // FACE_H
