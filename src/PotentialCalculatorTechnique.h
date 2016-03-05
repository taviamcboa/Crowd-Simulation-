#ifndef POTENTIAL_CALC_TECHNIQUE_H
#define	POTENTIAL_CALC_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class PotentialCalcTechnique : public Technique {
public:
    PotentialCalcTechnique();
    virtual bool Init();
	void SetPotentialReadWriteImageUnit(unsigned int ImageUnit);
	void SetCostReadOnlyImageUnit(unsigned int ImageUnit); 
	void SetPotentialWriteBufferUnit(unsigned int BufferUnit); 
	void SetNeighboringPotentialWriteBufferUnit(unsigned int BufferUnit); 
	void SetNeighboringCostWriteBufferUnit(unsigned int BufferUnit);
	void SetLoopCount( int LoopCount); 
 
private:
	GLuint m_potentialReadWriteImageUnitLocation;
	GLuint m_costReadOnlyImageUnitLocation; 
	GLuint m_potentialWriteBufferUnitLocation; 
	GLuint m_neighboringPotentialWriteBufferUnitLocation; 
	GLuint m_neighboringCostWriteBufferUnitLocation; 
	GLuint m_loopCount; 
};


#endif