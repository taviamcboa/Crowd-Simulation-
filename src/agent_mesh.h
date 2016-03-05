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

#ifndef AGENT_MESH_H
#define	AGENT_MESH_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"
#include "voronoi.h"

using namespace std;
	typedef struct {
		float x, y;
	} point_struct;

class AgentMesh
{
public:
   AgentMesh(int groupID, int agentct, int width, int height); 

    ~AgentMesh();

    bool InitializeMesh(int agentct, int agentgroup);

	bool ClinchMesh(std::vector<point_struct>& agents); // memory new and release only used during first time

	bool UpdateMesh(std::vector<point_struct>& agents);  // no memeory 

    void Render();

	void Read();

	void DrawDebug(bool draw_centroids,bool keypoints); 
	
    uint NumAgents() const
    {
        return m_NumAgents;
    }


 
	void gen_density_function();
    
public:
	int m_width, m_height; 
	int m_NumAgents; 
	int m_groupID; 

	vor_statep state_agents;



	//typedef struct {
	//	float px, py;
	//	float smax; 
	//	float dirx, diry; 
	//	float gx, gy;
	//	int g_id; 
	//} Agent_struct; 
    
	point_struct* point_vertices_agents;
	//Agent_struct* simulator_agents; 

	int draw_centroids; 
	int use_density;
	unsigned* density;

	unsigned highest_encoded_agents; 
	vor_region_info* info_agents;

	std::vector<point_struct> m_centroid_agents; 
};


#endif	