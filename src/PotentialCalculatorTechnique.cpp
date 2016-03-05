
#include <limits.h>
#include <string.h>

#include "PotentialCalculatorTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/PotentialCalculator.glsl";

PotentialCalcTechnique::PotentialCalcTechnique() : Technique(pEffectFile)
{   
}

bool PotentialCalcTechnique::Init()
{
    if (!CompileProgram("PotentialCalTechnique")) {
        return false;
    }

	m_potentialReadWriteImageUnitLocation = GetUniformLocation("gPotentialImage");
	m_costReadOnlyImageUnitLocation = GetUniformLocation("gPotentialImage");
	m_potentialWriteBufferUnitLocation = GetUniformLocation("gPotentialBuffer");
	m_neighboringPotentialWriteBufferUnitLocation = GetUniformLocation("gNeighboringPotentialBuffer");
	m_neighboringCostWriteBufferUnitLocation =  GetUniformLocation("gNeighboringCostBuffer");
	m_loopCount = GetUniformLocation("loopcount");
	if (m_potentialReadWriteImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_costReadOnlyImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_potentialWriteBufferUnitLocation== INVALID_UNIFORM_LOCATION
		||m_loopCount == INVALID_UNIFORM_LOCATION
		||m_neighboringCostWriteBufferUnitLocation == INVALID_UNIFORM_LOCATION
		||m_neighboringPotentialWriteBufferUnitLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	return true;
}

void PotentialCalcTechnique::SetLoopCount(int LoopCount)
{
	glUniform1i(m_loopCount, LoopCount);
}

void PotentialCalcTechnique::SetCostReadOnlyImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_costReadOnlyImageUnitLocation, ImageUnit);
}

void PotentialCalcTechnique::SetPotentialReadWriteImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_potentialReadWriteImageUnitLocation, ImageUnit);
}
void PotentialCalcTechnique::SetPotentialWriteBufferUnit(unsigned int BufferUnit)
{
	glUniform1i(m_potentialWriteBufferUnitLocation, BufferUnit);
}
void PotentialCalcTechnique::SetNeighboringPotentialWriteBufferUnit(unsigned int BufferUnit)
{
	glUniform1i(m_neighboringPotentialWriteBufferUnitLocation, BufferUnit);
}
void PotentialCalcTechnique::SetNeighboringCostWriteBufferUnit(unsigned int BufferUnit)
{
	glUniform1i(m_neighboringCostWriteBufferUnitLocation, BufferUnit);
}