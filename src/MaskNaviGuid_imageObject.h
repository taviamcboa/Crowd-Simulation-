
#ifndef MASKNAVIGUIDIMAGEOBJECT_H
#define	MASKNAVIGUIDIMAGEOBJECT_H

#include <GL/glew.h>
#include "math_3d.h"

class MaskedNaviGuidImageObject
{
public:
    MaskedNaviGuidImageObject();

    ~MaskedNaviGuidImageObject();

    bool Init(unsigned int GridWidth, unsigned int GridHeight, unsigned int groupnum);

    void BindForMaskedNaviImageArrayUnitWriting(GLenum ImageUnit);

	void BindForMaskedNaviImageArrayUnitReading(GLenum ImageUnit); 

	void BindForMaskedNaviTextureArrayReading(GLenum TextureUnit); 

	//
    void BindForMaskedGuidImageArrayUnitWriting(GLenum ImageUnit);

	void BindForMaskedGuidImageArrayUnitReading(GLenum ImageUnit); 

	void BindForMaskedGuidTextureArrayReading(GLenum TextureUnit); 

	void BindForShaderStorageBuffer(); 

	GLuint GUID01Buffer()
	{
		return m_Buffers[SHADERSTORAGEBUFFER_TYPES::GUID01];
	}
	GLuint GUID23Buffer()
	{
		return m_Buffers[SHADERSTORAGEBUFFER_TYPES::GUID23];
	}
	GLuint NAVI01Buffer()
	{
		return m_Buffers[SHADERSTORAGEBUFFER_TYPES::NAVI01];
	}
	GLuint NAVI23Buffer()
	{
		return m_Buffers[SHADERSTORAGEBUFFER_TYPES::NAVI23];
	}
private:

    GLuint m_masked_navi_texture; //texture2d HANDLE WILL BE BIND AS IMAGE UNIT
	GLuint m_masked_guid_texture; 

	enum SHADERSTORAGEBUFFER_TYPES {
		GUID01,
		GUID23,
		NAVI01,
		NAVI23,
		NUM_VBs            
	};

	GLuint m_Buffers[NUM_VBs];
};

#endif	

