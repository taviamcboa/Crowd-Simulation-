#include <limits.h>
#include <string.h>

#include "MarkerSamplingBufferTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/MarkerSamplingBuffer.glsl";

MarkerSamplingBufferTechnique::MarkerSamplingBufferTechnique() : Technique(pEffectFile)
{   
}

bool MarkerSamplingBufferTechnique::Init()
{
    if (!CompileProgram("MarkerSamplingBufferTechnique")) {
        return false;
    }

	m_AgentVoronoiTextureUnitLocation = GetUniformLocation("gAgentVoronoiMap");
	m_EularianTextureUnitLocation = GetUniformLocation("gEularianMap");


	if (m_AgentVoronoiTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_EularianTextureUnitLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	glGenSamplers(2, SamplerName);


	glSamplerParameteri(SamplerName[0], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName[0], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName[0], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glSamplerParameteri(SamplerName[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(SamplerName[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glSamplerParameteri(SamplerName[1], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName[1], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName[1], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glSamplerParameteri(SamplerName[1], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(SamplerName[1], GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void MarkerSamplingBufferTechnique::SetAgentVoronoiTextureUnit(unsigned int TextureUnit)
{   
	glBindSampler(TextureUnit, SamplerName[0]);
    glUniform1i(m_AgentVoronoiTextureUnitLocation, TextureUnit);
}

void MarkerSamplingBufferTechnique::SetEularianTextureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName[1]);
    glUniform1i(m_EularianTextureUnitLocation, TextureUnit);
}
