#include <limits.h>
#include <string.h>

#include "DrawNaviMaskTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/DrawNaviBuilding.glsl";

DrawNaviMaskTechnique::DrawNaviMaskTechnique() : Technique(pEffectFile)
{   
}

bool DrawNaviMaskTechnique::Init()
{
    if (!CompileProgram("DrawNaviMaskTechnique")) {
        return false;
    }
	m_eularianTextureUnitLocation1 = GetUniformLocation("gEularianMap");

	if (m_eularianTextureUnitLocation1 == INVALID_UNIFORM_LOCATION)
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

void DrawNaviMaskTechnique::SetEularianTexture(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName);
    glUniform1i(m_eularianTextureUnitLocation1, TextureUnit);

}
