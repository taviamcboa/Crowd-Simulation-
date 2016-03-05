#include <limits.h>
#include <string.h>

#include "DrawNaviBuildingTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/DrawNaviBuilding.glsl";

DrawNaviBuildingTechnique::DrawNaviBuildingTechnique() : Technique(pEffectFile)
{   
}

bool DrawNaviBuildingTechnique::Init()
{
    if (!CompileProgram("DrawNaviBuildingTechnique")) {
        return false;
    }

	m_xalignedUniformLocation = GetUniformLocation("xaligned");
	m_widthheightUniformLocation = GetUniformLocation("widthheight");
	if (m_widthheightUniformLocation == INVALID_UNIFORM_LOCATION
		||m_xalignedUniformLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }

	return true;
}

void DrawNaviBuildingTechnique::SetWidthHeightUniform(const Vector2f& _widthheight)
{
	glUniform2f(m_widthheightUniformLocation, _widthheight.x, _widthheight.y);
}

void DrawNaviBuildingTechnique::SetXAlignedUniform(const bool& _xaligned)
{
	glUniform1i(m_xalignedUniformLocation, _xaligned);
}