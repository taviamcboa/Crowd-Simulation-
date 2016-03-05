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

#ifndef MOTIONCOMPUTING_TECHNIQUE_H
#define	MOTIONCOMPUTING_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class MotionComputingTechnique : public Technique {
public:

    MotionComputingTechnique();

    virtual bool Init();

	void SetAgentVoronoiTextureUnit(unsigned int TextureUnit);
	void SetMarkerSamplingTextureUnit(unsigned int TextureUnit);
	void SetWriteMotionBufferTextureUnit(unsigned int TextureUnit);
	void SetNaviTexArrayUnit(unsigned int TextureUnit);
	void SetDensityTextureUnit(unsigned int TextureUnit);
	void SetAntiGradientTextureUnit(unsigned int TextureUnit);
	void SetGuidTexArrayUnit(unsigned int TextureUnit); 
	void SetPreviousMotionVectorUnit(unsigned int TextureUnit); 
	//void SetGoalDir(const Vector2f& GoalDir);
	//void SetPreviousDir(const Vector2f& PreviousDir);
	//void SetNavigationDir(const Vector2f& NavigationDir);
	//void SetSearchRaius(float SearchRadius);
	//void SetWeightCombo(const Vector3f& WeightCombo);

private:

	GLuint m_AgentVoronoiTextureUnitLocation;
	GLuint m_MarkerSamplingTextureUnitLocation;
	GLuint m_naviTextureArrayUnitLocation;
	GLuint m_guidTextureArrayUnitLocation; 

	GLuint m_WriteMotionBufferUnitLocation;  

	GLuint m_DensityTextureUnitLocation; 
	GLuint m_AntiGradientTextureUnitLocation;
	GLuint m_PreviousMotionVectorTexureUnitLocation; 

	GLuint SamplerName[2]; // 3 TEXTURE NEED TO SAMPLE
	GLuint SamplerName_navi; 
	//GLuint m_GoalDirLocation;
	//GLuint m_PreviouDirLocation;
	//GLuint m_NavigationDirLocation; 

	//GLuint m_SearchRadius; 
	//GLuint m_WeightCombo; 
};


#endif