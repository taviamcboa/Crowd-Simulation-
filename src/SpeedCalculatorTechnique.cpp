#include <limits.h>
#include <string.h>

#include "SpeedCalculatorTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/SpeedCalculator.glsl";

SpeedCalculatorTechnique::SpeedCalculatorTechnique() : Technique(pEffectFile)
{   
}

bool SpeedCalculatorTechnique::Init()
{
    if (!CompileProgram("SpeedCalculatorTechnique")) {
        return false;
    }

	m_ReadDensityVelocityImageUnitLocation = GetUniformLocation("gDensityVelocityImage");
	m_WriteSpeedImageUnitLocation = GetUniformLocation("gSpeedNESWImage"); 

	if (m_ReadDensityVelocityImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_WriteSpeedImageUnitLocation == INVALID_UNIFORM_LOCATION )
    {
        return false;
    }

	return true;
}

void SpeedCalculatorTechnique::SetReadDensityVelocityImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_ReadDensityVelocityImageUnitLocation, ImageUnit);
}

void SpeedCalculatorTechnique::SetWriteDensityVelocityImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_WriteSpeedImageUnitLocation, ImageUnit);
}