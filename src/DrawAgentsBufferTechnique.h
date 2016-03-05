#ifndef DRAW_AGENTS_TECHNIQUE_H
#define	DRAW_AGENTS_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class DrawAgentsBufferTechnique : public Technique {
public:
    DrawAgentsBufferTechnique();
    virtual bool Init();
	void SetUncertaintyImageUnit(unsigned int ImageUnit);
private:

	GLuint m_uncertaintyImageUnitLocation;
};


#endif