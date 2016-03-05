#include <stdio.h>

#include "Uncertainty_imageObject.h"
#include "util.h"

UncertaintyImageObject::UncertaintyImageObject()
{
    m_uncertainty_texture = 0;
}

UncertaintyImageObject::~UncertaintyImageObject()
{
    if (m_uncertainty_texture != 0) {
        glDeleteTextures(1, &m_uncertainty_texture);
    }
}

bool UncertaintyImageObject::Init(unsigned int GridWidth, unsigned int GridHeight)
{
    glGenTextures(1, &m_uncertainty_texture);
	glBindTexture(GL_TEXTURE_2D, m_uncertainty_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_RGBA8, GridWidth, GridHeight);

	return true;
}
void UncertaintyImageObject::BindForImageUnitReadWrite(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_uncertainty_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
}
void UncertaintyImageObject::BindForImageUnitWriting(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_uncertainty_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
}
void UncertaintyImageObject::BindForImageUnitReading(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_uncertainty_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
}

void UncertaintyImageObject::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_uncertainty_texture);
}