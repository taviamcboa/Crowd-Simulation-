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

#include "BuildingGeometry.h"

#define POSITION_LOCATION    0
#define BUILDING_ID_LOCATION 1

BuildingGeometry::BuildingGeometry(int _width, int _height)
{
	m_width = _width; 
	m_height = _height; 
	m_initialized = false; 

	
    m_VAO =0 ; 
    ZERO_MEM(m_Buffers);
}


BuildingGeometry::~BuildingGeometry()
{
	
    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
       
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
    }
}



bool BuildingGeometry::Initialize(std::vector<BuildingVS>& _allbuildlings)
{
    // Create the VAO
    glGenVertexArrays(1, &m_VAO);   
    glBindVertexArray(m_VAO);
    
    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	vector<Vector3f> Positions;
	vector<Vector3f> BuildingID; 

	float px, py, width, height; 
	int groupID, buildingID, functionID; 

	numberofbuildings = 0;  

	for(int i = 0; i<_allbuildlings.size(); i++){

		//quad 
		// 
		// 0, 1 ----> -1, 1 in vertex shader
		    width =_allbuildlings[i].width * 0.5;
			height = _allbuildlings[i].height * 0.5;
			px = _allbuildlings[i].x - width; 
			py = _allbuildlings[i].y - height; 
			
			groupID =_allbuildlings[i].groupID;
			functionID =_allbuildlings[i].functionID;
			buildingID =_allbuildlings[i].buildingID;

			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID,functionID)); 

			px = _allbuildlings[i].x - width; 
			py = _allbuildlings[i].y + height; 
			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID, functionID)); 

			px = _allbuildlings[i].x + width; 
			py = _allbuildlings[i].y + height; 
			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID, functionID)); 

			px = _allbuildlings[i].x + width; 
			py = _allbuildlings[i].y + height; 
			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID, functionID)); 

			px = _allbuildlings[i].x + width; 
			py = _allbuildlings[i].y - height; 
			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID, functionID)); 

			px = _allbuildlings[i].x - width; 
			py = _allbuildlings[i].y - height; 
			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID, functionID)); 

			numberofbuildings++; 
	}

	//numberofagents = _agentmesh.m_NumAgents; 


	// Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BUILDINGID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BuildingID[0]) * BuildingID.size(), &BuildingID[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BUILDING_ID_LOCATION);
    glVertexAttribPointer(BUILDING_ID_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	


	//deal with uncertainty part
	vector<Vector2f> Positions_Uncertainty;
	vector<Vector2i> BuildingID_Uncertainty; 
	for(int i = 0; i<_allbuildlings.size(); i++){
		//quad 
		// 
		// 0, 1 ----> -1, 1 in vertex shader
		    width =_allbuildlings[i].width * 0.5;
			height = _allbuildlings[i].height * 0.5;
			px = _allbuildlings[i].x - width; 
			py = _allbuildlings[i].y - height; 
			
			groupID =_allbuildlings[i].groupID;
			functionID =_allbuildlings[i].functionID;
			buildingID =_allbuildlings[i].buildingID;

			//v0
			Positions_Uncertainty.push_back(Vector2f(px, py)); 
			BuildingID_Uncertainty.push_back(Vector2i(buildingID,functionID)); 

			px = _allbuildlings[i].x - width; 
			py = _allbuildlings[i].y + height; 
			//v1
			Positions_Uncertainty.push_back(Vector2f(px, py)); 
			BuildingID_Uncertainty.push_back(Vector2i(buildingID,functionID)); 

			px = _allbuildlings[i].x + width; 
			py = _allbuildlings[i].y + height; 
			//v2
			Positions_Uncertainty.push_back(Vector2f(px, py)); 
			BuildingID_Uncertainty.push_back(Vector2i(buildingID,functionID)); 

			px = _allbuildlings[i].x + width; 
			py = _allbuildlings[i].y - height; 
			//v3
			Positions_Uncertainty.push_back(Vector2f(px, py)); 
			BuildingID_Uncertainty.push_back(Vector2i(buildingID,functionID)); 
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB_UNCERTAINTY]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BUILDINGID_VB_UNCERTAINTY]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2i) * BuildingID_Uncertainty.size(), &BuildingID_Uncertainty[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_VAO_Uncertainty);   
    glBindVertexArray(m_VAO_Uncertainty);
	glBindVertexArray(0);	

	m_initialized = true; 
    return GLCheckError();
}


bool BuildingGeometry::Update(std::vector<BuildingVS>& _allbuildlings)
{
    glBindVertexArray(m_VAO);

	vector<Vector3f> Positions;
	vector<Vector3f> BuildingID; 

	float px, py, width, height; 
	int groupID, buildingID, functionID; 

	numberofbuildings = 0;  

	for(int i = 0; i<_allbuildlings.size(); i++){

		    width =_allbuildlings[i].width * 0.5;
			height = _allbuildlings[i].height * 0.5;
			px = _allbuildlings[i].x - width; 
			py = _allbuildlings[i].y - height; 
			
			groupID =_allbuildlings[i].groupID;
			functionID =_allbuildlings[i].functionID;
			buildingID =_allbuildlings[i].buildingID;

			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID,functionID)); 

			px = _allbuildlings[i].x - width; 
			py = _allbuildlings[i].y + height; 
			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID, functionID)); 

			px = _allbuildlings[i].x + width; 
			py = _allbuildlings[i].y + height; 
			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID, functionID)); 

			px = _allbuildlings[i].x + width; 
			py = _allbuildlings[i].y + height; 
			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID, functionID)); 

			px = _allbuildlings[i].x + width; 
			py = _allbuildlings[i].y - height; 
			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID, functionID)); 

			px = _allbuildlings[i].x - width; 
			py = _allbuildlings[i].y - height; 
			Positions.push_back(Vector3f(px, py, 0)); 
			BuildingID.push_back(Vector3f(groupID, buildingID, functionID)); 

			numberofbuildings++; 
	}


	//numberofagents = _agentmesh.m_NumAgents; 


	// Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BUILDINGID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BuildingID[0]) * BuildingID.size(), &BuildingID[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BUILDING_ID_LOCATION);
    glVertexAttribPointer(BUILDING_ID_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	 


	//deal with uncertainty part
	vector<Vector2f> Positions_Uncertainty;
	vector<Vector2i> BuildingID_Uncertainty; 
	for(int i = 0; i<_allbuildlings.size(); i++){
		//quad 
		// 
		// 0, 1 ----> -1, 1 in vertex shader
		    width =_allbuildlings[i].width * 0.5;
			height = _allbuildlings[i].height * 0.5;
			px = _allbuildlings[i].x - width; 
			py = _allbuildlings[i].y - height; 
			
			groupID =_allbuildlings[i].groupID;
			functionID =_allbuildlings[i].functionID;
			buildingID =_allbuildlings[i].buildingID;

			//v0
			Positions_Uncertainty.push_back(Vector2f(px, py)); 
			BuildingID_Uncertainty.push_back(Vector2i(buildingID,functionID)); 

			px = _allbuildlings[i].x - width; 
			py = _allbuildlings[i].y + height; 
			//v1
			Positions_Uncertainty.push_back(Vector2f(px, py)); 
			BuildingID_Uncertainty.push_back(Vector2i(buildingID,functionID)); 

			px = _allbuildlings[i].x + width; 
			py = _allbuildlings[i].y + height; 
			//v2
			Positions_Uncertainty.push_back(Vector2f(px, py)); 
			BuildingID_Uncertainty.push_back(Vector2i(buildingID,functionID)); 

			px = _allbuildlings[i].x + width; 
			py = _allbuildlings[i].y - height; 
			//v3
			Positions_Uncertainty.push_back(Vector2f(px, py)); 
			BuildingID_Uncertainty.push_back(Vector2i(buildingID,functionID)); 
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB_UNCERTAINTY]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BUILDINGID_VB_UNCERTAINTY]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2i) * BuildingID_Uncertainty.size(), &BuildingID_Uncertainty[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    return GLCheckError();
}


void BuildingGeometry::RenderSprites()
{
    glBindVertexArray(m_VAO);
    
    uint Topology = GL_TRIANGLES;
    

	glDrawArrays(Topology, 0, numberofbuildings*6);
  
    // Make sure the VAO is not changed from the outside    
    glBindVertexArray(0);
}

void BuildingGeometry::RenderUncertainty()
{
    glBindVertexArray(m_VAO_Uncertainty);
    
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[VB_TYPES::POS_VB_UNCERTAINTY]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_Buffers[VB_TYPES::BUILDINGID_VB_UNCERTAINTY]);

	//glDrawArraysInstanced(GL_POINTS, 0, 10, 1);
	glDrawArraysInstancedBaseInstance(GL_POINTS, 0, numberofbuildings, 1, 0);
    glBindVertexArray(0);
}
