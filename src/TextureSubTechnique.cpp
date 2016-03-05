
#include <limits.h>
#include <string.h>

#include "TextureSubTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/TextureSub.glsl";

TextureSubTechnique::TextureSubTechnique() : Technique(pEffectFile)
{   
}

bool TextureSubTechnique::Init()
{
    if (!CompileProgram("TextureSubTechnique")) {
        return false;
    }

	m_markerSamplingTextureUnitLocation1 = GetUniformLocation("gMarkerSamplingMap1");
	m_markerSamplingTextureUnitLocation2 = GetUniformLocation("gMarkerSamplingMap2");

	if (m_markerSamplingTextureUnitLocation1 == INVALID_UNIFORM_LOCATION
		||m_markerSamplingTextureUnitLocation2 == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	glGenSamplers(1, &SamplerName);


	glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glSamplerParameteri(SamplerName, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(SamplerName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void TextureSubTechnique::SetMarkerSamplingTextureUnit1(unsigned int TextureUnit)
{

	glBindSampler(TextureUnit, SamplerName);
    glUniform1i(m_markerSamplingTextureUnitLocation1, TextureUnit);
}

void TextureSubTechnique::SetMarkerSamplingTextureUnit2(unsigned int TextureUnit)
{

	glBindSampler(TextureUnit, SamplerName);
    glUniform1i(m_markerSamplingTextureUnitLocation2, TextureUnit);
}