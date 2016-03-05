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

#ifndef BUILDINGGEOMETRY_H
#define	BUILDINGGEOMETRY_H

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

struct BuildingVS
{
	float x, y;
	int groupID; 
	int buildingID;
	//float radius; 
	float width; 
	float height; 
	int functionID; 

	BuildingVS(float _x, float _y, int _gID, int _bID, float _width, float _height, int _fID)
	{
		x = _x;
		y = _y;
		groupID = _gID; 
		buildingID = _bID; 
		functionID = _fID; 
		
		width = _width;
		height = _height; 
	}

	BuildingVS& operator=(const BuildingVS& rhs)
	{
		x = rhs.x; 
		y = rhs.y; 
		groupID = rhs.groupID; 
		buildingID = rhs.buildingID; 
		functionID = rhs.functionID; 
		width = rhs.width; 
		height = rhs.height; 

		return *this;
	};
}; 

class BuildingGeometry
{
public:
    BuildingGeometry(int _width, int _height);

    ~BuildingGeometry();

    bool Initialize(std::vector<BuildingVS>& _allbuildlings); // quad 

	bool Update(std::vector<BuildingVS>& _allbuildlings); //geometry update



    void RenderSprites();

	void RenderUncertainty(); 

	bool Initialized(){
		return m_initialized; 
	}
    
private:

	int m_width; 
	int m_height; 
	bool m_dirty; 
 
	enum VB_TYPES {
		POS_VB,
		BUILDINGID_VB,
		POS_VB_UNCERTAINTY, 
		BUILDINGID_VB_UNCERTAINTY,
		NUM_VBs            
	};

	int numberofbuildings; 

	bool m_initialized; 

    GLuint m_VAO; // 
	GLuint m_VAO_Uncertainty; 
    GLuint m_Buffers[NUM_VBs]; //
};


#endif	/* MESH_H */

