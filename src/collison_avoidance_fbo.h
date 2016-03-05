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

#ifndef COLLISIONAVOIDANCE_H
#define	COLLISIONAVOIDANCE_H

#include <GL/glew.h>
#include "math_3d.h"
class CollisionAvoidanceFBO
{
public:
    CollisionAvoidanceFBO();

    ~CollisionAvoidanceFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();
	void BindForWritingDensityFrameBuffer();  //clean the buffer
	void BindForWritingAntiGradientFrameBuffer(); //clean the image 
	
	void BindForStaticWriting(); 


	void BindForTextureWriting(GLenum TextureUnit, GLenum TextureUnit_Index);//REAL IMAGESTORE
	void BindForTextureReading(GLenum TextureUnit, GLenum TextureUnit_Index);//REAL IMAGESTORE
	
	void BindForDensityBufferWriting(GLenum TextureUnit, GLenum TextureUnit_Index);//REAL IMAGESTORE
	void BindForDensityWriting(GLenum TextureUnit, GLenum TextureUnit_Index);//REAL IMAGESTORE
	void BindForAntiGradientWriting(GLenum TextureUnit, GLenum TextureUnit_Index); //REAL IMAGESTORE

    void BindForReading(GLenum TextureUnit);
	void BindForDensityImageReading(GLenum TextureUnit_Index);
	void BindForAntiGradientImageReading(GLenum TextureUnit_Index);



	GLuint getFBO(){
		return m_fbo; 
	}
	GLuint getWriteBuffer(){
		return m_writebuffer; 
	}
	GLuint getDensityWriteBuffer(){
		return m_writeDensityBuffer; 
	}


private:
    GLuint m_fbo;
    GLuint m_collisionavoidancemap; //texture2d HANDLE
	GLuint m_depthTexture;

	//
	GLuint m_writebuffer; 
	GLuint m_writeTex; 

	GLuint m_writeDensityBuffer; 
	GLuint m_writeDensityBufferTex; 

	GLuint m_densityfbo; //just for clean data
	GLuint m_writeDensityTex;  //write or read depends your setting flag

	GLuint m_antigradientfbo;  // just for clean data
	GLuint m_writeAntiGradientTex;  //write or read depends your setting flag

};

#endif	/* SHADOWMAPFBO_H */

