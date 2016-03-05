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

#include "DrawBuildingSprite_technique.h"
#include "util.h"

static const char* pEffectFile = "shaders/DrawBuildings.glsl";

DrawBuildingSpriteTechnique::DrawBuildingSpriteTechnique() : Technique(pEffectFile)
{   
}

bool DrawBuildingSpriteTechnique::Init()
{
    if (!CompileProgram("DrawBuildingsTechnique")) {
        return false;
    }

	return true;
}


