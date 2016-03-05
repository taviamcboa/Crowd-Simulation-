#include <limits.h>
#include <string.h>

#include "AgentAdvectionTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/AdvectionMove.glsl";

AgentAdvectionTechnique::AgentAdvectionTechnique() : Technique(pEffectFile)
{   
}

bool AgentAdvectionTechnique::Init()
{
    if (!CompileProgram("AdvectionMoveTechnique")) {
        return false;
    }

	m_gradient01ImageUnitLocation = GetUniformLocation("gGradientImage01");
	m_gradient23ImageUnitLocation = GetUniformLocation("gGradientImage23");
	m_speedImageUnitLocation = GetUniformLocation("gSpeedImage");
	m_potentialImageUnitLocation = GetUniformLocation("gPotentialImage");
	m_uncertaintyImageUnitLocation = GetUniformLocation("gUncertaintiyImage");
	m_DeltaTimeLocation =  GetUniformLocation("gDeltaTime");
	m_ExitModeLocation =  GetUniformLocation("gExitMode");
	m_maskedGuidanceImageUnitLocation = GetUniformLocation("gMaskedGuidanceImage");
	m_markerSamplingImageUnitLocation = GetUniformLocation("gMarkerSamplingImage");
	m_BuildingCout = GetUniformLocation("gBuildingNum");
	m_HasGUID = GetUniformLocation("gHasGuid");

	if (m_gradient01ImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_gradient23ImageUnitLocation  == INVALID_UNIFORM_LOCATION
		||m_speedImageUnitLocation ==  INVALID_UNIFORM_LOCATION
		||m_potentialImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_uncertaintyImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_DeltaTimeLocation == INVALID_UNIFORM_LOCATION
		||m_ExitModeLocation == INVALID_UNIFORM_LOCATION
		||m_maskedGuidanceImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_markerSamplingImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_BuildingCout == INVALID_UNIFORM_LOCATION
		||m_HasGUID == INVALID_UNIFORM_LOCATION
		)
    {
        return false;
    }

	return true;
}

void AgentAdvectionTechnique::SetGradient01ImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_gradient01ImageUnitLocation, ImageUnit);
}

void AgentAdvectionTechnique::SetGradient23ImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_gradient23ImageUnitLocation, ImageUnit);
}

void AgentAdvectionTechnique::SetSpeedImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_speedImageUnitLocation, ImageUnit);
}

void AgentAdvectionTechnique::SetPotentialImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_potentialImageUnitLocation, ImageUnit);
}
void AgentAdvectionTechnique::SetUncertaintyImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_uncertaintyImageUnitLocation, ImageUnit);
}
void AgentAdvectionTechnique::SetMarkerSamplingImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_markerSamplingImageUnitLocation, ImageUnit);
}
void AgentAdvectionTechnique::SetDeltaTimeLocation(float deltatime)
{
	glUniform1f(m_DeltaTimeLocation, deltatime);
}
void AgentAdvectionTechnique::SetExitMode(int exitmode)
{
	glUniform1i(m_ExitModeLocation, exitmode);
}
void AgentAdvectionTechnique::SetMaskedGuidanceImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_maskedGuidanceImageUnitLocation, ImageUnit);
}
void AgentAdvectionTechnique::SetBuildingCt(int bct)
{
	glUniform1i(m_BuildingCout, bct);
}
void AgentAdvectionTechnique::SetHasGUID(int hguid)
{
	glUniform1i(m_HasGUID, hguid);
}