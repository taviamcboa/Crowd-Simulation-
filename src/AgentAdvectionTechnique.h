#ifndef AGENT_ADVECTION_TECHNIQUE_H
#define	AGENT_ADVECTION_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class AgentAdvectionTechnique : public Technique {
public:
    AgentAdvectionTechnique();
    virtual bool Init();
	void SetGradient01ImageUnit(unsigned int ImageUnit);
	void SetGradient23ImageUnit(unsigned int ImageUnit);
	void SetSpeedImageUnit(unsigned int ImageUnit);
	void SetPotentialImageUnit(unsigned int ImageUnit);
	void SetUncertaintyImageUnit(unsigned int ImageUnit); 
	void SetMaskedGuidanceImageUnit(unsigned int ImageUnit); 
	void SetMarkerSamplingImageUnit(unsigned int ImageUnit); 

	void SetDeltaTimeLocation(float deltatime); 
	void SetExitMode(int exitmode); 
	void SetBuildingCt(int bct); 
	void SetHasGUID(int hguid); 
 

private:
	GLuint m_gradient01ImageUnitLocation;
	GLuint m_gradient23ImageUnitLocation;
	GLuint m_speedImageUnitLocation;
	GLuint m_potentialImageUnitLocation;
	GLuint m_uncertaintyImageUnitLocation;
	GLuint m_maskedGuidanceImageUnitLocation; 
	GLuint m_markerSamplingImageUnitLocation; 





	//
	GLuint m_DeltaTimeLocation; 
	GLuint m_ExitModeLocation; 
	GLuint m_BuildingCout; 
	GLuint m_HasGUID; 

};


#endif