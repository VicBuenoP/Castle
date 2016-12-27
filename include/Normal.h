/**
* Clase Normal.
* Clase encargada de almacenar y pintar la normal de una face.
*/

#ifndef NORMAL_H
#define NORMAL_H


class Normal
{
    public:
        Normal(float x, float y, float z); // Constructor
        virtual ~Normal(); // Destructor
        void render(); // Pintado

    private:
        float x,y,z; // Coordenadas
};

#endif // NORMAL_H
