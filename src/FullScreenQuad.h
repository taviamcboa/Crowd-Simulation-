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

#ifndef FULLSCREENQUAD_H
#define	FULLSCREENQUAD_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"

using namespace std;

class FullScreenQuad
{
public:
    FullScreenQuad();

    ~FullScreenQuad();

    bool Initialize();

    void Render();
    
private:
 
	enum VB_TYPES {
		INDEX_BUFFER,
		POS_VB,
		TEXCOORD_VB,
		NUM_VBs            
	};

    GLuint m_VAO;
    GLuint m_Buffers[NUM_VBs];


};


#endif	/* MESH_H */

