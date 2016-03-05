#ifndef NAVIGATIONBUILDINGBUFFERGEOMETRY_H
#define	NAVIGATIONBUILDINGBUFFERGEOMETRY_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"


using namespace std;


class NavigationBuildingBufferGeometry
{
public:
    NavigationBuildingBufferGeometry(int _width, int _height);

    ~NavigationBuildingBufferGeometry();

    bool Initialize(float xstart, float ystart, bool xaligned, int width, int height, int g_padding, int num);

    void RenderNavigationBuildingBuffer(); // 5 buffer used to splat

	void RenderBoundingBoxBuffer(); 
	
	GLuint PositionBuffer()
	{
		return m_Buffers[VB_TYPES::POS_VB];
	}


	
private:

	int m_width; 
	int m_height; 
 
	enum VB_TYPES {
		POS_VB, // vec2 b:0 
		BOUNDING_VB,
		NUM_VBs            
	};

	int numberofnaviBuilding; 

    GLuint m_VAO;
    GLuint m_Buffers[NUM_VBs];

public:
	std::vector<Vector2f> m_center; 
	std::vector<Vector2f> m_boundingboxCornor; 
};


#endif	/* MESH_H */

