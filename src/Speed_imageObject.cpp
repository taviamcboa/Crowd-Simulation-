#include <stdio.h>

#include "Speed_imageObject.h"
#include "util.h"

SpeedImageObject::SpeedImageObject()
{
    m_speed_texture = 0;
}

SpeedImageObject::~SpeedImageObject()
{
    if (m_speed_texture != 0) {
        glDeleteTextures(1, &m_speed_texture);
    }
}

bool SpeedImageObject::Init(unsigned int GridWidth, unsigned int GridHeight)
{
    glGenTextures(1, &m_speed_texture);
	glBindTexture(GL_TEXTURE_2D, m_speed_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_RGBA32F, GridWidth, GridHeight);

    return true;
}

void SpeedImageObject::BindForImageUnitWriting(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_speed_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void SpeedImageObject::BindForImageUnitReading(GLenum ImageUnit)
{
     glBindImageTexture(ImageUnit, m_speed_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
}
void SpeedImageObject::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_speed_texture);
}
