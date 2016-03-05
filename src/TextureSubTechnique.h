#ifndef TEXTURESUBTECHNIQUE_H
#define	TEXTURESUBTECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class TextureSubTechnique : public Technique {
public:

    TextureSubTechnique();

    virtual bool Init();

	void SetMarkerSamplingTextureUnit1(unsigned int TextureUnit);
	void SetMarkerSamplingTextureUnit2(unsigned int TextureUnit);

private:

	GLuint m_markerSamplingTextureUnitLocation1;
	GLuint m_markerSamplingTextureUnitLocation2;
	GLuint SamplerName; // 3 TEXTURE NEED TO SAMPLE
};


#endif