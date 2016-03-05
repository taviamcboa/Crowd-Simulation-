#ifndef COST_CALCULATOR_TECHNIQUE_H
#define	COST_CALCULATOR_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class CostCalculatorTechnique : public Technique {
public:
    CostCalculatorTechnique();
    virtual bool Init();
	void SetDensityVelocityReadOnlyImageUnit(unsigned int ImageUnit);
	void SetSpeedReadOnlyImageUnit(unsigned int ImageUnit);
	void SetCostWriteOnlyImageUnit(unsigned int ImageUnit);
	void SetEularianMapImageUnit(unsigned int ImageUnit); 

    
	//
	void SetWeightDiscomfort(float discomfort); 
	void SetWeightPathLength(float pathlength); 
	void SetWeightTime(float time); 
	void SetPredictable(int predictable); 

private:
	GLuint m_densityVelocityReadOnlyImageUnitLocation;
	GLuint m_speedReadOnlyImageUnitLocation;
	GLuint m_costWriteOnlyImageUnitLocation;
	GLuint m_eularianmapImageUnitLocation;



	GLuint m_weightDiscomfort; 
	GLuint m_weightPathLength; 
	GLuint m_weightTime; 

	GLuint m_predictable; 

};


#endif