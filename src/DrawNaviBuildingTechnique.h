#ifndef DRAW_NAVI_BUILDING_TECHNIQUE_H
#define	DRAW_NAVI_BUILDING_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class DrawNaviBuildingTechnique : public Technique {
public:
    DrawNaviBuildingTechnique();
    virtual bool Init();
	void SetWidthHeightUniform(const Vector2f& _widthheight); 
	void SetXAlignedUniform(const bool& _xaligned); 

private:
	GLuint m_widthheightUniformLocation;
	GLuint m_xalignedUniformLocation;
};


#endif