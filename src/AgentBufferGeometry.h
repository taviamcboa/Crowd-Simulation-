#ifndef AGENTBUFFERGEOMETRY_H
#define	AGENTBUFFERGEOMETRY_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"
#include "texture.h"

#include "AgentManager.h"
#include "BuildingManager.h" 

using namespace std;
class AgentManager;

class AgentBufferGeometry
{
public:
    AgentBufferGeometry(int _width, int _height);

    ~AgentBufferGeometry();

    bool Initialize(AgentManager &_agentManag);
	bool UpdateBuildingInfo(BuildingManager &_buildingManag);
	bool UpdateRandomPropability(AgentManager &_agentManag); 

    void RenderDensityVelocitySplat(); // 5 buffer used to splat
	void RenderFutureDiscomfortSplat(); // 5 buffer used to splat
	void ComputeAdvectionBindingBuffer(); //9 buffers used together

	void ComputeAdvectionUnBindingBuffer(); 

	void CollisionDetectionBindingBuffer(); 
	void CollisionDetectionUnBindingBuffer(); 

	//draw Agents
	void DrawAgentsBuffer(); 
	

	GLuint PositionBuffer()
	{
		return m_Buffers[VB_TYPES::POS_VB];
	}
	GLuint DirectionBuffer()
	{
		return m_Buffers[VB_TYPES::DIRECTION_VB];
	}
    GLuint SpeedBuffer()
	{
		return m_Buffers[VB_TYPES::SPEED_VB];
	}
	GLuint MaxSpeedBuffer()
	{
		return m_Buffers[VB_TYPES::MAXSPEED_VB];
	}
	GLuint GidAidBuffer()
	{
		return m_Buffers[VB_TYPES::GID_AGENTID_VB];
	}
	GLuint RadBuffer()
	{
		return m_Buffers[VB_TYPES::RAD_VB];
	}
	GLuint TurnRateBuffer()
	{
		return m_Buffers[VB_TYPES::TURNRATE_VB];
	}
	GLuint ParkPosBuffer()
	{
		return m_Buffers[VB_TYPES::PARKPOSALIVE_VB];
	}
	GLuint OrigPosBuffer()
	{
		return m_Buffers[VB_TYPES::ORIGINALPOS_VB];
	}
	GLuint DeltaPosBuffer()
	{
		return m_Buffers[VB_TYPES::DELTAPOS_VB];
	}
	GLuint DebugBuffer()
	{
		return m_Buffers[VB_TYPES::DEBUGPURPOSE_VB];
	}
	GLuint DebugGuidBuffer()
	{
		return m_Buffers[VB_TYPES::DEBUGGUID_VB];
	}
	GLuint BuildingPosBuffer()
	{
		return m_Buffers[VB_TYPES::BUILDINGPOS_VB];
	}
	GLuint VisitingHistory()
	{
		return m_Buffers[VB_TYPES::VISITINGHIS_VB];
	}
private:

	int m_width; 
	int m_height; 
 
	enum VB_TYPES {
		POS_VB, // vec2 b:0 
		DIRECTION_VB,  // vec2 b:1
		SPEED_VB,  // float b:2
		MAXSPEED_VB, //float b:3
		GID_AGENTID_VB, // ivec2 b:4
		RAD_VB, //float b:5
		TURNRATE_VB, //float b: 6
		PARKPOSALIVE_VB, //vec4, b: 7
		ORIGINALPOS_VB, //vec2 b: 8
		DELTAPOS_VB, //9
		DEBUGPURPOSE_VB,//10 
		UNCERTAINTYSTATUS_VB,//11 
		DEBUGGUID_VB, //12
		VISITINGHIS_VB, 
		BUILDINGPOS_VB,
		RANDOMPROB_VB, 
		NUM_VBs            
	};

	int numberofagents; 

    GLuint m_VAO;

    GLuint m_Buffers[NUM_VBs];
};


#endif	/* MESH_H */

