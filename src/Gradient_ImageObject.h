
#ifndef GRADIENTIMAGEOBJECT_H
#define	GRADIENTIMAGEOBJECT_H

#include <GL/glew.h>
#include "math_3d.h"

class GradientImageObject
{
public:
    GradientImageObject();

    ~GradientImageObject();

    bool Init(unsigned int GridWidth, unsigned int GridHeight);

    void BindForImageUnitWriting(GLenum ImageUnit);

	void BindForBufferImageUnitWriting(GLenum ImageUnit); 

	void BindForImageUnitReading(GLenum ImageUnit); 

	void BindForReading(GLenum TextureUnit); 
	GLuint getWriteBuffer(){
		return m_gradient_writebuffer; 
	}
private:

    GLuint m_gradient_texture; //texture2d HANDLE WILL BE BIND AS IMAGE UNIT

	GLuint m_gradient_writebuffer; 
	GLuint m_gradient_writetexture; 

};

#endif	

