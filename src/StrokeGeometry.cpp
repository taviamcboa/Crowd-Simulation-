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

#include "StrokeGeometry.h"

#define POSITION_LOCATION    0

StrokeGeometry::StrokeGeometry(int _width, int _height)
{
	m_width = _width; 
	m_height = _height; 
	m_initialized = false; 
	
    m_VAO =0 ; 
    ZERO_MEM(m_Buffers);
}


StrokeGeometry::~StrokeGeometry()
{
	
    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
       
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
    }
}



bool StrokeGeometry::Initialize(std::vector<StrokeVS>& _allstrokes)
{
    // Create the VAO
    glGenVertexArrays(1, &m_VAO);   
    glBindVertexArray(m_VAO);
    
    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	vector<Vector4f> Positions;

	float px, py; 
	int groupID; 

	numberofstrokes = 0;  

	for(int i = 0; i<_allstrokes.size(); i++)
	{
		px = _allstrokes[i].x; 
		py = _allstrokes[i].y; 
		groupID =_allstrokes[i].groupID;

		Positions.push_back(Vector4f(px, py, 0, groupID)); 
		numberofstrokes++; 
	}

	// Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);    

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	

	m_initialized= true; 

    return GLCheckError();
}


bool StrokeGeometry::Update(std::vector<StrokeVS>& _allstrokes)
{
    glBindVertexArray(m_VAO);

	vector<Vector4f> Positions;

	float px, py; 
	int groupID; 

	numberofstrokes = 0;  

	for(int i = 0; i<_allstrokes.size(); i++)
	{
		px = _allstrokes[i].x; 
		py = _allstrokes[i].y; 
		groupID =_allstrokes[i].groupID;

		Positions.push_back(Vector4f(px, py, 0, groupID)); 
		numberofstrokes++; 
	}

	// Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);  

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	  

    return GLCheckError();
}


void StrokeGeometry::RenderSprites()
{
    glBindVertexArray(m_VAO);
    
    uint Topology = GL_POINTS;
    
	glDrawArrays(Topology, 0, numberofstrokes);
  
    // Make sure the VAO is not changed from the outside    
    glBindVertexArray(0);
}
