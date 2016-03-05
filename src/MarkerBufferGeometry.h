#ifndef MARKERBUFFERGEOMETRY_H
#define	MARKERBUFFERGEOMETRY_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"

#include "marker_mesh.h"

using namespace std;


class MarkerBufferGeometry
{
public:
    MarkerBufferGeometry(int _width, int _height);

    ~MarkerBufferGeometry();

    bool Initialize(MarkerMesh &_markermesh, bool _round =true);

	//draw Agents
	void DrawMarkersBuffer(); 
	
public: 

	GLuint PositionBuffer()
	{
		return m_Buffers[VB_TYPES::POS_VB];
	}

private:

	int m_width; 
	int m_height; 
 
	enum VB_TYPES {
		POS_VB, // vec2 b:0 
		ID_VB, 
		NUM_VBs            
	};

	int numberofmarkers; 

    GLuint m_VAO;
    GLuint m_Buffers[NUM_VBs];
};


#endif	/* MESH_H */

