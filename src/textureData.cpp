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

#include <iostream>
#include "util.h"
#include "textureData.h"

TextureData::TextureData(GLenum TextureTarget, NavigationField2D* NavigationField)
{
    m_textureTarget = TextureTarget;
    m_NavigationField      = NavigationField;
    Data        = NULL;
	initialized = false; 
}

TextureData::~TextureData()
{
	  // Release the texture object.
   delete[] Data; 
   glDeleteTextures(1, &m_textureObj);

}

bool TextureData::Load()
{
	if(initialized == false){
		glGenTextures(1, &m_textureObj);
	}
    glBindTexture(m_textureTarget, m_textureObj);

	unsigned int width, height;
	width = height = (int)sqrt(float(m_NavigationField->m_navigationfield2D.size()));
	m_width = width; 
	m_height = height; 
	if(initialized == false){
		this->Data = new float[width * height * 4]; 
	}
	//column * rows
	int value; 
	int gridx, gridz; 
	
	for(int i = 0 ; i< width * height; i++)
	{
		gridx = i/width; 
		gridz = i-gridx*width; 

		float gradx, gradz; 
		gradx = m_NavigationField->readState(gridx, gridz).navix; 
		gradz = m_NavigationField->readState(gridx, gridz).naviz; 

		this->Data[4*i + 0]  = abs(gradx);
	//  this->Data[4*i + 0] = 255;
		this->Data[4*i + 1] = abs(gradz);
		this->Data[4*i + 2] = gradx > 0? 1.0 : 0.0f; 
		this->Data[4*i + 3] = gradz > 0? 1.0 : 0.0f; 
	}

  //  glTexImage2D(m_textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
	glTexImage2D(m_textureTarget, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_FLOAT, Data);
    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	initialized = true; 
    return GLCheckError();
}


//bool TextureData::LoadArray()
//{
//	GLint MaxTextureArrayLayers(0);
//	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &MaxTextureArrayLayers);
//
//	if(initialized == false)
//	{
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//		glGenTextures(1, &m_textureObj);
//	}
//    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureObj);
//
//	unsigned int width, height;
//	width = height = (int)sqrt(float(m_NavigationField->m_navigationfield2D.size()));
//	if(initialized == false){
//		this->Data = new float[width * height * 4]; 
//	}
//	//column * rows
//	int value; 
//	int gridx, gridz; 
//	
//	for(int i = 0 ; i< width * height; i++)
//	{
//		gridx = i/width; 
//		gridz = i-gridx*width; 
//
//		float gradx, gradz; 
//		gradx = m_NavigationField->readState(gridx, gridz).navix; 
//		gradz = m_NavigationField->readState(gridx, gridz).naviz; 
//
//		this->Data[4*i + 0]  = abs(gradx);
//	//  this->Data[4*i + 0] = 255;
//		this->Data[4*i + 1] = abs(gradz);
//		this->Data[4*i + 2] = gradx > 0? 1.0 : 0.0f; 
//		this->Data[4*i + 3] = gradz > 0? 1.0 : 0.0f; 
//	}
//
//  //  glTexImage2D(m_textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
//	glTexImage2D(m_textureTarget, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_FLOAT, Data);
//    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	initialized = true; 
//    return GLCheckError();
//}

void TextureData::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}
