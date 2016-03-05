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

#include "DrawNaviFullScreenArray_technique.h"
#include "util.h"

static const char* pEffectFile = "shaders/ReadNaviTex.glsl";

DrawNaviFullScreenArrayTechnique::DrawNaviFullScreenArrayTechnique() : Technique(pEffectFile)
{   
}

bool DrawNaviFullScreenArrayTechnique::Init()
{
    if (!CompileProgram("ReadNaviTexArrayTechnique")) {
        return false;
    }

	m_naviTextureArrayUnitLocation = GetUniformLocation("gNaviTexArray");
	m_navigroupNumLocation =  GetUniformLocation("gNaviGroup");

	if (m_naviTextureArrayUnitLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	return true;
}

void DrawNaviFullScreenArrayTechnique::SetNaviTexArrayUnit(unsigned int TextureUnit)
{
    glUniform1i(m_naviTextureArrayUnitLocation, TextureUnit);
}

void DrawNaviFullScreenArrayTechnique::SetGroupNumFloat(float g_id)
{
    glUniform1f(m_navigroupNumLocation, g_id);
}


