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

#ifndef MOTIONCOMPUTINGGEOMETRY_H
#define	MOTIONCOMPUTINGGEOMETRY_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"

#include "agent_mesh.h"
#include "AgentManager.h"

using namespace std;

class MotionComputingGeometry
{
public:
    MotionComputingGeometry(int _width, int _height);

    ~MotionComputingGeometry();

    bool Initialize(std::vector<std::vector<agent_struct>> &_agents);

	bool Update(std::vector<std::vector<agent_struct>> &_agents); //geometry update

    void Render();

	void RenderAgentSprites();

	void SplatDensityVelocity(); 
    
private:

	int m_width; 
	int m_height; 
 
	enum VB_TYPES {
		POS_VB,
		TEXCOORD_VB,
		GOALPREVIOUS_VB, 
		NAVIGATION_VB, 
		WEIGHTSCOMBO_VB,
		AGENTID_VB,
		NAVISPEEDRAD_VB, 
		NUM_VBs            
	};

	enum VAO_PASS {
		MOTION_PASS, 
		DRAW_PASS,
		SPLAT_PASS, 
		NUM_VAOS
	};

	int numberofagents; 

    GLuint m_VAO[NUM_VAOS]; // 
    GLuint m_Buffers[NUM_VBs]; //


};


#endif	/* MESH_H */

