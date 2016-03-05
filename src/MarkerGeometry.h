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

#ifndef MARKERGEOMETRY_H
#define	MARKERGEOMETRY_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"

#include "marker_mesh.h"

using namespace std;

class MarkerGeometry
{
public:
    MarkerGeometry(int _width, int _height);

    ~MarkerGeometry();

    bool Initialize(MarkerMesh &_markermesh, bool _round =true);

    void Render();
    
private:

	int m_width; 
	int m_height; 
 
	enum VB_TYPES {
		POS_VB,
		TEXCOORD_VB,
		MARKERID_VB, 
		NUM_VBs            
	};

	int numberofmarkers; 

    GLuint m_VAO;
    GLuint m_Buffers[NUM_VBs];


};


#endif	/* MESH_H */

