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

#ifndef TEXTUREARRAYDATA_H
#define	TEXTUREARRAYDATA_H

#include <string>

#include <GL/glew.h>
#include "NavigationField2D.h"


class TextureArrayData
{
public:
    TextureArrayData(GLenum TextureTarget, NavigationField2D** NavigationField, int num);
	~TextureArrayData(); 

	bool LoadArray(); 

    void Bind(GLenum TextureUnit);
	void BindImageArray(GLenum ImageUnit); 

private:
	int m_num; 
    NavigationField2D **m_NavigationField;
    GLenum m_textureTarget; //GL_TEXTURE_2D_ARRAY
    GLuint m_texture2DArrayName; //
	bool initialized; 
	float **Data;
};


#endif	/* TEXTURE_H */

