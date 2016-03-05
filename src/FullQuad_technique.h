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

#ifndef FULLQUAD_TECHNIQUE_H
#define	FULLQUAD_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class FullQuadTechnique : public Technique {
public:

    FullQuadTechnique();

    virtual bool Init();

	void SetColorTextureUnit(unsigned int TextureUnit);
	void SetDensityImageUnit(unsigned int TextureUnit);
	void SetAntiGradientImageUnit(unsigned int TextureUnit);
	void SetGuidTexArrayTextureUnit(unsigned int TextureUnit);
	void SetNaviTexArrayTextureUnit(unsigned int TextureUnit);
	void SetDensityVelocityTextureUnit(unsigned int TextureUnit); 

	void SetIntegerTextureFlag(float flag); 
	void SetIntegerNaviGuidance(unsigned int NaviGuidance); 
	void SetIntegerGuidance(unsigned int Guidance);
	void SetIntegerNavi(unsigned int Navi);
	void SetGroupID(unsigned int groupID);
private:

	GLuint m_colorTextureUnitLocation;

	GLuint m_densityImageUnitLocation; 
	GLuint m_antigradientImageUnitLocation; 
	GLuint m_guidtexarrayTexureUnitLocation; 
	GLuint m_navitexarrayTexureUnitLocation; 
	GLuint m_densityvelocityTexureUnitLocation; 

	GLuint m_integerTex; 
	GLuint m_integerNaviGuidance; 
	GLuint m_integerGuidance; 
	GLuint m_integerNavi; 
	GLuint m_groupID; 
};


#endif