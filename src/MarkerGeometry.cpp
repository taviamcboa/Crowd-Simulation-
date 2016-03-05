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

#include "MarkerGeometry.h"

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define MARKER_ID_LOCATION   2

MarkerGeometry::MarkerGeometry(int _width, int _height)
{
	m_width = _width; 
	m_height = _height; 

    m_VAO = 0;
    ZERO_MEM(m_Buffers);
}


MarkerGeometry::~MarkerGeometry()
{
	
    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
       
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}



bool MarkerGeometry::Initialize(MarkerMesh &_markermesh, bool _round)
{
    // Create the VAO
    glGenVertexArrays(1, &m_VAO);   
    glBindVertexArray(m_VAO);
    
    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	vector<Vector3f> Positions;
    vector<Vector2f> TexCoords;
	vector<float> MarkerIDs; 

	float vx, vy; //in normalized space. 
	 float marker_id; 

	for(int i = 0; i<_markermesh.m_centroid_markers.size(); i++){
		vx = _markermesh.m_centroid_markers[i].x; 
		vy = _markermesh.m_centroid_markers[i].y; 
		marker_id = i; 

		if(_round){
			vx = round(vx); 
			vy = round(vy); 
		}

		Positions.push_back(Vector3f(vx/m_width, 
			vy/m_height, 0)); 
		TexCoords.push_back(Vector2f(vx/m_width, 
			vy/m_height)); 
		MarkerIDs.push_back(marker_id);
	}

	numberofmarkers = Positions.size(); 

	// Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[MARKERID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MarkerIDs[0]) * MarkerIDs.size(), &MarkerIDs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(MARKER_ID_LOCATION);
    glVertexAttribPointer(MARKER_ID_LOCATION, 1, GL_FLOAT, GL_FALSE, 0, 0);

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	

    return GLCheckError();
}


void MarkerGeometry::Render()
{
    glBindVertexArray(m_VAO);
    
    uint Topology = GL_POINTS;
    
	glDrawArrays(Topology, 0, numberofmarkers);
  
    // Make sure the VAO is not changed from the outside    
    glBindVertexArray(0);
}
