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

#ifndef NAVIGATIONFIELD_H
#define	NAVIGATIONFIELD_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"
#include "voronoi.h"

using namespace std;

struct navigation_struct
{
	float navi_x, navi_y;
	int pixel_x, pixel_y; 
	int id; 

	navigation_struct(int _id, int _px, int _py, float _nx = 0.f, float _ny = 0.0f)
	{
		id = _id; 
		 
		pixel_x = _px; 
		pixel_y = _py; 

		navi_x = _nx;
		navi_y = _ny;


	}

	navigation_struct& operator=(const navigation_struct& rhs){
		pixel_x = rhs.pixel_x; 
		pixel_y = rhs.pixel_y; 
		navi_x = rhs.navi_x; 
		navi_y = rhs.navi_y; 
		id = rhs.id; 
		return *this;
	};
};

class NavigationField
{
public:
    NavigationField(int _width, int _height);  //32 * 32

    ~NavigationField();

    bool LoadfromFile(const char* filepath, int _id);

	void AddGroupNavigation(std::vector<navigation_struct>& _navi);

	//void DrawAgents(int groupnum, bool all = false); 

	//void ClinchBuildings(std::vector<BuildingVS>& _allbuildlings); 
	
  //  uint NumBuildingGroup() const
  //  {
		//return m_buildingcollection.size();
  //  }

	uint NumNavigationFields() const
	{
		return m_navigationCollection.size(); 
	}

    
public:
	int m_width, m_height; 

	std::vector<std::vector<navigation_struct>> m_navigationCollection;
};


#endif	