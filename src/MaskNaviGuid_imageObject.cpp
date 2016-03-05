#include <stdio.h>

#include "MaskNaviGuid_imageObject.h"
#include "util.h"


MaskedNaviGuidImageObject::MaskedNaviGuidImageObject()
{
    m_masked_navi_texture = 0;
	m_masked_guid_texture = 0;

	ZERO_MEM(m_Buffers);
}

MaskedNaviGuidImageObject::~MaskedNaviGuidImageObject()
{
    if (m_masked_navi_texture != 0) {
        glDeleteTextures(1, &m_masked_navi_texture);
    }
	if (m_masked_guid_texture != 0) {
        glDeleteTextures(1, &m_masked_guid_texture);
    }
	if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
}

bool MaskedNaviGuidImageObject::Init(unsigned int GridWidth, unsigned int GridHeight, unsigned int groupnum)
{
    glGenTextures(1, &m_masked_navi_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_masked_navi_texture);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, GridWidth, GridHeight, groupnum); //depth
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenTextures(1, &m_masked_guid_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_masked_guid_texture);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, GridWidth, GridHeight, groupnum); //depth
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenBuffers(NUM_VBs, m_Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[SHADERSTORAGEBUFFER_TYPES::GUID01]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4f) * GridWidth * GridHeight, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[SHADERSTORAGEBUFFER_TYPES::GUID23]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4f) * GridWidth * GridHeight, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[SHADERSTORAGEBUFFER_TYPES::NAVI01]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4f) * GridWidth * GridHeight, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[SHADERSTORAGEBUFFER_TYPES::NAVI23]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4f) * GridWidth * GridHeight, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
    return true;
}

void MaskedNaviGuidImageObject::BindForMaskedGuidImageArrayUnitWriting(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_masked_guid_texture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
void MaskedNaviGuidImageObject::BindForMaskedGuidImageArrayUnitReading(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_masked_guid_texture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
}
void MaskedNaviGuidImageObject::BindForMaskedGuidTextureArrayReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_masked_guid_texture);
}

void MaskedNaviGuidImageObject::BindForMaskedNaviImageArrayUnitWriting(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_masked_navi_texture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
void MaskedNaviGuidImageObject::BindForMaskedNaviImageArrayUnitReading(GLenum ImageUnit)
{
    glBindImageTexture(ImageUnit, m_masked_navi_texture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
}
void MaskedNaviGuidImageObject::BindForMaskedNaviTextureArrayReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_masked_navi_texture);
}
void MaskedNaviGuidImageObject::BindForShaderStorageBuffer()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[SHADERSTORAGEBUFFER_TYPES::GUID01]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_Buffers[SHADERSTORAGEBUFFER_TYPES::GUID23]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_Buffers[SHADERSTORAGEBUFFER_TYPES::NAVI01]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_Buffers[SHADERSTORAGEBUFFER_TYPES::NAVI23]);
}