#ifndef FUTUREDISCOMFORTSPLATTER_H
#define	FUTUREDISCOMFORTSPLATTER_H

#include "technique.h"
#include "math_3d.h"

class FutureDiscomfortSplatterTechnique : public Technique {
public:

    FutureDiscomfortSplatterTechnique();

    virtual bool Init();

	void SetAgentSphereTexureUnit(unsigned int TextureUnit);
	void SetTimeStepUniform(int TimeStep);
	void SetDiscomfortCoeffUniform(int DiscomfortCoeff);
	void SetDeltaTimePerStepUniform(float DeltaTimePerStep);


private:

	GLuint m_AgentSphereTextureUnitLocation;
	GLuint m_TimeStepUniformLocation;
	GLuint m_DiscomfortCoeffUniformLocation;
	GLuint m_DeltaTimePerStepUniformLocation;
	GLuint SamplerName[1]; 
};


#endif