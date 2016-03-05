#ifndef DRAW_NAVI_MASK_TECHNIQUE_H
#define	DRAW_NAVI_MASK_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class DrawNaviMaskTechnique : public Technique {
public:
    DrawNaviMaskTechnique();
	void SetEularianTexture(unsigned int TextureUnit);
    virtual bool Init();
private:
	GLuint SamplerName; // 3 TEXTURE NEED TO SAMPLE
	GLuint m_eularianTextureUnitLocation1; 
};


#endif