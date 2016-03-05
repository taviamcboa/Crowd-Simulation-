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

#ifndef AGENT_MANAGER_H
#define	AGENT_MANAGER_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"
#include "voronoi.h"

#include "agent_mesh.h"

#include "AgentBufferGeometry.h"

using namespace std;

struct agent_struct
{
	float px, py; //current position 
	float initialpx, initialpy; //initial position
	float smax; //maxspeed
	float scurrent;  // speed
	float dirx, diry;  // direction
	float dirx_c, diry_c; 
	float gx, gy;  // goal point

	int groupid;  // group ID
	int agentid;  // Agent ID
	int active;  // parked 

	float parkx, parky; 

	agent_struct(float _px, float _py, float _parkx, float _parky, float _smax = 0.0f, float _scurrent = 0.0f, float _dirx = 0.0f, float _diry = 0.0f, 
		float _dirx_c = 0.0f, float _diry_c = 0.0f, 
		float _gx = 0.0f, float _gy = 0.0f, int _groupid = 0, int _agentid= 0, int _active = 1 )
	{
		px = _px;
		py = _py;

		initialpx = px; 
		initialpy = py; 

		smax = _smax; 
		scurrent = _scurrent; 

		dirx = _dirx; 
		diry = _diry; 

		dirx_c = _dirx_c; 
		diry_c = _diry_c; 

		gx = _gx; 
		gy = _gy; 

		groupid= _groupid; 
		agentid = _agentid; 

		active = _active; 

		parkx = _parkx;
		parky = _parky;
	}

	agent_struct& operator=(const agent_struct& rhs){
		
		px = rhs.px; 
		py = rhs.py; 

		initialpx = rhs.initialpx; 
		initialpy = rhs.initialpy; 

		smax = rhs.smax; 
		scurrent = rhs.scurrent; 

		dirx = rhs.dirx;
		diry = rhs.diry;

		dirx_c = rhs.dirx_c; 
		diry_c = rhs.diry_c; 

		gx = rhs.gx;
		gy = rhs.gy;

		groupid = rhs.groupid;
		agentid = rhs.agentid; 

		active = rhs.active; 

		parkx = rhs.parkx; 
		parky = rhs.parky; 

		return *this;
	};
};

class AgentManager
{
public:

    AgentManager(int _width, int _height); 

    ~AgentManager();

    bool LoadfromFile(const char* filepath);

	void AddGroup(std::vector<point_struct>& agents, Vector2f minR, Vector2f maxR,Vector2f minGoalR, 
		Vector2f maxGoalR, float smin, float smax, int groupID);

	void DrawAgents(int groupnum, bool all = false); 

	void ClinchAgents(std::vector<point_struct>& clichagents); 

    uint NumAgentsGroup() const
    {
		return m_agentcollection.size();
    }

	uint NumAgents() const
	{
		uint num = 0; 
		for(int i =0 ; i< NumAgentsGroup(); i++){

			num += m_agentcollection[i].size(); 
		}
		return num; 
	}

protected:

	void DrawCircle(float x, float y, float r, int segments); 
    
public:
	int m_width, m_height; 

	std::vector<std::vector<agent_struct>> m_agentcollection;

	friend class AgentBufferGeometry;
};


#endif	