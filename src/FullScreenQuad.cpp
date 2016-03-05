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

#include "FullScreenQuad.h"

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1

FullScreenQuad::FullScreenQuad()
{
    m_VAO = 0;
    ZERO_MEM(m_Buffers);
}


FullScreenQuad::~FullScreenQuad()
{
	 if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
       
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}



bool FullScreenQuad::Initialize()
{
    // Create the VAO
    glGenVertexArrays(1, &m_VAO);   
    glBindVertexArray(m_VAO);
    
    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	vector<Vector3f> Positions;
    vector<Vector2f> TexCoords;
    vector<uint> Indices;

	Positions.resize(4);
    TexCoords.resize(4);
    Indices.resize(6);

	Positions[0] = Vector3f(-1.0f, -1.0f, 0.0f);
	TexCoords[0] = Vector2f(0.0f, 0.0f);

	Positions[1] = Vector3f(-1.0f, 1.0f, 0.0f);
	TexCoords[1] = Vector2f(0.0f, 1.0f);

	Positions[2] = Vector3f(1.0f, 1.0f, 0.0f);
	TexCoords[2] = Vector2f(1.0f, 1.0f);

	Positions[3] = Vector3f(1.0f, -1.0f, 0.0f);
	TexCoords[3] = Vector2f(1.0f, 0.0f);

	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;
	
	Indices[3] = 2;
	Indices[4] = 3;
	Indices[5] = 0;

	// Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
  
    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	

    return GLCheckError();
}


void FullScreenQuad::Render()
{
    glBindVertexArray(m_VAO);
    
    uint Topology = GL_TRIANGLES;
    
	glDrawElementsBaseVertex(Topology, 6, GL_UNSIGNED_INT, 0, 0);
  
    // Make sure the VAO is not changed from the outside    
    glBindVertexArray(0);
}
