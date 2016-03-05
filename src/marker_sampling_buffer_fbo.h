#ifndef MARKERSAMPLINGBUFFER_H
#define	MARKERSAMPLINGBUFFER_H

#include <GL/glew.h>

class MarkerSamplingBufferFBO
{
public:
    MarkerSamplingBufferFBO();

    ~MarkerSamplingBufferFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading(GLenum TextureUnit);

	void BindForImageReading(GLenum ImageUnit); 

	GLuint getFBO(){

		return m_fbo; 
	}
	
private:
    GLuint m_fbo;

    GLuint m_markersamplingmap; //texture2d HANDLE
};

#endif	/* SHADOWMAPFBO_H */

