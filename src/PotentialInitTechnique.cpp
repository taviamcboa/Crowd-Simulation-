
#include <limits.h>
#include <string.h>

#include "PotentialInitTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/PotentialCalculator.glsl";

PotentialInitTechnique::PotentialInitTechnique() : Technique(pEffectFile)
{   
}

bool PotentialInitTechnique::Init()
{
    if (!CompileProgram("PotentialInitTechnique")) {
        return false;
    }

	m_potentialWriteImageUnitLocation = GetUniformLocation("gPotentialImage");
	

	if (m_potentialWriteImageUnitLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	return true;
}

void PotentialInitTechnique::SetPotentialWriteImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_potentialWriteImageUnitLocation, ImageUnit);
}
