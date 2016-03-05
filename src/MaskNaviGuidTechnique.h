#ifndef MASK_NAVI_GUID_TECHNIQUE_H
#define	MASK_NAVI_GUID_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class MaskNaviGuidTechnique : public Technique {
public:

    MaskNaviGuidTechnique();
    virtual bool Init();
	void SetNaviMaskImageUnit(unsigned int ImageUnit); 
	void SetGuidArrayImageUnit(unsigned int ImageUnit);
	void SetNaviArrayImageUnit(unsigned int ImageUnit);
	void SetMaskedGuidArrayImageUnit(unsigned int ImageUnit);
	void SetMaskedNaviArrayImageUnit(unsigned int ImageUnit);

private:
	GLuint m_NaviMaskImageUnitLocation; 
	GLuint m_GuidArrayImageUnitLocation; 
	GLuint m_NaviArrayImageUnitLocation; 
	GLuint m_MaskedGuidArrayImageUnitLocation; 
	GLuint m_MaskedNaviArrayImageUnitLocation; 
};


#endif