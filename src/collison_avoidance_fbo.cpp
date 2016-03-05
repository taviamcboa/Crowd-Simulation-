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

#include "collison_avoidance_fbo.h"
#include "util.h"

CollisionAvoidanceFBO::CollisionAvoidanceFBO()
{
    m_fbo = 0;
    m_collisionavoidancemap = 0;

	m_writeTex = 0;
	m_writebuffer = 0;

	m_writeDensityBuffer = 0; 
	m_writeDensityBufferTex = 0; 
	m_writeDensityTex = 0; 

	m_densityfbo = 0; 
	m_antigradientfbo = 0; 
	m_writeAntiGradientTex = 0; 
}

CollisionAvoidanceFBO::~CollisionAvoidanceFBO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

	if (m_densityfbo != 0) {
        glDeleteFramebuffers(1, &m_densityfbo);
    }

	if (m_antigradientfbo != 0) {
        glDeleteFramebuffers(1, &m_antigradientfbo);
    }

	if (m_writeAntiGradientTex != 0) {
        glDeleteTextures(1, &m_writeAntiGradientTex);
    }

    if (m_collisionavoidancemap != 0) {
        glDeleteTextures(1, &m_collisionavoidancemap);
    }

	if (m_writeTex != 0) {
        glDeleteTextures(1, &m_writeTex);
    }

    if (m_writebuffer != 0) {
        glDeleteBuffers(1, &m_writebuffer);
    }

	if (m_writeDensityBufferTex != 0) {
        glDeleteTextures(1, &m_writeDensityBufferTex);
    }

	if (m_writeDensityBuffer != 0) {
        glDeleteBuffers(1, &m_writeDensityBuffer);
    }

	if (m_writeDensityTex != 0) {
        glDeleteTextures(1, &m_writeDensityTex);
    }
}

bool CollisionAvoidanceFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glGenTextures(1, &m_collisionavoidancemap);
    glBindTexture(GL_TEXTURE_2D, m_collisionavoidancemap);

	glGenTextures(1, &m_depthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_collisionavoidancemap, 0);

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
	 // // Restore the default framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glGenBuffers(1, &m_writebuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, m_writebuffer);

	float *inital = new float[600*4];
	for(int i =0; i< 2400; i++)
	{
		inital[i] = -99.0f; 
	}

	//glBufferData(GL_TEXTURE_BUFFER, sizeof(float), NULL, GL_DYNAMIC_READ);
	//glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBufferData(GL_TEXTURE_BUFFER, 600*4*sizeof(float), (GLvoid*)inital, GL_DYNAMIC_READ);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glGenTextures(1, &m_writeTex);
	glBindTexture(GL_TEXTURE_BUFFER, m_writeTex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_writebuffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	//glBufferSubData(GL_TEXTURE_BUFFER, 0, 0, Displacement);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	delete[] inital; 

	glGenBuffers(1, &m_writeDensityBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, m_writeDensityBuffer);
	int *density_initial = new int[64 * 64];
	for(int i =0; i< 64 * 64; i++)
	{
		density_initial[i] = 0; 
	}
	glBufferData(GL_TEXTURE_BUFFER, 64 * 64 *sizeof( int), (GLvoid*)density_initial, GL_DYNAMIC_READ);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glGenTextures(1, &m_writeDensityBufferTex);
	glBindTexture(GL_TEXTURE_BUFFER, m_writeDensityBufferTex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, m_writeDensityBuffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	glGenTextures(1, &m_writeDensityTex);
	glBindTexture(GL_TEXTURE_2D, m_writeDensityTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_R32I, 64, 64);

	//
	glGenFramebuffers(1, &m_densityfbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_densityfbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_writeDensityTex, 0);
	glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);
    Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }
	 // // Restore the default framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	
	glGenTextures(1, &m_writeAntiGradientTex);
	glBindTexture(GL_TEXTURE_2D, m_writeAntiGradientTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_RGBA8I, 64, 64);

	glGenFramebuffers(1, &m_antigradientfbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_antigradientfbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_writeAntiGradientTex, 0);
	glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);
    Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }
	 // // Restore the default framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	delete[] density_initial; 
    return true;
}

void CollisionAvoidanceFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}
void CollisionAvoidanceFBO::BindForWritingDensityFrameBuffer()
{
	 glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_densityfbo);
}

void CollisionAvoidanceFBO::BindForWritingAntiGradientFrameBuffer()
{
	 glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_antigradientfbo);
}

void CollisionAvoidanceFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_collisionavoidancemap);
}
void CollisionAvoidanceFBO::BindForDensityImageReading(GLenum TextureUnit_Index)
{
	glBindImageTexture(TextureUnit_Index, m_writeDensityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32I);
}

void CollisionAvoidanceFBO::BindForAntiGradientImageReading(GLenum TextureUnit_Index)
{
	glBindImageTexture(TextureUnit_Index, m_writeAntiGradientTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8I);
}

void CollisionAvoidanceFBO::BindForTextureWriting(GLenum TextureUnit, GLenum TextureUnit_Index)
{
	//miplevel = 0
	glBindImageTexture(TextureUnit_Index, m_writeTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	//glActiveTexture(TextureUnit);
 //   glBindTexture(GL_TEXTURE_2D, m_writeTex);
}

void CollisionAvoidanceFBO::BindForTextureReading(GLenum TextureUnit, GLenum TextureUnit_Index)
{
		
	//miplevel = 0
	glBindImageTexture(TextureUnit_Index, m_writeTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	
		
}//REAL IMAGESTORE
void CollisionAvoidanceFBO::BindForDensityWriting(GLenum TextureUnit, GLenum TextureUnit_Index)
{
	//miplevel = 0
	glBindImageTexture(TextureUnit_Index, m_writeDensityTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32I);
}
void CollisionAvoidanceFBO::BindForDensityBufferWriting(GLenum TextureUnit, GLenum TextureUnit_Index)
{
	//miplevel = 0
	glBindImageTexture(TextureUnit_Index, m_writeDensityBufferTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32I);
}
void CollisionAvoidanceFBO::BindForAntiGradientWriting(GLenum TextureUnit, GLenum TextureUnit_Index)
{
	//miplevel = 0
	glBindImageTexture(TextureUnit_Index, m_writeAntiGradientTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8I);
}
