#ifndef Uncertainty_Smoother_TECHNIQUE_H
#define	Uncertainty_Smoother_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class UncertaintySmootherTechnique : public Technique {
public:
    UncertaintySmootherTechnique();
    virtual bool Init();
	void SetUncertaintyImageUnit(unsigned int ImageUnit);
	void SetEularianImageUnit(unsigned int ImageUnit);
	void SetFirstTimeUniform(int FirstTime); 
private:
	GLuint m_uncertaintyImageUnitLocation;
	GLuint m_eularianImageUnitLocation;
	GLuint m_firsttimeUniformLocation; 
};


#endif