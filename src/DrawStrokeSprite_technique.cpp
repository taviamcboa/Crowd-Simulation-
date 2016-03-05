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

#include "DrawStrokeSprite_technique.h"
#include "util.h"

static const char* pEffectFile = "shaders/DrawStroke.glsl";

DrawStrokeSpriteTechnique::DrawStrokeSpriteTechnique() : Technique(pEffectFile)
{   
}

bool DrawStrokeSpriteTechnique::Init()
{
    if (!CompileProgram("DrawStrokeTechnique")) {
        return false;
    }

    m_activegroupLocation = GetUniformLocation("g_id");

	if(m_activegroupLocation == INVALID_UNIFORM_LOCATION){
		return false;
	}

	return true;
}

void DrawStrokeSpriteTechnique::SetActiveGroupFloat(float g_id)
{
    glUniform1f(m_activegroupLocation, g_id);
}

