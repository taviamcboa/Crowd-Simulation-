/*
        Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>

#include "marker_sampling_fbo.h"
#include "util.h"

MarkerSamplingFBO::MarkerSamplingFBO()
{
    m_fbo = 0;
    m_markersamplingmap = 0;
	m_writeTex = 0;
	m_writebuffer = 0; 
}

MarkerSamplingFBO::~MarkerSamplingFBO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_markersamplingmap != 0) {
        glDeleteTextures(1, &m_markersamplingmap);
    }

	if (m_writeTex != 0) {
        glDeleteTextures(1, &m_writeTex);
    }

    if (m_writebuffer != 0) {
        glDeleteBuffers(1, &m_writebuffer);
    }
}

bool MarkerSamplingFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight, int agentnum, int markernum, bool debug)
{
	this->m_debug = debug;
	this->m_markernum = markernum;
	this->m_agentnum = agentnum; 

    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glGenTextures(1, &m_markersamplingmap);
    glBindTexture(GL_TEXTURE_2D, m_markersamplingmap);

	glGenTextures(1, &m_depthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WindowWidth, WindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_markersamplingmap, 0);

	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };

    glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	if(this->m_debug == true)
	{
		glGenBuffers(1, &m_writebuffer);
		glBindBuffer(GL_TEXTURE_BUFFER, m_writebuffer);
		//glBufferData(GL_TEXTURE_BUFFER, sizeof(float), NULL, GL_DYNAMIC_READ);
		//glBindBuffer(GL_TEXTURE_BUFFER, 0);
		glBufferData(GL_TEXTURE_BUFFER, 4 * markernum*sizeof(float), NULL, GL_DYNAMIC_READ);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);

		//floating texture
		glGenTextures(1, &m_writeTex);
		glBindTexture(GL_TEXTURE_BUFFER, m_writeTex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_writebuffer);
		glBindTexture(GL_TEXTURE_BUFFER, 0);
	}

    return true;
}


void MarkerSamplingFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}


void MarkerSamplingFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_markersamplingmap);
}

void MarkerSamplingFBO::BindForTextureWriting(GLenum TextureUnit, GLenum TextureUnit_Index)
{
	//miplevel = 0
	glBindImageTexture(TextureUnit_Index, m_writeTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	//glActiveTexture(TextureUnit);
 //   glBindTexture(GL_TEXTURE_2D, m_writeTex);
}
