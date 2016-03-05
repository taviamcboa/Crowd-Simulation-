
#ifndef POTENTIALIMAGEOBJECT_H
#define	POTENTIALIMAGEOBJECT_H

#include <GL/glew.h>
#include "math_3d.h"

class PotentialImageObject
{
public:
    PotentialImageObject();

    ~PotentialImageObject();

    bool Init(unsigned int GridWidth, unsigned int GridHeight);

	void BindForImageUnitWritingOnly(GLenum ImageUnit);

    void BindForImageUnitReadingWriting(GLenum ImageUnit);

	void BindForImageUnitReadingOnly(GLenum ImageUnit); 

	void BindForReading(GLenum TextureUnit); 

	void BindForImageBufferWritingOnly(GLenum ImageUnit); 
	void BindForNeighboringPotentialImageBufferWritingOnly(GLenum ImageUnit); 
	void BindForNeighboringCostImageBufferWritingOnly(GLenum ImageUnit); 

	GLuint getWriteBuffer(){
		return m_potential_writebuffer; 
	}
	GLuint getNeighboringPotentialWriteBuffer(){
		return m_neighboring_potential_writebuffer; 
	}
	GLuint getNeighboringCostWriteBuffer(){
		return m_neighboring_cost_writebuffer; 
	}
private:

    GLuint m_potential_texture; //texture2d HANDLE WILL BE BIND AS IMAGE UNIT

	GLuint m_potential_writebuffer; 
	GLuint m_potential_writetexture; 

	GLuint m_neighboring_potential_writebuffer; 
	GLuint m_neighboring_potential_writetexture; 

	GLuint m_neighboring_cost_writebuffer; 
	GLuint m_neighboring_cost_writetexture; 

};

#endif	

