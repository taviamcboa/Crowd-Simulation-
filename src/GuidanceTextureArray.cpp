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
#include "GuidanceTextureArray.h" 

GuidanceTextureArray::GuidanceTextureArray(GLenum TextureTarget, SketchyInterface** GuidanceField, int num)
{
    m_textureTarget = TextureTarget;
    m_GuidanceField = GuidanceField;
	m_num = num; 

    Data        = NULL;
	initialized = false; 
}

GuidanceTextureArray::~GuidanceTextureArray()
{
	  // Release the texture object.
   for(int i = 0 ; i< m_num; i ++){
	   delete Data[i]; 
   }
   delete[] Data; 
   glDeleteTextures(1, &m_texture2DArrayName);

}

bool GuidanceTextureArray::LoadArray()
{
	if(initialized == false){
		glGenTextures(1, &m_texture2DArrayName);
	}
    glBindTexture(m_textureTarget, m_texture2DArrayName); 

	unsigned int width, height;
	width = height = UtilHelper::GridDimensionX;

	if(initialized == false){

		this->Data = new float*[m_num]; 
		for(int i =0 ;i < m_num; i++){
			this->Data[i]  = new float[width * height * 4]; 
		}
	}
	//column * rows
	int value; 
	int gridx, gridz; 
	int gridNx, gridNz; 
	int gridWx, gridWz; 
	int gridSx, gridSz; 
	int gridEx, gridEz; 

	for(int id = 0; id< m_num; id++){
		for(int i = 0 ; i< width * height; i++)
		{
			//gridx = i/width; 
			//gridz = i-gridx*width; 
			gridx = i/width; 
			gridz = i-gridx*width; 

			float guidx, guidz; 
			float guidNx, guidNz; 
			float guidWx, guidWz; 
			float guidSx, guidSz; 
			float guidEx, guidEz; 

			gridNx = gridx; 
			gridNz = gridz + 1; 

			gridWx = gridx - 1; 
			gridWz = gridz; 
			
			gridSx = gridx; 
			gridSz = gridz - 1; 
			
			gridEx = gridx + 1; 
			gridEz = gridz; 

			guidx = m_GuidanceField[id]->readSketchyGradientCopy(gridz, gridx).gradientx; 
			guidz = m_GuidanceField[id]->readSketchyGradientCopy(gridz, gridx).gradientz; 

			this->Data[id][4*i + 0] = abs(guidx);
		//  this->Data[4*i + 0] = 255;
			this->Data[id][4*i + 1] = abs(guidz);
			this->Data[id][4*i + 2] = guidx > 0? 1.0 : 0.0f; 
			this->Data[id][4*i + 3] = guidz > 0? 1.0 : 0.0f; 
		}
	}

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, width, height, m_num); //depth
	for(int Array = 0; Array < m_num; ++Array)
	{
		for(int Level = 0; Level < 1; ++Level)
		{	
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, GLint(Level), 
			0, // offset x 
			0, // offset y 
			GLint(Array),// offset z
			GLsizei(width), 
			GLsizei(height), 
			GLsizei(1), //depth
			GL_RGBA, //format
			GL_FLOAT, //type
			this->Data[Array]);
		}
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);


 // //  glTexImage2D(m_textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
	//glTexImage2D(m_textureTarget, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_FLOAT, Data);
 //   glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

void GuidanceTextureArray::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_texture2DArrayName);
}

void GuidanceTextureArray::BindImageArray(GLenum ImageUnit)
{
	glBindImageTexture(ImageUnit, m_texture2DArrayName, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
}