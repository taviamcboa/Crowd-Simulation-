#include "BuildingManager.h"

BuildingManager::BuildingManager(int _width, int _height)
{
	this->m_width = _width; 
	this->m_height = _height; 
	//this->m_buildingcollection.resize(0); 
}

BuildingManager::~BuildingManager()
{
	this->m_buildingcollection.clear(); 
}

bool BuildingManager::LoadfromFile(const char* filepath)
{
	return true; 

}
//void BuildingManager::AddBuilding(int pixelx, int pixely, float radmin, float radmax, int groupnum, int functionID)
//{
//	//
//	Vector2f posFullScreen; 
//	float rad; 
//	int groupID; 
//
//	int existedbuildings = 0; 
//	existedbuildings = this->m_buildingcollection.size();
//
//
//	posFullScreen = Vector2f(float(pixelx)/this->m_width, float(pixely)/this->m_height); 
//
//	rad = radmin + RandomFloat()*(radmax - radmin); 
//	rad = rad/this->m_width; 
//
//	groupID = 0 + RandomFloat()*(groupnum - 0); 
//	if(groupID == groupnum)
//		groupID--; 
//
//	functionID = groupID; 
//
//	building_struct temp(posFullScreen.x, posFullScreen.y, rad, groupID, existedbuildings, functionID, true); 
//
//	this->m_buildingcollection.push_back(temp); 
//}

void BuildingManager::AddBuilding(int pixelx, int pixely, float width, float height, int groupnum, int functionID)
{
	//
	Vector2f posFullScreen; 
	float rad; 
	int groupID; 

	int existedbuildings = 0; 
	existedbuildings = this->m_buildingcollection.size();


	posFullScreen = Vector2f(float(pixelx)/this->m_width, float(pixely)/this->m_height); 

	//offset the position to the center of the cell
	int cellx, cellz; 
	float cellpx, cellpz; 

	UtilHelper::PositionToCell(pixelx, pixely, cellx, cellz); 
	UtilHelper::CellToPosition(cellx, cellz, cellpx, cellpz); 

    posFullScreen = Vector2f(float(cellpx)/this->m_width, float(cellpz)/this->m_height); 


	groupID = 0 + RandomFloat()*(groupnum - 0); 
	if(groupID == groupnum)
		groupID--; 

	functionID = groupID; 

	building_struct temp(posFullScreen.x, posFullScreen.y, width, height, groupID, existedbuildings, functionID, true); 

	this->m_buildingcollection.push_back(temp); 
}


void BuildingManager::ClinchBuildings(std::vector<BuildingVS>& _allbuildlings)
{
	_allbuildlings.clear(); 

    Vector2f buildingpos; 
	int groupID; 
	int buildingID; 
	int functionID; 
	//float radius; 
	float width; 
	float height; 

	for (int i = 0; i < m_buildingcollection.size(); ++i) {

			buildingpos.x = m_buildingcollection[i].px; 
			buildingpos.y = m_buildingcollection[i].py; 

			groupID = m_buildingcollection[i].groupid; 
			buildingID = m_buildingcollection[i].buildingid; 
			functionID = m_buildingcollection[i].functionid; 
			width = m_buildingcollection[i].width;
			height = m_buildingcollection[i].height; 

			if(m_buildingcollection[i].active == 0){
				buildingpos.x = buildingpos.y = -1; 
			}

		_allbuildlings.push_back(BuildingVS(buildingpos.x, buildingpos.y, groupID, buildingID, width, height, functionID));
	}
}

