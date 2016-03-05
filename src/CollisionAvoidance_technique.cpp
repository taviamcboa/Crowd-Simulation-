/*

	Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <limits.h>
#include <string.h>

#include "CollisionAvoidance_technique.h"
#include "util.h"

static const char* pEffectFile = "shaders/CollisionAvoidance.glsl";

CollisionAvoidanceTechnique::CollisionAvoidanceTechnique() : Technique(pEffectFile)
{   
}

bool CollisionAvoidanceTechnique::Init()
{
    if (!CompileProgram("CollisionAvoidanceTechnique")) {
        return false;
    }

	m_AgentVoronoiTextureUnitLocation = GetUniformLocation("gAgentVoronoiMap");
	m_MotionTextureUnitLocation = GetUniformLocation("gMotionMap");
	m_WriteFinalMotionBufferUnitLocation = GetUniformLocation("gMotionBufferNoCollision");
	m_WriteDensityFieldUnitLocation = GetUniformLocation("gDensityField");
	m_WriteDensityFieldBufferUnitLocation = GetUniformLocation("gDensityFieldBuffer");
	//screen out for debug original algorithm
	//m_naviTextureArrayUnitLocation = GetUniformLocation("gNaviTexArray");


	//m_ReadFinalMotionBufferUnitLocation_Pre = GetUniformLocation("gMotionBufferNoCollisionPre");
	//m_GoalDirLocation = GetUniformLocation("goal_dir");
	//m_PreviouDirLocation = GetUniformLocation("previous_dir");
	//m_NavigationDirLocation = GetUniformLocation("navigation_dir");
	//m_SearchRadius = GetUniformLocation("gSearchRadius");
	//m_WeightCombo = GetUniformLocation("weightcomb");

	if (m_AgentVoronoiTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_MotionTextureUnitLocation ==INVALID_UNIFORM_LOCATION
		||m_WriteFinalMotionBufferUnitLocation == INVALID_UNIFORM_LOCATION
		||m_WriteDensityFieldUnitLocation == INVALID_UNIFORM_LOCATION
		||m_WriteDensityFieldBufferUnitLocation == INVALID_UNIFORM_LOCATION
		//||m_naviTextureArrayUnitLocation == INVALID_UNIFORM_LOCATION
		//||m_ReadFinalMotionBufferUnitLocation_Pre == INVALID_UNIFORM_LOCATION
/*		||m_GoalDirLocation == INVALID_UNIFORM_LOCATION
		||m_PreviouDirLocation == INVALID_UNIFORM_LOCATION
		||m_NavigationDirLocation == INVALID_UNIFORM_LOCATION
		||m_SearchRadius == INVALID_UNIFORM_LOCATION
		||m_WeightCombo == INVALID_UNIFORM_LOCATION*/)
    {
        return false;
    }

	glGenSamplers(2, SamplerName);

	for(std::size_t i = 0; i < 2; ++i)
	{
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glSamplerParameteri(SamplerName[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glSamplerParameteri(SamplerName[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	//glGenSamplers(1, &SamplerName_navi);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void CollisionAvoidanceTechnique::SetAgentVoronoiTextureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName[0]);
    glUniform1i(m_AgentVoronoiTextureUnitLocation, TextureUnit);
}
//motiontexture: id
void CollisionAvoidanceTechnique::SetMotionTextureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName[1]);
    glUniform1i(m_MotionTextureUnitLocation, TextureUnit);
}
//texturearray: group id
//void CollisionAvoidanceTechnique::SetNaviTexArrayUnit(unsigned int TextureUnit)
//{
//	glBindSampler(TextureUnit, SamplerName_navi);
//    glUniform1i(m_naviTextureArrayUnitLocation, TextureUnit);
//}
void CollisionAvoidanceTechnique::SetWriteFinalMotionBufferTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_WriteFinalMotionBufferUnitLocation, TextureUnit);
}
void CollisionAvoidanceTechnique::SetWriteDensityFieldTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_WriteDensityFieldUnitLocation, TextureUnit);
}
void CollisionAvoidanceTechnique::SetWriteDensityFieldBufferTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_WriteDensityFieldBufferUnitLocation, TextureUnit);
}
//void CollisionAvoidanceTechnique::SetReadFinalMotionBufferTextureUnitPrev(unsigned int TextureUnit)
//{
//	glUniform1i(m_ReadFinalMotionBufferUnitLocation_Pre, TextureUnit);
//}

//void MotionComputingTechnique::SetGoalDir(const Vector2f& GoalDir)
//{
//	glUniform2f(m_GoalDirLocation, GoalDir.x, GoalDir.y);
//}
//
//void MotionComputingTechnique::SetPreviousDir(const Vector2f& PreviousDir)
//{
//	glUniform2f(m_PreviouDirLocation, PreviousDir.x, PreviousDir.y);
//}
//
//void MotionComputingTechnique::SetNavigationDir(const Vector2f& NavigationDir)
//{
//	glUniform2f(m_NavigationDirLocation, NavigationDir.x, NavigationDir.y);
//}
//
//void MotionComputingTechnique::SetSearchRaius(float SearchRadius)
//{
//	glUniform1f(m_SearchRadius, SearchRadius);
//}
//
//void MotionComputingTechnique::SetWeightCombo(const Vector3f& WeightCombo)
//{
//	glUniform3f(m_WeightCombo, WeightCombo.x, WeightCombo.y, WeightCombo.z);
//}
