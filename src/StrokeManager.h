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

#ifndef STROKE_MANAGER_H
#define	STROKE_MANAGER_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"
#include "voronoi.h"

#include "StrokeGeometry.h"

using namespace std;

struct stroke_struct
{
	float px, py;
	int groupid; 

	stroke_struct(float _px, float _py, int _groupid = 0)
	{
		px = _px;
		py = _py;
		groupid= _groupid; 
	}

	stroke_struct& operator=(const stroke_struct& rhs){
		
		px = rhs.px; 
		py = rhs.py; 
		groupid = rhs.groupid; 
		return *this;
	};
};

class StrokeManager
{
public:
    StrokeManager(int _width, int _height); 

    ~StrokeManager();

    bool LoadfromFile(const char* filepath);

	void AddStroke(int pixelx, int pixely, int groupID);

	//void DrawAgents(int groupnum, bool all = false); 

	void ClinchStrokes(std::vector<StrokeVS>& _allstrokes); 
	
  //  uint NumBuildingGroup() const
  //  {
		//return m_buildingcollection.size();
  //  }

	uint NumStrokes() const
	{
		return m_strokecollection.size(); 
	}

    
public:
	int m_width, m_height; 

	std::vector<stroke_struct> m_strokecollection;
};


#endif	