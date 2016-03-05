#ifndef VORONOI_TESSALLATION_TECHNIQUE_H
#define	VORONOI_TESSALLATION_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class VoronoiTessallationTechnique : public Technique {
public:
    VoronoiTessallationTechnique();

    virtual bool Init();

	void SetNumConeSlices(int numslices); 

	void SetRadius(float radius); 
	
private:

	GLuint mNumConeSlices; 
	GLuint mRadius; 
 
};


#endif