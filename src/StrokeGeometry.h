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

#ifndef STROKEGEOMETRY_H
#define	STROKEGEOMETRY_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"

//#include "agent_mesh.h"
//#include "AgentManager.h"

using namespace std;

struct StrokeVS
{
	float x, y;
	int groupID; 

	StrokeVS(float _x, float _y, int _gID)
	{
		x = _x;
		y = _y;
		groupID = _gID; 
	}

	StrokeVS& operator=(const StrokeVS& rhs)
	{
		x = rhs.x; 
		y = rhs.y; 
		groupID = rhs.groupID; 
		return *this;
	};
}; 

class StrokeGeometry
{
public:
    StrokeGeometry(int _width, int _height);

    ~StrokeGeometry();

    bool Initialize(std::vector<StrokeVS>& _allstrokes);

	bool Update(std::vector<StrokeVS>& _allstrokes); //geometry update

    void RenderSprites();

	bool Initialized()
	{
		return m_initialized; 
	}
    
private:

	int m_width; 
	int m_height; 
 
	enum VB_TYPES {
		POS_VB,
		NUM_VBs            
	};

	int numberofstrokes; 

	bool m_initialized; 

    GLuint m_VAO; // 
    GLuint m_Buffers[NUM_VBs]; //
};


#endif	/* MESH_H */

