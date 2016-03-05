#ifndef SPEED_CALCULATOR_TECHNIQUE_H
#define	SPEED_CALCULATOR_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class SpeedCalculatorTechnique : public Technique {
public:
    SpeedCalculatorTechnique();
    virtual bool Init();
	void SetReadDensityVelocityImageUnit(unsigned int ImageUnit);
	void SetWriteDensityVelocityImageUnit(unsigned int ImageUnit);
private:
	GLuint m_ReadDensityVelocityImageUnitLocation;
	GLuint m_WriteSpeedImageUnitLocation; 
};


#endif