#ifndef NAVIMASKFBO_H
#define	NAVIMASKFBO_H

#include <GL/glew.h>
#include "math_3d.h"
class NaviMaskFBO
{
public:
    NaviMaskFBO();

    ~NaviMaskFBO();

    bool Init(unsigned int g_width, unsigned int g_height);

    void BindForWriting();

	void BindForImageReading(GLenum ImageUnit); 

    void BindForReading(GLenum TextureUnit);
private:

    GLuint m_fbo;
    GLuint m_navi_mask_map; //texture2d HANDLE
};

#endif	/* SHADOWMAPFBO_H */

