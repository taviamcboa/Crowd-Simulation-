#include <limits.h>
#include <string.h>

#include "CostCalculatorTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/CostCalculator.glsl";

CostCalculatorTechnique::CostCalculatorTechnique() : Technique(pEffectFile)
{   

}

bool CostCalculatorTechnique::Init()
{
    if (!CompileProgram("CostCalculatorTechnique")) {
        return false;
    }

	m_densityVelocityReadOnlyImageUnitLocation = GetUniformLocation("gDensityVelocityImage");
	m_speedReadOnlyImageUnitLocation = GetUniformLocation("gSpeedImage");
	m_costWriteOnlyImageUnitLocation = GetUniformLocation("gCostImage");
	m_eularianmapImageUnitLocation = GetUniformLocation("gDiscomfortImage"); 
	m_weightDiscomfort = GetUniformLocation("weightDiscomfort"); 
	m_weightPathLength = GetUniformLocation("weightPathLength"); 
	m_weightTime = GetUniformLocation("weightTime"); 
	m_predictable = GetUniformLocation("predictable"); 

	if (m_densityVelocityReadOnlyImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_speedReadOnlyImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_costWriteOnlyImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_weightDiscomfort == INVALID_UNIFORM_LOCATION
		||m_weightPathLength == INVALID_UNIFORM_LOCATION
		||m_weightTime == INVALID_UNIFORM_LOCATION
		||m_eularianmapImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_predictable == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	return true;
}
void CostCalculatorTechnique::SetEularianMapImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_eularianmapImageUnitLocation, ImageUnit);
}

void CostCalculatorTechnique::SetDensityVelocityReadOnlyImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_densityVelocityReadOnlyImageUnitLocation, ImageUnit);
}

void CostCalculatorTechnique::SetSpeedReadOnlyImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_speedReadOnlyImageUnitLocation, ImageUnit);
}

void CostCalculatorTechnique::SetCostWriteOnlyImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_costWriteOnlyImageUnitLocation, ImageUnit);
}

void CostCalculatorTechnique::SetWeightDiscomfort(float discomfort)
{
	glUniform1f(m_weightDiscomfort, discomfort);
}

void CostCalculatorTechnique::SetWeightPathLength(float pathlength)
{
	glUniform1f(m_weightPathLength, pathlength);
}

void CostCalculatorTechnique::SetWeightTime(float time)
{
	glUniform1f(m_weightTime, time);
}

void CostCalculatorTechnique::SetPredictable(int predictable)
{
	glUniform1i(m_predictable, predictable);
}