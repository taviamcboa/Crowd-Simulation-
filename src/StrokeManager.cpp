#include "StrokeManager.h"

StrokeManager::StrokeManager(int _width, int _height)
{
	this->m_width = _width; 
	this->m_height = _height; 
	//this->m_buildingcollection.resize(0); 
}

StrokeManager::~StrokeManager()
{
	this->m_strokecollection.clear(); 
}

bool StrokeManager::LoadfromFile(const char* filepath)
{
	return true; 

}
void StrokeManager::AddStroke(int pixelx, int pixely, int groupID)
{
	//
	Vector2f posFullScreen; 
	posFullScreen = Vector2f(float(pixelx)/this->m_width, float(pixely)/this->m_height); 

	stroke_struct temp(posFullScreen.x, posFullScreen.y, groupID); 

	this->m_strokecollection.push_back(temp); 
}

void StrokeManager::ClinchStrokes(std::vector<StrokeVS>& _allstrokes)
{
	_allstrokes.clear(); 

    Vector2f strokepos; 
	int groupID; 
	int buildingID; 

	for (int i = 0; i < m_strokecollection.size(); ++i) {

		strokepos.x = m_strokecollection[i].px; 
		strokepos.y = m_strokecollection[i].py; 
		groupID = m_strokecollection[i].groupid; 

		_allstrokes.push_back(StrokeVS(strokepos.x, strokepos.y, groupID));
	}
}