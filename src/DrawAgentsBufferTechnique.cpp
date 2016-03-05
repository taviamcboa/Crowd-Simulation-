#include <limits.h>
#include <string.h>

#include "DrawAgentsBufferTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/DrawAgentsBuffer.glsl";

DrawAgentsBufferTechnique::DrawAgentsBufferTechnique() : Technique(pEffectFile)
{   
}

bool DrawAgentsBufferTechnique::Init()
{
    if (!CompileProgram("DrawAgentsBufferTechnique")) {
        return false;
    }
	m_uncertaintyImageUnitLocation = GetUniformLocation("gUncertaintyImage");
	if (m_uncertaintyImageUnitLocation == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }
	return true;
}

void DrawAgentsBufferTechnique::SetUncertaintyImageUnit(unsigned int ImageUnit)
{
	glUniform1i(m_uncertaintyImageUnitLocation, ImageUnit);
}
