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

#include "DensityVelocitySplatter_technique.h"
#include "util.h"

static const char* pEffectFile = "shaders/DensityVelocitySplatter.glsl";

DensityVelocitySplatterTechnique::DensityVelocitySplatterTechnique() : Technique(pEffectFile)
{   
}

bool DensityVelocitySplatterTechnique::Init()
{
    if (!CompileProgram("DensityVelocitySplatterTechnique")) {
        return false;
    }

	m_AgentSphereTextureUnitLocation = GetUniformLocation("gAgentSphereMap");


	if (m_AgentSphereTextureUnitLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	glGenSamplers(1, SamplerName);

	for(std::size_t i = 0; i < 1; ++i)
	{
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glSamplerParameteri(SamplerName[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glSamplerParameteri(SamplerName[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	//glGenSamplers(1, &SamplerName_navi);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glSamplerParameteri(SamplerName_navi, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void DensityVelocitySplatterTechnique::SetAgentSphereTexureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName[0]);
    glUniform1i(m_AgentSphereTextureUnitLocation, TextureUnit);
}
