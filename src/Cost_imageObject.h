
#ifndef COSTIMAGEOBJECT_H
#define	COSTIMAGEOBJECT_H

#include <GL/glew.h>
#include "math_3d.h"

class CostImageObject
{
public:
    CostImageObject();

    ~CostImageObject();

    bool Init(unsigned int GridWidth, unsigned int GridHeight);

    void BindForImageUnitWriting(GLenum ImageUnit);

	void BindForImageUnitReading(GLenum ImageUnit); 

	void BindForReading(GLenum TextureUnit); 

private:

    GLuint m_cost_texture; //texture2d HANDLE WILL BE BIND AS IMAGE UNIT
};

#endif	

