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

#include "MarkerSampling_technique.h"
#include "util.h"

static const char* pEffectFile = "shaders/MarkerSampling.glsl";

MarkerSamplingTechnique::MarkerSamplingTechnique() : Technique(pEffectFile)
{   
}

bool MarkerSamplingTechnique::Init()
{
    if (!CompileProgram("MarkerSamplingTechnique")) {
        return false;
    }

	m_AgentVoronoiTextureUnitLocation = GetUniformLocation("gAgentVoronoiMap");
	m_EularianTextureUnitLocation = GetUniformLocation("gEularianMap");

	m_MarkerVoronoiTextureUnitLocation = GetUniformLocation("gMarkerVoronoiMap");
	m_WriteMarkerIDUnitLocation = GetUniformLocation("gMarkerBuffer");

	if (m_AgentVoronoiTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_EularianTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_MarkerVoronoiTextureUnitLocation == INVALID_UNIFORM_LOCATION
		||m_WriteMarkerIDUnitLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	glGenSamplers(3, SamplerName);

	for(std::size_t i = 0; i < 3; ++i)
	{
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(SamplerName[i], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glSamplerParameteri(SamplerName[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glSamplerParameteri(SamplerName[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	return true;
}

void MarkerSamplingTechnique::SetAgentVoronoiTextureUnit(unsigned int TextureUnit)
{   
	glBindSampler(TextureUnit, SamplerName[0]);
    glUniform1i(m_AgentVoronoiTextureUnitLocation, TextureUnit);
}

void MarkerSamplingTechnique::SetEularianTextureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName[1]);
    glUniform1i(m_EularianTextureUnitLocation, TextureUnit);
}
void MarkerSamplingTechnique::SetMarkerVoronoiTextureUnit(unsigned int TextureUnit)
{
	glBindSampler(TextureUnit, SamplerName[2]);
	glUniform1i(m_MarkerVoronoiTextureUnitLocation, TextureUnit);
}

void MarkerSamplingTechnique::SetWriteMarkerIDTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_WriteMarkerIDUnitLocation, TextureUnit);
}