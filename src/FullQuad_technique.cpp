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

#include "FullQuad_technique.h"
#include "util.h"

static const char* pEffectFile = "shaders/FullQuad.glsl";

FullQuadTechnique::FullQuadTechnique() : Technique(pEffectFile)
{   
}

bool FullQuadTechnique::Init()
{
    if (!CompileProgram("FullQuadTechnique")) {
        return false;
    }

	m_colorTextureUnitLocation = GetUniformLocation("gColorMap");
	m_densityImageUnitLocation = GetUniformLocation("gDensityImage");
	m_antigradientImageUnitLocation = GetUniformLocation("gAntiGradientImage");
	m_guidtexarrayTexureUnitLocation = GetUniformLocation("gGuidTexureArray");
	m_navitexarrayTexureUnitLocation = GetUniformLocation("gNaviTexureArray");
	m_densityvelocityTexureUnitLocation = GetUniformLocation("gDensityVelocityTexture");

	m_integerTex = GetUniformLocation("gIntegerMap"); 
	m_integerNaviGuidance = GetUniformLocation("gIntegerNaviGuidance"); 
	m_integerGuidance = GetUniformLocation("gIntegerGuidance"); 
	m_integerNavi = GetUniformLocation("gIntegerNavi"); 
	m_groupID = GetUniformLocation("gGroupID"); 

	if (m_colorTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_densityImageUnitLocation == INVALID_UNIFORM_LOCATION
		||m_antigradientImageUnitLocation== INVALID_UNIFORM_LOCATION
		||m_integerTex == INVALID_UNIFORM_LOCATION
		||m_integerNaviGuidance == INVALID_UNIFORM_LOCATION
		||m_integerGuidance == INVALID_UNIFORM_LOCATION
		||m_integerNavi == INVALID_UNIFORM_LOCATION
		||m_groupID == INVALID_UNIFORM_LOCATION
		||m_densityvelocityTexureUnitLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	return true;
}

void FullQuadTechnique::SetColorTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_colorTextureUnitLocation, TextureUnit);
}

void FullQuadTechnique::SetDensityImageUnit(unsigned int TextureUnit)
{
    glUniform1i(m_densityImageUnitLocation, TextureUnit);
}

void FullQuadTechnique::SetIntegerTextureFlag(float flag)
{
    glUniform1f(m_integerTex, flag);
}

void FullQuadTechnique::SetAntiGradientImageUnit(unsigned int TextureUnit)
{
   glUniform1i(m_antigradientImageUnitLocation, TextureUnit);
}

void FullQuadTechnique::SetGuidTexArrayTextureUnit(unsigned int TextureUnit)
{
   glUniform1i(m_guidtexarrayTexureUnitLocation, TextureUnit);
}

void FullQuadTechnique::SetNaviTexArrayTextureUnit(unsigned int TextureUnit)
{
   glUniform1i(m_navitexarrayTexureUnitLocation, TextureUnit);
}

void FullQuadTechnique::SetIntegerNaviGuidance(unsigned int NaviGuidance)
{
	glUniform1i(m_integerNaviGuidance, NaviGuidance);
}

void FullQuadTechnique::SetGroupID(unsigned int groupID)
{
	glUniform1i(m_groupID, groupID);
}

void FullQuadTechnique::SetIntegerGuidance(unsigned int Guidance)
{
	glUniform1i(m_integerGuidance, Guidance); 
}

void FullQuadTechnique::SetIntegerNavi(unsigned int Navi)
{
	glUniform1i(m_integerNavi, Navi); 
}

void FullQuadTechnique::SetDensityVelocityTextureUnit(unsigned int DensityVelocity)
{
	glUniform1i(m_densityvelocityTexureUnitLocation, DensityVelocity); 
}