#include <assert.h>
#include "MarkerBufferGeometry.h"

#define POSITION_LOCATION    0


MarkerBufferGeometry::MarkerBufferGeometry(int _width, int _height)
{
	m_width = _width; 
	m_height = _height; 

    m_VAO = 0;
    ZERO_MEM(m_Buffers);
}

MarkerBufferGeometry::~MarkerBufferGeometry()
{
	
    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
       
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

bool MarkerBufferGeometry::Initialize(MarkerMesh &_markermesh, bool _round)
{
	vector<Vector2f> Positions;
	vector<int> IDs; 
	
	float vx, vy; //in normalized space. 
	int marker_id; 

	for(int i = 0; i<_markermesh.m_centroid_markers.size(); i++){
		vx = _markermesh.m_centroid_markers[i].x; 
		vy = _markermesh.m_centroid_markers[i].y; 
		marker_id = i; 

		if(_round){
			vx = round(vx); 
			vy = round(vy); 
		}

		Positions.push_back(Vector2f(vx/m_width, 
			vy/m_height)); 
		
		IDs.push_back(marker_id);
	}

	numberofmarkers = Positions.size(); 

    glGenBuffers(NUM_VBs, m_Buffers);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::ID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IDs[0]) * IDs.size(), &IDs[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glBindVertexArray(0);

    return GLCheckError();
}


void MarkerBufferGeometry::DrawMarkersBuffer()
{
    glBindVertexArray(m_VAO);
    
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[VB_TYPES::POS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_Buffers[VB_TYPES::ID_VB]);
	
	glDrawArraysInstancedBaseInstance(GL_POINTS, 0, numberofmarkers, 1, 0);
    glBindVertexArray(0);
}


