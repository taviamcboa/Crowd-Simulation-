#include <stdio.h>

#include "Potential_imageObject.h"
#include "util.h"

PotentialImageObject::PotentialImageObject()
{
    m_potential_texture = 0;


}

PotentialImageObject::~PotentialImageObject()
{
    if (m_potential_texture != 0) {
        glDeleteTextures(1, &m_potential_texture);
    }

	if (m_potential_writetexture != 0) {
        glDeleteTextures(1, &m_potential_writetexture);
    }

    if (m_potential_writebuffer != 0) {
        glDeleteBuffers(1, &m_potential_writebuffer);
    }

	if (m_neighboring_potential_writetexture != 0) {
        glDeleteTextures(1, &m_neighboring_potential_writetexture);
    }
	if (m_neighboring_potential_writebuffer != 0) {
        glDeleteBuffers(1, &m_neighboring_potential_writebuffer);
    }
	if (m_neighboring_cost_writetexture != 0) {
        glDeleteTextures(1, &m_neighboring_cost_writetexture);
    }
	if (m_neighboring_cost_writebuffer != 0) {
        glDeleteTextures(1, &m_neighboring_cost_writebuffer);
    }
}

bool PotentialImageObject::Init(unsigned int GridWidth, unsigned int GridHeight)
{
    glGenTextures(1, &m_potential_texture);
	glBindTexture(GL_TEXTURE_2D, m_potential_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexStorage2D(GL_TEXTURE_2D, GLint(1), GL_RGBA32F, GridWidth, GridHeight);
	
	float *inital = new float[128 * 128 *4];
	for(int i =0; i< 128 * 128 *4; i++)
	{
		inital[i] = -99.0f; 
	}

	float *sharevalue = new float[324 * 8 * 8 *4]; 
	for(int i =0; i< 324 * 8 * 8 *4; i++)
	{
		sharevalue[i] = 0.0f; 
	}

	glGenBuffers(1, &m_potential_writebuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, m_potential_writebuffer);
	glBufferData(GL_TEXTURE_BUFFER, 324 * 8 * 8 *4*sizeof(float), sharevalue, GL_DYNAMIC_READ);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glGenTextures(1, &m_potential_writetexture);
	glBindTexture(GL_TEXTURE_BUFFER, m_potential_writetexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_potential_writebuffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	//
	glGenBuffers(1, &m_neighboring_potential_writebuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, m_neighboring_potential_writebuffer);
	glBufferData(GL_TEXTURE_BUFFER, 128 * 128 *4*sizeof(float), inital, GL_DYNAMIC_READ);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glGenTextures(1, &m_neighboring_potential_writetexture);
	glBindTexture(GL_TEXTURE_BUFFER, m_neighboring_potential_writetexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_neighboring_potential_writebuffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	glGenBuffers(1, &m_neighboring_cost_writebuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, m_neighboring_cost_writebuffer);
	glBufferData(GL_TEXTURE_BUFFER, 128 * 128 *4*sizeof(float), inital, GL_DYNAMIC_READ);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glGenTextures(1, &m_neighboring_cost_writetexture);
	glBindTexture(GL_TEXTURE_BUFFER, m_neighboring_cost_writetexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_neighboring_cost_writebuffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	//glBufferSubData(GL_TEXTURE_BUFFER, 0, 0, Displacement);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	delete[] inital;
	delete[] sharevalue;
    return true;
}

void PotentialImageObject::BindForImageUnitReadingWriting(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_potential_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void PotentialImageObject::BindForImageUnitReadingOnly(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_potential_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
}

void PotentialImageObject::BindForImageUnitWritingOnly(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_potential_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void PotentialImageObject::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_potential_texture);
}
void PotentialImageObject::BindForImageBufferWritingOnly(GLenum ImageUnit)
{
	//miplevel = 0
	glBindImageTexture(ImageUnit, m_potential_writetexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void PotentialImageObject::BindForNeighboringPotentialImageBufferWritingOnly(GLenum ImageUnit)
{
	//miplevel = 0
	glBindImageTexture(ImageUnit, m_neighboring_potential_writetexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void PotentialImageObject::BindForNeighboringCostImageBufferWritingOnly(GLenum ImageUnit)
{
	//miplevel = 0
	glBindImageTexture(ImageUnit, m_neighboring_cost_writetexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}