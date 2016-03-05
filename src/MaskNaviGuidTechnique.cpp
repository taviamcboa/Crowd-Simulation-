#include <limits.h>
#include <string.h>

#include "MaskNaviGuidTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/MaskNavigationGuidance.glsl";

MaskNaviGuidTechnique::MaskNaviGuidTechnique() : Technique(pEffectFile)
{   
}

bool MaskNaviGuidTechnique::Init()
{
    if (!CompileProgram("MaskNaviGuidTechnique")) {
        return false;
    }

    m_NaviMaskImageUnitLocation = GetUniformLocation("gNaviMaskImage");
	m_NaviArrayImageUnitLocation = GetUniformLocation("gNaviFieldImage");
	m_GuidArrayImageUnitLocation = GetUniformLocation("gGuidanceFieldImage");
	m_MaskedGuidArrayImageUnitLocation = GetUniformLocation("gMaskedGuidanceFieldImage");
	m_MaskedNaviArrayImageUnitLocation = GetUniformLocation("gMaskedNaviFieldImage");

	if(m_NaviMaskImageUnitLocation == INVALID_UNIFORM_LOCATION
	   ||m_GuidArrayImageUnitLocation == INVALID_UNIFORM_LOCATION
	   ||m_NaviArrayImageUnitLocation == INVALID_UNIFORM_LOCATION
	   ||m_MaskedGuidArrayImageUnitLocation == INVALID_UNIFORM_LOCATION
	   ||m_MaskedNaviArrayImageUnitLocation == INVALID_UNIFORM_LOCATION)
	{
		return false;
	}

	return true;
}

void MaskNaviGuidTechnique::SetNaviMaskImageUnit(unsigned int ImageUnit)
{
    glUniform1i(m_NaviMaskImageUnitLocation, ImageUnit);
}
void MaskNaviGuidTechnique::SetGuidArrayImageUnit(unsigned int ImageUnit)
{
    glUniform1i(m_GuidArrayImageUnitLocation, ImageUnit);
}
void MaskNaviGuidTechnique::SetNaviArrayImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_NaviArrayImageUnitLocation, ImageUnit);
}
void MaskNaviGuidTechnique::SetMaskedGuidArrayImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_MaskedGuidArrayImageUnitLocation, ImageUnit);
}
void MaskNaviGuidTechnique::SetMaskedNaviArrayImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_MaskedNaviArrayImageUnitLocation, ImageUnit);
}
