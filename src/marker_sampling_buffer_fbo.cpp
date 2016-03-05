#include <stdio.h>

#include "marker_sampling_buffer_fbo.h"
#include "util.h"

MarkerSamplingBufferFBO::MarkerSamplingBufferFBO()
{
    m_fbo = 0;
    m_markersamplingmap = 0;
}

MarkerSamplingBufferFBO::~MarkerSamplingBufferFBO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_markersamplingmap != 0) {
        glDeleteTextures(1, &m_markersamplingmap);
    }
}

bool MarkerSamplingBufferFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glGenTextures(1, &m_markersamplingmap);
    glBindTexture(GL_TEXTURE_2D, m_markersamplingmap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WindowWidth, WindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_markersamplingmap, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };

    glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}


void MarkerSamplingBufferFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}


void MarkerSamplingBufferFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_markersamplingmap);
}

void MarkerSamplingBufferFBO::BindForImageReading(GLenum ImageUnit)
{
	glBindImageTexture(ImageUnit, m_markersamplingmap, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
}
