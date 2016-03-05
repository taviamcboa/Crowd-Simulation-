#ifndef MARKERSAMPLINGBUFFER_TECHNIQUE_H
#define	MARKERSAMPLINGBUFFER_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class MarkerSamplingBufferTechnique : public Technique {
public:

    MarkerSamplingBufferTechnique();

    virtual bool Init();

	void SetAgentVoronoiTextureUnit(unsigned int TextureUnit);
	void SetEularianTextureUnit(unsigned int TextureUnit);


private:

	GLuint m_AgentVoronoiTextureUnitLocation;
	GLuint m_EularianTextureUnitLocation; 
	GLuint SamplerName[2]; // 3 TEXTURE NEED TO SAMPLE
};


#endif