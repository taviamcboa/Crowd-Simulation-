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

#include "MotionComputing_technique.h"
#include "util.h"

static const char* pEffectFile = "shaders/MotionComputing.glsl";

MotionComputingTechnique::MotionComputingTechnique() : Technique(pEffectFile)
{   
}

bool MotionComputingTechnique::Init()
{
    if (!CompileProgram("MotionComputingTechnique")) {
        return false;
    }

	m_AgentVoronoiTextureUnitLocation = GetUniformLocation("gAgentVoronoiMap");
	m_MarkerSamplingTextureUnitLocation = GetUniformLocation("gMarkerSamplingMap");
	m_WriteMotionBufferUnitLocation = GetUniformLocation("gMotionBuffer");
	m_naviTextureArrayUnitLocation = GetUniformLocation("gNaviTexArray");
	m_DensityTextureUnitLocation = GetUniformLocation("gDensityMap");
	m_AntiGradientTextureUnitLocation = GetUniformLocation("gAntiGradientMap");
	m_guidTextureArrayUnitLocation = GetUniformLocation("gGuidTexArray"); 
	m_PreviousMotionVectorTexureUnitLocation = GetUniformLocation("gPreviousMotionVector");
	//m_GoalDirLocation = GetUniformLocation("goal_dir");
	//m_PreviouDirLocation = GetUniformLocation("previous_dir");
	//m_NavigationDirLocation = GetUniformLocation("navigation_dir");
	//m_SearchRadius = GetUniformLocation("gSearchRadius");
	//m_WeightCombo = GetUniformLocation("weightcomb");

	if (m_AgentVoronoiTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_MarkerSamplingTextureUnitLocation ==INVALID_UNIFORM_LOCATION
		||m_WriteMotionBufferUnitLocation == INVALID_UNIFORM_LOCATION
		||m_naviTextureArrayUnitLocation == INVALID_UNIFORM_LOCATION
		||m_DensityTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_AntiGradientTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_guidTextureArrayUnitLocation == INVALID_UNIFORM_LOCATION
		||m_PreviousMotionVectorTexureUnitLocation== INVALID_UNIFORM_LOCATION
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
	glGenSamplers(1, &SamplerName_navi);
	glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName_navi, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(SamplerName_navi, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void MotionComputingTechnique::SetAgentVoronoiTextureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName[0]);
    glUniform1i(m_AgentVoronoiTextureUnitLocation, TextureUnit);
}

void MotionComputingTechnique::SetMarkerSamplingTextureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName[1]);
    glUniform1i(m_MarkerSamplingTextureUnitLocation, TextureUnit);
}
void MotionComputingTechnique::SetWriteMotionBufferTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_WriteMotionBufferUnitLocation, TextureUnit);
}
void MotionComputingTechnique::SetNaviTexArrayUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName_navi);
    glUniform1i(m_naviTextureArrayUnitLocation, TextureUnit);
}
void MotionComputingTechnique::SetGuidTexArrayUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName_navi);
    glUniform1i(m_guidTextureArrayUnitLocation, TextureUnit);
}
void MotionComputingTechnique::SetDensityTextureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName_navi);
    glUniform1i(m_DensityTextureUnitLocation, TextureUnit);
}
void MotionComputingTechnique::SetAntiGradientTextureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName_navi);
    glUniform1i(m_AntiGradientTextureUnitLocation, TextureUnit);
}
void MotionComputingTechnique::SetPreviousMotionVectorUnit(unsigned int TextureUnit)
{
    glUniform1i(m_PreviousMotionVectorTexureUnitLocation, TextureUnit);
}

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
