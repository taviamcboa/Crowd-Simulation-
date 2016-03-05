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

#include "DrawNaviFullScreen_technique.h"
#include "util.h"

static const char* pEffectFile = "shaders/ReadNaviTex.glsl";

DrawNaviFullScreenTechnique::DrawNaviFullScreenTechnique() : Technique(pEffectFile)
{   
}

bool DrawNaviFullScreenTechnique::Init()
{
    if (!CompileProgram("ReadNaviTexTechnique")) {
        return false;
    }

	m_naviTextureUnitLocation = GetUniformLocation("gNaviTex");

	if (m_naviTextureUnitLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	return true;
}

void DrawNaviFullScreenTechnique::SetNaviTexUnit(unsigned int TextureUnit)
{
    glUniform1i(m_naviTextureUnitLocation, TextureUnit);
}

