#include <stdio.h>

#include "Gradient_imageObject.h"
#include "util.h"

GradientImageObject::GradientImageObject()
{
    m_gradient_texture = 0;
}

GradientImageObject::~GradientImageObject()
{
    if (m_gradient_texture != 0) {
        glDeleteTextures(1, &m_gradient_texture);
    }
	if (m_gradient_writetexture != 0) {
        glDeleteTextures(1, &m_gradient_writetexture);
    }
	if (m_gradient_writebuffer != 0) {
        glDeleteBuffers(1, &m_gradient_writebuffer);
    }
}

bool GradientImageObject::Init(unsigned int GridWidth, unsigned int GridHeight)
{
    glGenTextures(1, &m_gradient_texture);
	glBindTexture(GL_TEXTURE_2D, m_gradient_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_RGBA32F, GridWidth, GridHeight);

	float *gridvalue = new float[128 * 128 *4]; 
	for(int i =0; i< 128 * 128 *4; i++)
	{
		gridvalue[i] = 0.0f; 
	}

	glGenBuffers(1, &m_gradient_writebuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, m_gradient_writebuffer);
	glBufferData(GL_TEXTURE_BUFFER, 128 *128 * 4*sizeof(float), gridvalue, GL_DYNAMIC_READ);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glGenTextures(1, &m_gradient_writetexture);
	glBindTexture(GL_TEXTURE_BUFFER, m_gradient_writetexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_gradient_writebuffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	delete[] gridvalue; 
    return true;
}

void GradientImageObject::BindForImageUnitWriting(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_gradient_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void GradientImageObject::BindForImageUnitReading(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_gradient_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
}

void GradientImageObject::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_gradient_texture);
}
void GradientImageObject::BindForBufferImageUnitWriting(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_gradient_writetexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
