#ifndef AVERAGE_VELOCITY_CALCULATOR_TECHNIQUE_H
#define	AVERAGE_VELOCITY_CALCULATOR_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class AverageVelocityCalculatorTechnique : public Technique {
public:
    AverageVelocityCalculatorTechnique();
    virtual bool Init();
	void SetDensityVelocityImageUnit(unsigned int ImageUnit);
private:
	GLuint m_densityVelocityImageUnitLocation;
};


#endif