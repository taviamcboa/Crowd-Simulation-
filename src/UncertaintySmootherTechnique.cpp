#include <limits.h>
#include <string.h>

#include "UncertaintySmootherTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/UncertaintySmoother.glsl";

UncertaintySmootherTechnique::UncertaintySmootherTechnique() : Technique(pEffectFile)
{   
}

bool UncertaintySmootherTechnique::Init()
{
    if (!CompileProgram("UncertaintySmootherTechnique")) {
        return false;
    }

	m_uncertaintyImageUnitLocation = GetUniformLocation("gUncertaintyImage");
	m_eularianImageUnitLocation = GetUniformLocation("gEularianImage");
	//m_firsttimeUniformLocation = GetUniformLocation("firsttime");
	if (m_uncertaintyImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_eularianImageUnitLocation == INVALID_UNIFORM_LOCATION)
		//||m_firsttimeUniformLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	return true;
}

void UncertaintySmootherTechnique::SetFirstTimeUniform(int FirstTime)
{
	glUniform1i(m_firsttimeUniformLocation, FirstTime);

}

void UncertaintySmootherTechnique::SetUncertaintyImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_uncertaintyImageUnitLocation, ImageUnit);
}
void UncertaintySmootherTechnique::SetEularianImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_eularianImageUnitLocation, ImageUnit);
}

