#include <limits.h>
#include <string.h>

#include "FutureDiscomfortSplatterTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/FutureDiscomfortSplatter.glsl";

FutureDiscomfortSplatterTechnique::FutureDiscomfortSplatterTechnique() : Technique(pEffectFile)
{   
}

bool FutureDiscomfortSplatterTechnique::Init()
{
    if (!CompileProgram("FutureDiscomfortSplatterTechnique")) {
        return false;
    }

	m_AgentSphereTextureUnitLocation = GetUniformLocation("gAgentSphereMap");
	m_TimeStepUniformLocation = GetUniformLocation("gTimeStep");
	m_DiscomfortCoeffUniformLocation = GetUniformLocation("gDiscomfortCoeff");
	m_DeltaTimePerStepUniformLocation = GetUniformLocation("gDeltaTimePerStep");
	if (m_AgentSphereTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_TimeStepUniformLocation == INVALID_UNIFORM_LOCATION
		||m_DiscomfortCoeffUniformLocation == INVALID_UNIFORM_LOCATION
		||m_DeltaTimePerStepUniformLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	glGenSamplers(1, SamplerName);

	for(std::size_t i = 0; i < 1; ++i)
	{
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glSamplerParameteri(SamplerName[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glSamplerParameteri(SamplerName[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	//glGenSamplers(1, &SamplerName_navi);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void FutureDiscomfortSplatterTechnique::SetAgentSphereTexureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName[0]);
    glUniform1i(m_AgentSphereTextureUnitLocation, TextureUnit);
}
void FutureDiscomfortSplatterTechnique::SetTimeStepUniform(int TimeStep)
{
    glUniform1i(m_TimeStepUniformLocation, TimeStep);
}
void FutureDiscomfortSplatterTechnique::SetDiscomfortCoeffUniform(int DiscomfortCoeff)
{
    glUniform1i(m_DiscomfortCoeffUniformLocation, DiscomfortCoeff);
}
void FutureDiscomfortSplatterTechnique::SetDeltaTimePerStepUniform(float DeltaTimePerStep)
{
    glUniform1f(m_DeltaTimePerStepUniformLocation, DeltaTimePerStep);
}
