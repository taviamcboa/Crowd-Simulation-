#include <assert.h>
#include "NavigationBuildingBufferGeometry.h"

#define POSITION_LOCATION    0


NavigationBuildingBufferGeometry::NavigationBuildingBufferGeometry(int _width, int _height)
{
	m_width = _width; 
	m_height = _height; 

    m_VAO = 0;
    ZERO_MEM(m_Buffers);
}

NavigationBuildingBufferGeometry::~NavigationBuildingBufferGeometry()
{
	
    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
       
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

bool NavigationBuildingBufferGeometry::Initialize(float xstart, float ystart, bool xaligned, int width, int height, int g_padding, int num)
{
	vector<Vector2f> Positions;

	int cellx, celly; 
	float xstart_gc, ystart_gc; 
	UtilHelper::PositionToCell(xstart * 1600, ystart * 900, cellx, celly); 
	UtilHelper::CellToPosition(cellx, celly, xstart_gc, ystart_gc); 

	xstart_gc /= 1600;
	ystart_gc /= 900;

	float xspan = 1/float(UtilHelper::GridDimensionX);
	float yspan = 1/float(UtilHelper::GridDimensionZ);

	float vx, vy; //in normalized space

	float vxleft, vyleft, vxright, vyright; 

	vxleft = xstart_gc; 
	vyleft = ystart_gc;

	for(int i = 0; i<num; i++)
	{
		if(xaligned == true)
		{
			vx = xstart_gc + i * g_padding * xspan; 
			vy = ystart_gc; 
		}
		else
		{
			vy = ystart_gc + i * g_padding * yspan; 
			vx = xstart_gc; 
		}
		Positions.push_back(Vector2f(vx, vy)); // 0
		m_center.push_back(Vector2f(vx, vy)); 
	}

	vxright = vx; 
	vyright = vy;
	

	//four corners
	Vector2f vertex_corner; 
	if(xaligned == true)
	{
		//TWO TRIANGLES
		vertex_corner.x = vxleft - 0.5 * xspan;
		vertex_corner.y = vyleft - 0.5 * height * yspan - 3 * yspan;
		m_boundingboxCornor.push_back(vertex_corner);

		vertex_corner.x = vxleft - 0.5 * xspan;
		vertex_corner.y = vyleft + 0.5 * height * yspan + 3* yspan;
		m_boundingboxCornor.push_back(vertex_corner);

		vertex_corner.x = vxright + 0.5 * xspan;
		vertex_corner.y = vyright + 0.5 * height * yspan + 3* yspan;
		m_boundingboxCornor.push_back(vertex_corner);

		vertex_corner.x = vxright + 0.5 * xspan;
		vertex_corner.y = vyright + 0.5 * height * yspan + 3* yspan;
		m_boundingboxCornor.push_back(vertex_corner);

		vertex_corner.x = vxright + 0.5 * xspan;
		vertex_corner.y = vyright - 0.5 * height * xspan - 3 * yspan;
		m_boundingboxCornor.push_back(vertex_corner);

		vertex_corner.x = vxleft - 0.5 * xspan;
		vertex_corner.y = vyleft - 0.5 * height * xspan -3 * yspan;
		m_boundingboxCornor.push_back(vertex_corner);
	}
	else
	{
		vertex_corner.y = vyleft - 0.5 * yspan;
		vertex_corner.x = vxleft - 0.5 * height * xspan - 1 * xspan;
		m_boundingboxCornor.push_back(vertex_corner);

		vertex_corner.y = vyright + 0.5 * yspan;
		vertex_corner.x = vxright - 0.5 * height * xspan - 1 * xspan;
		m_boundingboxCornor.push_back(vertex_corner);

		vertex_corner.y = vyright + 0.5 * yspan;
		vertex_corner.x = vxright + 0.5 * height * xspan +1 * xspan;
		m_boundingboxCornor.push_back(vertex_corner);

		vertex_corner.y = vyright + 0.5 * yspan;
		vertex_corner.x = vxright + 0.5 * height * xspan +1 * xspan;
		m_boundingboxCornor.push_back(vertex_corner);

		vertex_corner.y = vyleft - 0.5 * yspan;
		vertex_corner.x = vxleft + 0.5 * height * xspan +1 * xspan;
		m_boundingboxCornor.push_back(vertex_corner);

		vertex_corner.y = vyleft - 0.5 * yspan;
		vertex_corner.x = vxleft - 0.5 * height * xspan - 1 * xspan;
		m_boundingboxCornor.push_back(vertex_corner);

	}

	numberofnaviBuilding = Positions.size(); 

    glGenBuffers(NUM_VBs, m_Buffers);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::BOUNDING_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * 6, &m_boundingboxCornor[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glBindVertexArray(0);

    return GLCheckError();
}


void NavigationBuildingBufferGeometry::RenderNavigationBuildingBuffer()
{
    glBindVertexArray(m_VAO);
    
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[VB_TYPES::POS_VB]);

	//glDrawArraysInstanced(GL_POINTS, 0, 10, 1);
	glDrawArraysInstancedBaseInstance(GL_POINTS, 0, numberofnaviBuilding, 1, 0);
    glBindVertexArray(0);


	/*glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[VB_TYPES::POS_VB]);
	float *ptr = (float *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * 2 * 600, GL_MAP_READ_BIT);
		float test[2]; 
		for(int i = 0; i<600; i++)
		{
			test[0] = ptr[2 * i +0]; 
			test[1] = ptr[2 * i +1]; 
			
			printf("%d: %f, %f\n", i, test[0], test[1]); 
		}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);*/
}

void NavigationBuildingBufferGeometry::RenderBoundingBoxBuffer()
{
	glBindVertexArray(m_VAO);
    
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[VB_TYPES::BOUNDING_VB]);

	//glDrawArraysInstanced(GL_POINTS, 0, 10, 1);
	glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
    glBindVertexArray(0);

}
