#include <limits.h>
#include <string.h>

#include "GradientCalculator_Technique.h"
#include "util.h"

static const char* pEffectFile = "shaders/GradientCalculator.glsl";

GradientCalcTechnique::GradientCalcTechnique() : Technique(pEffectFile)
{   

}

bool GradientCalcTechnique::Init()
{
    if (!CompileProgram("GradientCalculatorTechnique")) {
        return false;
    }

	m_potentialReadImageUnitLocation = GetUniformLocation("gPotentialImage");
	m_gradient12WriteImageUnitLocation = GetUniformLocation("gGradient12");
	m_gradient34WriteImageUnitLocation = GetUniformLocation("gGradient34");
	m_gradientBufferWriteImageUnitLocation = GetUniformLocation("gGradientTest");
	if (m_potentialReadImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_gradient12WriteImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_gradient34WriteImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_gradientBufferWriteImageUnitLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }


	return true;
}

void GradientCalcTechnique::SetPotentialReadImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_potentialReadImageUnitLocation, ImageUnit);
}

void GradientCalcTechnique::SetGradient12WriteImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_gradient12WriteImageUnitLocation, ImageUnit);
}

void GradientCalcTechnique::SetGradient34WriteImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_gradient34WriteImageUnitLocation, ImageUnit);
}
void GradientCalcTechnique::SetGradientBufferWriteImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_gradientBufferWriteImageUnitLocation, ImageUnit);
}