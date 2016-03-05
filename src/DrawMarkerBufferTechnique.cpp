#include <limits.h>
#include <string.h>

#include "DrawMarkerBufferTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/DrawMarkerBuffer.glsl";

DrawMarkerBufferTechnique::DrawMarkerBufferTechnique() : Technique(pEffectFile)
{   
}

bool DrawMarkerBufferTechnique::Init()
{
    if (!CompileProgram("DrawMarkersBufferTechnique")) {
        return false;
    }

	return true;
}

