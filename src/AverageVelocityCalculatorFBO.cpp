#include <stdio.h>
#include "AverageVelocityCalculatorFBO.h"
#include "util.h"

AverageVelocityCalculatorFBO::AverageVelocityCalculatorFBO()
{
   m_AverageVelocityRWTexure = 0; 
}

AverageVelocityCalculatorFBO::~AverageVelocityCalculatorFBO()
{
	if (m_AverageVelocityRWTexure != 0) 
	{
        glDeleteTextures(1, &m_AverageVelocityRWTexure);
    }
}

bool AverageVelocityCalculatorFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
	
}

void AverageVelocityCalculatorFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}
void AverageVelocityCalculatorFBO::BindForReading(GLenum ImageUnit)
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
