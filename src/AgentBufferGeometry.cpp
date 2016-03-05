#include <assert.h>
#include "AgentBufferGeometry.h"

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define MARKER_ID_LOCATION   2

AgentBufferGeometry::AgentBufferGeometry(int _width, int _height)
{
	m_width = _width; 
	m_height = _height; 

    m_VAO = 0;
    ZERO_MEM(m_Buffers);
}

AgentBufferGeometry::~AgentBufferGeometry()
{
	
    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
       
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

bool AgentBufferGeometry::Initialize(AgentManager &_agentManag)
{
	//per-agent
	vector<Vector2f> Positions;
	vector<Vector2f> Directions; 
	vector<float> Speeds; 
	vector<float> MaxSpeeds; 
	vector<Vector2i> GroupIDAgentIDs; 
	vector<float> Radius; 
	vector<float> Turnrates; 
	vector<Vector4f> ParkPosAlive; 
	vector<Vector2f> OriginalPos; 
	vector<Vector4f> DeltaPos; 
	vector<Vector2i> UncertaintyStatus; 
	vector<Vector4i> VisitHistory; 
	
	


	float vx, vy; //in normalized space. 
	float dirx, diry; 
	float speed; 
	float maxspeed; 
	int aID; 
	int gID; 
	float radius; 
	float turnrates; 
	float parkposx, parkposy; 
	float alive; 
	float o_vx, o_vy; 
	float deltax, deltay; 
	int statusID, counter; 
	

	for (int i = 0; i < _agentManag.m_agentcollection.size(); ++i) {
		for(int j = 0 ; j < _agentManag.m_agentcollection[i].size(); j++){

			//binding: 0
			vx = _agentManag.m_agentcollection[i][j].px; 
			vy = _agentManag.m_agentcollection[i][j].py; 

			//binding: 1
			dirx = _agentManag.m_agentcollection[i][j].dirx; 
			diry = _agentManag.m_agentcollection[i][j].diry; 

			//binding: 2
			speed = _agentManag.m_agentcollection[i][j].scurrent;
			//binding: 3
			maxspeed = _agentManag.m_agentcollection[i][j].smax; 

			//binding: 5
			radius = 0.00390625f; 
			//binding: 6
			turnrates = 0.15f; 

			//binding: 4
			aID = _agentManag.m_agentcollection[i][j].agentid; 
			gID = _agentManag.m_agentcollection[i][j].groupid; 

			//binding: 8
			o_vx = _agentManag.m_agentcollection[i][j].initialpx; 
			o_vy = _agentManag.m_agentcollection[i][j].initialpy;

			//binding: 7
			parkposx = _agentManag.m_agentcollection[i][j].parkx; 
			parkposy = _agentManag.m_agentcollection[i][j].parky; 
			alive = 1.0f; 

			deltax = 0.0f; 
			deltay = 0.0f; 

			statusID = 1; //READY
			counter = 0; 


			Positions.push_back(Vector2f(vx, vy)); // 0
			Directions.push_back(Vector2f(dirx, diry)); //1
			Speeds.push_back(speed); //2 
			Radius.push_back(radius); //5
			MaxSpeeds.push_back(maxspeed);//3
			GroupIDAgentIDs.push_back(Vector2i(gID, aID));  //4
			Turnrates.push_back(turnrates); //6
			ParkPosAlive.push_back(Vector4f(parkposx, parkposy, alive, 0));//7
			OriginalPos.push_back(Vector2f(o_vx, o_vy)); //8
			DeltaPos.push_back(Vector4f(deltax, deltay, 1, 1));//9
			UncertaintyStatus.push_back(Vector2i(statusID, counter));//10 
			VisitHistory.push_back(Vector4i(-1, -1, -1, -1)); 
		}
	}

	numberofagents = Positions.size(); 

    glGenBuffers(NUM_VBs, m_Buffers);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::DIRECTION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Directions[0]) * Directions.size(), &Directions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::SPEED_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Speeds[0]) * Speeds.size(), &Speeds[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::MAXSPEED_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MaxSpeeds[0]) * MaxSpeeds.size(), &MaxSpeeds[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::GID_AGENTID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GroupIDAgentIDs[0]) * GroupIDAgentIDs.size(), &GroupIDAgentIDs[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::RAD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Radius[0]) * Radius.size(), &Radius[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::TURNRATE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Turnrates[0]) * Turnrates.size(), &Turnrates[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::PARKPOSALIVE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParkPosAlive[0]) * ParkPosAlive.size(), &ParkPosAlive[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::ORIGINALPOS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(OriginalPos[0]) * OriginalPos.size(), &OriginalPos[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::DELTAPOS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DeltaPos[0]) * DeltaPos.size(), &DeltaPos[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::DEBUGPURPOSE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DeltaPos[0]) * DeltaPos.size(), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::UNCERTAINTYSTATUS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UncertaintyStatus[0]) * UncertaintyStatus.size(), &UncertaintyStatus[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::DEBUGGUID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4f) * UncertaintyStatus.size(), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::VISITINGHIS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4i) * VisitHistory.size(), &VisitHistory[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::BUILDINGPOS_VB]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f), NULL, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glBindVertexArray(0);

    return GLCheckError();
}
bool AgentBufferGeometry::UpdateBuildingInfo(BuildingManager &_buildingManag)
{
	//per-building
	vector<Vector2f> BuildingPos; 
	float b_vx, b_vy; 
	for(int bid = 0; bid < _buildingManag.NumBuildings(); bid++)
	{
		b_vx = _buildingManag.m_buildingcollection[bid].px; 
		b_vy = _buildingManag.m_buildingcollection[bid].py; 
		BuildingPos.push_back(Vector2f(b_vx, b_vy));
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::BUILDINGPOS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * BuildingPos.size(), &BuildingPos[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return GLCheckError();
}
bool AgentBufferGeometry::UpdateRandomPropability(AgentManager &_agentManag)
{
	vector<float> probability; 
	float p; //in normalized space. 
	for (int i = 0; i < _agentManag.m_agentcollection.size(); ++i) {
		for(int j = 0 ; j < _agentManag.m_agentcollection[i].size(); j++)
		{
			    p = RandomFloat();
				probability.push_back(p); // 0
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VB_TYPES::RANDOMPROB_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * probability.size(), &probability[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return GLCheckError();
}

void AgentBufferGeometry::RenderDensityVelocitySplat()
{
    glBindVertexArray(m_VAO);
    
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[VB_TYPES::POS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_Buffers[VB_TYPES::DIRECTION_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_Buffers[VB_TYPES::SPEED_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_Buffers[VB_TYPES::GID_AGENTID_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_Buffers[VB_TYPES::RAD_VB]);

	//glDrawArraysInstanced(GL_POINTS, 0, 10, 1);
	glDrawArraysInstancedBaseInstance(GL_POINTS, 0, numberofagents, 1, 0);
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
void AgentBufferGeometry::RenderFutureDiscomfortSplat()
{
    glBindVertexArray(m_VAO);
    
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[VB_TYPES::POS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_Buffers[VB_TYPES::DIRECTION_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_Buffers[VB_TYPES::SPEED_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_Buffers[VB_TYPES::GID_AGENTID_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_Buffers[VB_TYPES::RAD_VB]);

	//glDrawArraysInstanced(GL_POINTS, 0, 10, 1);
	glDrawArraysInstancedBaseInstance(GL_POINTS, 0, numberofagents, 1, 0);
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
void AgentBufferGeometry::ComputeAdvectionBindingBuffer()
{
    glBindVertexArray(m_VAO);
    
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[VB_TYPES::POS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_Buffers[VB_TYPES::DIRECTION_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_Buffers[VB_TYPES::SPEED_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_Buffers[VB_TYPES::MAXSPEED_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_Buffers[VB_TYPES::GID_AGENTID_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_Buffers[VB_TYPES::RAD_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_Buffers[VB_TYPES::TURNRATE_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_Buffers[VB_TYPES::PARKPOSALIVE_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, m_Buffers[VB_TYPES::ORIGINALPOS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, m_Buffers[VB_TYPES::DELTAPOS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, m_Buffers[VB_TYPES::UNCERTAINTYSTATUS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, m_Buffers[VB_TYPES::DEBUGGUID_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, m_Buffers[VB_TYPES::BUILDINGPOS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, m_Buffers[VB_TYPES::VISITINGHIS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 14, m_Buffers[VB_TYPES::RANDOMPROB_VB]);
}
void AgentBufferGeometry::ComputeAdvectionUnBindingBuffer()
{

    glBindVertexArray(0);
}
void AgentBufferGeometry::CollisionDetectionBindingBuffer()
{
	glBindVertexArray(m_VAO);
    
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[VB_TYPES::POS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_Buffers[VB_TYPES::DIRECTION_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_Buffers[VB_TYPES::SPEED_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_Buffers[VB_TYPES::GID_AGENTID_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_Buffers[VB_TYPES::RAD_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_Buffers[VB_TYPES::PARKPOSALIVE_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_Buffers[VB_TYPES::DELTAPOS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_Buffers[VB_TYPES::DEBUGPURPOSE_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, m_Buffers[VB_TYPES::TURNRATE_VB]);

}
void AgentBufferGeometry::CollisionDetectionUnBindingBuffer()
{

	glBindVertexArray(0);
}
void AgentBufferGeometry::DrawAgentsBuffer()
{
    glBindVertexArray(m_VAO);
    
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_Buffers[VB_TYPES::POS_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_Buffers[VB_TYPES::GID_AGENTID_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_Buffers[VB_TYPES::RAD_VB]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_Buffers[VB_TYPES::UNCERTAINTYSTATUS_VB]);
	glDrawArraysInstancedBaseInstance(GL_POINTS, 0, numberofagents, 1, 0);
    glBindVertexArray(0);
}


