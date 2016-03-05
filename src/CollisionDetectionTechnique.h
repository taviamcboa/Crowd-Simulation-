#ifndef COLLISIONDETECTION_TECHNIQUE_H
#define	COLLISIONDETECTION_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class CollisionDetectionTechnique : public Technique {
public:

    CollisionDetectionTechnique();

    virtual bool Init();

	void SetAgentVoronoiImageUnit(unsigned int TextureUnit);
	void SetMarkerSamplingImageUnit(unsigned int TextureUnit); 
	void SetEularianMapImageUnit(unsigned int TextureUnit); 
	void SetGuidanceImageUnit(unsigned int TextureUnit);
	void SetDeltaTimeUniform(float deltatime); 
	void SetHasGUIDUniformLocation(int hasGUID); 

private:

	GLuint m_AgentVoronoiImageUnitLocation;
	GLuint m_MarkerSamplingImageUnitLocation;
	GLuint m_EularianMapImageUnitLocation;
	GLuint m_GuidanceImageUnitLocation;
	GLuint m_DeltaTimeUniformLocation; 
	
	GLuint m_HasGUIDUniformLocation; 
};


#endif