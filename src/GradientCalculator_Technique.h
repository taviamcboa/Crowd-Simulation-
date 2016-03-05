#ifndef GRADIENT_CALC_TECHNIQUE_H
#define	GRADIENT_CALC_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class GradientCalcTechnique : public Technique {
public:
    GradientCalcTechnique();
    virtual bool Init();
	void SetPotentialReadImageUnit(unsigned int ImageUnit);
	void SetGradient12WriteImageUnit(unsigned int ImageUnit);
	void SetGradient34WriteImageUnit(unsigned int ImageUnit);
	void SetGradientBufferWriteImageUnit(unsigned int ImageUnit); 
 
private:
	GLuint m_potentialReadImageUnitLocation; 
	GLuint m_gradient12WriteImageUnitLocation; 
	GLuint m_gradient34WriteImageUnitLocation; 
	GLuint m_gradientBufferWriteImageUnitLocation; 
};


#endif