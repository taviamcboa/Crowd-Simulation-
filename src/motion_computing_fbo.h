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

#ifndef MOTIONCOMPUTING_H
#define	MOTIONCOMPUTING_H

#include <GL/glew.h>
#include "math_3d.h"
class MotionComputingFBO
{
public:
    MotionComputingFBO();

    ~MotionComputingFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

	void BindForTextureWriting(GLenum TextureUnit, GLenum TextureUnit_Index);

    void BindForReading(GLenum TextureUnit);

	GLuint getFBO(){

		return m_fbo; 
	}
	GLuint getWriteBuffer(){

		return m_writebuffer; 
	}


private:

    GLuint m_fbo;
    GLuint m_motioncomputingmap; //texture2d HANDLE
	GLuint m_depthTexture;

	//
	GLuint m_writebuffer; 
	GLuint m_writeTex; 
};

#endif	/* SHADOWMAPFBO_H */

