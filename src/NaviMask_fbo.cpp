#include <stdio.h>

#include "NaviMask_fbo.h"
#include "util.h"

NaviMaskFBO::NaviMaskFBO()
{
    m_fbo = 0;
    m_navi_mask_map = 0;
}

NaviMaskFBO::~NaviMaskFBO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_navi_mask_map != 0) {
        glDeleteTextures(1, &m_navi_mask_map);
    }
}

bool NaviMaskFBO::Init(unsigned int g_width, unsigned int g_height)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glGenTextures(1, &m_navi_mask_map);
    glBindTexture(GL_TEXTURE_2D, m_navi_mask_map);

	//motion vector is out the range 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, g_width, g_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_navi_mask_map, 0);

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

void NaviMaskFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void NaviMaskFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_navi_mask_map);
}

void NaviMaskFBO::BindForImageReading(GLenum ImageUnit)
{
	glBindImageTexture(ImageUnit, m_navi_mask_map, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
}
