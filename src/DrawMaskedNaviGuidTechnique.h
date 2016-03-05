#ifndef DRAWMASKEDNAVIGUID_TECHNIQUE_H
#define	DRAWMASKEDNAVIGUID_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class DrawMaskedNaviGuidTechnique : public Technique {
public:

    DrawMaskedNaviGuidTechnique();

    virtual bool Init();

	void SetMaskedNaviArrayTextureUnit(unsigned int TextureUnit);
	void SetMaskedGuidArrayTextureUnit(unsigned int TextureUnit); 
	void SetGroupIDUniform(unsigned int groupID); 
	void SetDrawNaviUniform(unsigned int GroupID); 

private:

	GLuint m_MaskedNaviArrayTextureUnitLocation;
	GLuint m_MaskedGuidArrayTextureUnitLocation;
	GLuint m_GroupIDUniformLocation; 
	GLuint m_drawNaviUniformLocation; 
};


#endif