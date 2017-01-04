/**
* Clase Vertex.
* Clase encargada de almacenar y pintar los v�rtices de un objeto.
*/

#ifndef VERTEX_H
#define VERTEX_H


class Vertex
{
    public:
        Vertex(double x, double y, double z); // Constructor
        virtual ~Vertex(); // Destructor
        void render(); // Funci�n de pintado
        void printVertex(); // Funcion auxiliar para pintar las coordenadas del v�rtice
    public:
        double x, y, z; // Coordenadas
};

#endif // VERTEX_H
