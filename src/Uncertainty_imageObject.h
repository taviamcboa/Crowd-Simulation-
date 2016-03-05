
#ifndef UNCERTAINTYIMAGEOBJECT_H
#define	UNCERTAINTYIMAGEOBJECT_H

#include <GL/glew.h>
#include "math_3d.h"
class UncertaintyImageObject
{
public:
    UncertaintyImageObject();

    ~UncertaintyImageObject();

    bool Init(unsigned int GridWidth, unsigned int GridHeight);

	void BindForImageUnitReadWrite(GLenum ImageUnit);

    void BindForImageUnitWriting(GLenum ImageUnit);

	void BindForImageUnitReading(GLenum ImageUnit); 

	void BindForReading(GLenum TextureUnit); 

private:

    GLuint m_uncertainty_texture; //texture2d HANDLE WILL BE BIND AS IMAGE UNIT
};

#endif	

