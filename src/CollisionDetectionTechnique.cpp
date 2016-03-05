#include <limits.h>
#include <string.h>

#include "CollisionDetectionTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/CollisionDetection.glsl";

CollisionDetectionTechnique::CollisionDetectionTechnique() : Technique(pEffectFile)
{   
}

bool CollisionDetectionTechnique::Init()
{
    if (!CompileProgram("CollisionDetectionTechnique")) {
        return false;
    }

	m_AgentVoronoiImageUnitLocation = GetUniformLocation("gAgentVoronoiImage");
	m_MarkerSamplingImageUnitLocation = GetUniformLocation("gMarkerSamplingImage");
    m_EularianMapImageUnitLocation = GetUniformLocation("gEularianImageFullResolution");
	m_DeltaTimeUniformLocation = GetUniformLocation("gDeltaTime");
	m_GuidanceImageUnitLocation =  GetUniformLocation("gGuidanceImageArray");
	m_HasGUIDUniformLocation = GetUniformLocation("gHasGuid");

	if (m_AgentVoronoiImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_MarkerSamplingImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_EularianMapImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_DeltaTimeUniformLocation == INVALID_UNIFORM_LOCATION
		||m_GuidanceImageUnitLocation == INVALID_UNIFORM_LOCATION)
		//||m_HasGUIDUniformLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	return true;
}

void CollisionDetectionTechnique::SetAgentVoronoiImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_AgentVoronoiImageUnitLocation, ImageUnit);
}

void CollisionDetectionTechnique::SetMarkerSamplingImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_MarkerSamplingImageUnitLocation, ImageUnit);
}

void CollisionDetectionTechnique::SetEularianMapImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_EularianMapImageUnitLocation, ImageUnit);
}
void CollisionDetectionTechnique::SetGuidanceImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_GuidanceImageUnitLocation, ImageUnit);
}
void CollisionDetectionTechnique::SetDeltaTimeUniform(float deltaTime)
{
	glUniform1f(m_DeltaTimeUniformLocation, deltaTime);
}

void CollisionDetectionTechnique::SetHasGUIDUniformLocation(int hasGUID)
{
	glUniform1i(m_HasGUIDUniformLocation, hasGUID);
}