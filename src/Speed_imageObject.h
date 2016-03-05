
#ifndef SPEEDIMAGEOBJECT_H
#define	SPEEDIMAGEOBJECT_H

#include <GL/glew.h>
#include "math_3d.h"
class SpeedImageObject
{
public:
    SpeedImageObject();

    ~SpeedImageObject();

    bool Init(unsigned int GridWidth, unsigned int GridHeight);

    void BindForImageUnitWriting(GLenum ImageUnit);

	void BindForImageUnitReading(GLenum ImageUnit); 

	void BindForReading(GLenum TextureUnit); 

private:

    GLuint m_speed_texture; //texture2d HANDLE WILL BE BIND AS IMAGE UNIT
};

#endif	

