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

#ifndef BUILDING_MANAGER_H
#define	BUILDING_MANAGER_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"
#include "voronoi.h"

#include "BuildingGeometry.h"

using namespace std;


struct building_struct
{
	float px, py;
	//float radius;
	float width; 
	float height; 
	int groupid; 
	int buildingid; 
	int functionid; 
	int active; 

	building_struct(float _px, float _py, float _width, float _height, int _groupid = 0, int _buildingid= 0, int _functionid = 0, int _active = 0 )
	{
		px = _px;
		py = _py;
		width = _width; 
		height = _height; 
		groupid= _groupid; 
		buildingid = _buildingid; 
		functionid = _functionid; 
		active = _active; 
	}

	building_struct& operator=(const building_struct& rhs){
		
		px = rhs.px; 
		py = rhs.py; 
		width = rhs.width; 
		height = rhs.height; 
		groupid = rhs.groupid; 
		buildingid = rhs.buildingid; 
		functionid = rhs.functionid;
		active = rhs.active;
		return *this;
	};
};

class BuildingManager
{
public:
    BuildingManager(int _width, int _height); 

    ~BuildingManager();

    bool LoadfromFile(const char* filepath);

	//void AddBuilding(int pixelx, int pixely, float radmin, float radmax, int groupnum, int functionID);

	void AddBuilding(int pixelx, int pixely, float width, float height, int groupnum, int functionID);


	//void DrawAgents(int groupnum, bool all = false); 

	void ClinchBuildings(std::vector<BuildingVS>& _allbuildlings); 
	
  //  uint NumBuildingGroup() const
  //  {
		//return m_buildingcollection.size();
  //  }

	uint NumBuildings() const
	{
		return m_buildingcollection.size(); 
	}

    
public:
	int m_width, m_height; 

	std::vector<building_struct> m_buildingcollection;
};


#endif	