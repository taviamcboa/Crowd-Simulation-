#include <limits.h>
#include <string.h>

#include "VoronoiTessallationTechnique.h"
#include "util.h"

static const char* pEffectFile = "shaders/VoronoiTessellation.glsl";

VoronoiTessallationTechnique::VoronoiTessallationTechnique() : Technique(pEffectFile)
{   

}

bool VoronoiTessallationTechnique::Init()
{
    if (!CompileProgram("DrawAgentsTechnique")) {
        return false;
    }

	/*mNumConeSlices = GetUniformLocation("num_cone_slices"); 
	mRadius = GetUniformLocation("radius"); 
	
	if (mNumConeSlices == INVALID_UNIFORM_LOCATION
		||mRadius == INVALID_UNIFORM_LOCATION)
    {
        return false;
    }*/


	return true;
}

void VoronoiTessallationTechnique::SetNumConeSlices(int numslices)
{
	glUniform1i(mNumConeSlices, numslices);
}

void VoronoiTessallationTechnique::SetRadius(float radius)
{
	glUniform1i(mRadius, radius);
}
