#include <limits.h>
#include <string.h>

#include "DrawMaskedNaviGuidTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/DrawMaskedNaviGuid.glsl";

DrawMaskedNaviGuidTechnique::DrawMaskedNaviGuidTechnique() : Technique(pEffectFile)
{   
}

bool DrawMaskedNaviGuidTechnique::Init()
{
    if (!CompileProgram("DrawMaskedNaviGuidTechnique")) {
        return false;
    }

	m_MaskedNaviArrayTextureUnitLocation = GetUniformLocation("gGuidTexureArray");
	m_MaskedGuidArrayTextureUnitLocation = GetUniformLocation("gNaviTexureArray");
	m_GroupIDUniformLocation = GetUniformLocation("groupID");
	m_drawNaviUniformLocation = GetUniformLocation("drawNavi");

	if (m_MaskedNaviArrayTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_MaskedGuidArrayTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_GroupIDUniformLocation == INVALID_UNIFORM_LOCATION
		||m_drawNaviUniformLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	return true;
}

void DrawMaskedNaviGuidTechnique::SetMaskedNaviArrayTextureUnit(unsigned int TexureUnit)
{
	glUniform1i(m_MaskedNaviArrayTextureUnitLocation, TexureUnit);
}

void DrawMaskedNaviGuidTechnique::SetMaskedGuidArrayTextureUnit(unsigned int TexureUnit)
{
	glUniform1i(m_MaskedGuidArrayTextureUnitLocation, TexureUnit);
}
void DrawMaskedNaviGuidTechnique::SetGroupIDUniform(unsigned int GroupID)
{
	glUniform1i(m_GroupIDUniformLocation, GroupID);
}
void DrawMaskedNaviGuidTechnique::SetDrawNaviUniform(unsigned int drawNavi)
{
	glUniform1i(m_drawNaviUniformLocation, drawNavi);
}

