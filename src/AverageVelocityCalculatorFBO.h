#ifndef AVERAGEVELOCITYCAL_H
#define	AVERAGEVELOCITYCAL_H

#include <GL/glew.h>
#include "math_3d.h"

class AverageVelocityCalculatorFBO
{

public:
    AverageVelocityCalculatorFBO();

    ~AverageVelocityCalculatorFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading(GLenum ImageUnit);

	GLuint getAverageVelocityRWImageUnit()
	{
		return m_AverageVelocityRWTexure; 
	}

private:

	GLuint m_AverageVelocityRWTexure;  //write or read depends your setting flag
};

#endif	/* SHADOWMAPFBO_H */

