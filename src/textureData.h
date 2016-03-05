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

#ifndef TEXTUREDATA_H
#define	TEXTUREDATA_H

#include <string>

#include <GL/glew.h>
#include "NavigationField2D.h"


class TextureData
{
public:
    TextureData(GLenum TextureTarget, NavigationField2D* NavigationField);
	~TextureData(); 

    bool Load();
	inline float* ReadLocalCopy() const
	{
		return Data; 	
	}

	inline uint Width() const
	{

		return m_width; 
	}

	inline uint Height() const
	{

		return m_height; 
	}

	bool LoadArray(); 

    void Bind(GLenum TextureUnit);

private:
    NavigationField2D *m_NavigationField;
    GLenum m_textureTarget;
    GLuint m_textureObj;
	uint m_width, m_height; 
	bool initialized; 
	float *Data;
};


#endif	/* TEXTURE_H */

