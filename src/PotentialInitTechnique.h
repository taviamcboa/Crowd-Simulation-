#ifndef POTENTIAL_INIT_TECHNIQUE_H
#define	POTENTIAL_INIT_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class PotentialInitTechnique : public Technique {
public:
    PotentialInitTechnique();
    virtual bool Init();
	void SetPotentialWriteImageUnit(unsigned int ImageUnit);
 
private:
	GLuint m_potentialWriteImageUnitLocation;
};


#endif