#include <stdio.h>

#include "Density_Velocity_fbo.h"
#include "util.h"

DensityVelocityFBO::DensityVelocityFBO()
{
    m_fbo = 0;
    m_density_velocity = 0;
}

DensityVelocityFBO::~DensityVelocityFBO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_density_velocity != 0) {
        glDeleteTextures(1, &m_density_velocity);
    }
}

bool DensityVelocityFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glGenTextures(1, &m_density_velocity);
    glBindTexture(GL_TEXTURE_2D, m_density_velocity);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 128, 128, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_density_velocity, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }
	// Restore the default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void DensityVelocityFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void DensityVelocityFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_density_velocity);
}

void DensityVelocityFBO::BindForImageUnitReadingWriting(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_density_velocity, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}
void DensityVelocityFBO::BindForImageUnitReading(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_density_velocity, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
}
