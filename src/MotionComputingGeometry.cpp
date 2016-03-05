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

#include <assert.h>

#include "MotionComputingGeometry.h"

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define GOAL_PREV_LOCATION   2
#define NAVI_ID_LOCATION   3
#define WEIGHT_RAD_LOCATION 4
#define AGENT_ID_LOCATION 5

MotionComputingGeometry::MotionComputingGeometry(int _width, int _height)
{
	m_width = _width; 
	m_height = _height; 

	
    ZERO_MEM(m_VAO);
    ZERO_MEM(m_Buffers);
}


MotionComputingGeometry::~MotionComputingGeometry()
{
	
    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
       
    if (m_VAO[0] != 0) {
        glDeleteVertexArrays(ARRAY_SIZE_IN_ELEMENTS(m_VAO), m_VAO);
        ZERO_MEM(m_VAO);
    }
}



bool MotionComputingGeometry::Initialize(std::vector<std::vector<agent_struct>> &_agents)
{
    // Create the VAO
    glGenVertexArrays(NUM_VAOS, m_VAO);   
    glBindVertexArray(m_VAO[MOTION_PASS]);
    
    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	vector<Vector3f> Positions;
    vector<Vector2f> TexCoords;
	vector<Vector4f> GoalPrev; //goal + previous direction
	vector<Vector2f> NaviGroupID;
	vector<Vector4f> WeightRad;
	vector<Vector4f> AgentID; 

	vector<Vector4f> NaviSpeedRad; 

	float px, py, gx,gy, dirx, diry; 
	float speed; 
	int agentid; 
	numberofagents = 0;  
	for(int i = 0; i<_agents.size(); i++){ //group
		for(int j = 0; j<_agents[i].size(); j++)// per group
		{
			gx = _agents[i][j].gx; 
			gy = _agents[i][j].gy; 

			dirx = _agents[i][j].dirx; 
			diry = _agents[i][j].diry; 

			agentid =_agents[i][j].agentid;

			speed = _agents[i][j].scurrent; 

			Positions.push_back(Vector3f(_agents[i][j].px, 
				_agents[i][j].py, 0)); 
			TexCoords.push_back(Vector2f(_agents[i][j].px, 
				_agents[i][j].py)); 
			GoalPrev.push_back(Vector4f(_agents[i][j].gx, _agents[i][j].gy, 
				_agents[i][j].dirx, _agents[i][j].diry)); 

			NaviGroupID.push_back(Vector2f(_agents[i][j].dirx, _agents[i][j].diry)); 

			WeightRad.push_back(Vector4f(1,0,0,0.004558f));

			AgentID.push_back(Vector4f(_agents[i][j].groupid, _agents[i][j].agentid, _agents[i][j].smax, _agents[i][j].active)); 

			NaviSpeedRad.push_back(Vector4f(dirx, diry, speed, 0.00390625f));

			numberofagents++; 
		}
	}

	//numberofagents = _agentmesh.m_NumAgents; 


	// Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[GOALPREVIOUS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GoalPrev[0]) * GoalPrev.size(), &GoalPrev[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(GOAL_PREV_LOCATION);
    glVertexAttribPointer(GOAL_PREV_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NAVIGATION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NaviGroupID[0]) * NaviGroupID.size(), &NaviGroupID[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(NAVI_ID_LOCATION);
    glVertexAttribPointer(NAVI_ID_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WEIGHTSCOMBO_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(WeightRad[0]) * WeightRad.size(), &WeightRad[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(WEIGHT_RAD_LOCATION);
    glVertexAttribPointer(WEIGHT_RAD_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[AGENTID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AgentID[0]) * AgentID.size(), &AgentID[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(AGENT_ID_LOCATION);
    glVertexAttribPointer(AGENT_ID_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);


    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	

	//
	glBindVertexArray(m_VAO[DRAW_PASS]);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);   

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[AGENTID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AgentID[0]) * AgentID.size(), &AgentID[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);	

	//
	glBindVertexArray(m_VAO[SPLAT_PASS]);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);   

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NAVISPEEDRAD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NaviSpeedRad[0]) * NaviSpeedRad.size(), &NaviSpeedRad[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
    return GLCheckError();
}


bool MotionComputingGeometry::Update(std::vector<std::vector<agent_struct>> &_agents)
{
    glBindVertexArray(m_VAO[MOTION_PASS]);
    
	vector<Vector3f> Positions;
    vector<Vector2f> TexCoords;
	vector<Vector4f> GoalPrev;
	vector<Vector2f> NaviGroupID;
	vector<Vector4f> WeightRad;
	vector<Vector4f> AgentID; 

	vector<Vector4f> NaviSpeedRad; 

	float px, py, gx,gy,dirx, diry; 
	float speed; 
	int agentid; 
	int active; 
	numberofagents = 0;  
	for(int i = 0; i<_agents.size(); i++){
		for(int j = 0; j<_agents[i].size(); j++)
		{
			gx = _agents[i][j].gx; 
			gy = _agents[i][j].gy; 

			dirx = _agents[i][j].dirx; 
			diry = _agents[i][j].diry; 

			agentid =_agents[i][j].agentid;

			speed = _agents[i][j].scurrent; 

			Positions.push_back(Vector3f(_agents[i][j].px, 
				_agents[i][j].py, 0)); 
			TexCoords.push_back(Vector2f(_agents[i][j].px, 
				_agents[i][j].py)); 
			GoalPrev.push_back(Vector4f(_agents[i][j].gx, _agents[i][j].gy, 
				_agents[i][j].dirx, _agents[i][j].diry)); 

			NaviGroupID.push_back(Vector2f(_agents[i][j].dirx, _agents[i][j].diry)); 

			WeightRad.push_back(Vector4f(1,0,0,0.0145856f));

			AgentID.push_back(Vector4f(_agents[i][j].groupid, _agents[i][j].agentid, _agents[i][j].smax, _agents[i][j].active)); 

			NaviSpeedRad.push_back(Vector4f(dirx, diry, speed, 0.00390625f));

			numberofagents++; 
		}
	}

	//numberofagents = _agentmesh.m_NumAgents; 

	// Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[GOALPREVIOUS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GoalPrev[0]) * GoalPrev.size(), &GoalPrev[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(GOAL_PREV_LOCATION);
    glVertexAttribPointer(GOAL_PREV_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NAVIGATION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NaviGroupID[0]) * NaviGroupID.size(), &NaviGroupID[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NAVI_ID_LOCATION);
    glVertexAttribPointer(NAVI_ID_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WEIGHTSCOMBO_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(WeightRad[0]) * WeightRad.size(), &WeightRad[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(WEIGHT_RAD_LOCATION);
    glVertexAttribPointer(WEIGHT_RAD_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[AGENTID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AgentID[0]) * AgentID.size(), &AgentID[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(AGENT_ID_LOCATION);
    glVertexAttribPointer(AGENT_ID_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	

	glBindVertexArray(m_VAO[DRAW_PASS]);
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);  

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[AGENTID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AgentID[0]) * AgentID.size(), &AgentID[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);

	//
	glBindVertexArray(m_VAO[SPLAT_PASS]);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);   

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NAVISPEEDRAD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NaviSpeedRad[0]) * NaviSpeedRad.size(), &NaviSpeedRad[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

    return GLCheckError();
}


void MotionComputingGeometry::Render()
{
    glBindVertexArray(m_VAO[MOTION_PASS]);
    
    uint Topology = GL_POINTS;
    
	glDrawArrays(Topology, 0, numberofagents);
  
    // Make sure the VAO is not changed from the outside    
    glBindVertexArray(0);
}

void MotionComputingGeometry::RenderAgentSprites()
{
    glBindVertexArray(m_VAO[DRAW_PASS]);
    
    uint Topology = GL_POINTS;
    
	glDrawArrays(Topology, 0, numberofagents);
  
    // Make sure the VAO is not changed from the outside    
    glBindVertexArray(0);
}

void MotionComputingGeometry::SplatDensityVelocity()
{
	glBindVertexArray(m_VAO[SPLAT_PASS]);
    
    uint Topology = GL_POINTS;
    
	glDrawArrays(Topology, 0, numberofagents);
  
    // Make sure the VAO is not changed from the outside    
    glBindVertexArray(0);

}