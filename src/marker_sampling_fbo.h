/*
        Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARKERSAMPLING_H
#define	MARKERSAMPLING_H

#include <GL/glew.h>

class MarkerSamplingFBO
{
public:
    MarkerSamplingFBO();

    ~MarkerSamplingFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight, int agentnum, int markernum, bool debug);

    void BindForWriting();

    void BindForReading(GLenum TextureUnit);

	void BindForTextureWriting(GLenum TextureUnit, GLenum TextureUnit_Index);

	GLuint getFBO(){

		return m_fbo; 
	}
	GLuint getWriteBuffer(){

		return m_writebuffer; 
	}
	inline bool isDebugMode(){
		return m_debug; 
	}
	inline int getAgentNum()
	{
		return m_agentnum; 
	}
	inline int getMarkerNum()
	{
		return m_markernum; 
	}


private:
	bool m_debug; 

	int m_markernum; 
	int m_agentnum; 

    GLuint m_fbo;
    GLuint m_markersamplingmap; //texture2d HANDLE
	GLuint m_depthTexture;

	//for debug purposes
	GLuint m_writebuffer; //index front-end
	GLuint m_writeTex; //back-end storage
};

#endif	/* SHADOWMAPFBO_H */

