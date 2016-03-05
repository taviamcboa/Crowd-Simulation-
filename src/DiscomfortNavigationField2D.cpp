// *************************************************************************************************
//
// Horde3D
//   Next-Generation Graphics Engine
//
// Sample Application
// --------------------------------------
// Copyright (C) 2006-2011 Nicolas Schulz
//
//
// This sample source file is not covered by the EPL as the rest of the SDK
// and may be used without any restrictions. However, the EPL's disclaimer of
// warranty and liability shall be in effect for this file.
//
// *************************************************************************************************

#include "DiscomfortNavigationField2D.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Windows.h"

DiscomfortNavigationField2D::DiscomfortNavigationField2D(int m, int n) : Texture2DBase(m,n), dfmax(0.0), dfmin(0.0)
{
	int pixel_num = m * n; 
	this->m_discomfortNavigationField2D.resize(pixel_num, 0.0f); 
}

void DiscomfortNavigationField2D::update(ObstaclePtrList& o_list)
{
	//loop each obstacle and get its cells

	clearAll(); 

	dfmax = dfmin = 0.0f; 

	int numofobstacles = o_list.size(); 
	for(int i = 0; i<numofobstacles ; i++){
		this->convert(o_list[i]);
	}


}


void DiscomfortNavigationField2D::Normalize()
{
	if(Normalized == true){
		return; 
	}
	for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
		for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

			float pp = this->readDiscomfortNavigationField2D(cellx, cellz); 

			if(pp != POSINFINITE){
				pp /= dfmax; 
				this->write(cellx, cellz, pp); 
			}

			printf("cellx: %d, cellz: %d, discomfort: %f\n", cellx, cellz, pp);
		}
	}
	Normalized = true;
}

void DiscomfortNavigationField2D::DeNormalize()
{
	if(Normalized == false){

		return; 
	}

	//char debugstring[256];
	if(Normalized == true){
	for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
		for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

				float pp = this->readDiscomfortNavigationField2D(cellx, cellz); 

				if(pp != POSINFINITE){
					pp *= dfmax; 
					this->write(cellx, cellz, pp); 
				}
				printf("cellx: %d, cellz: %d, discomfort: %f\n", cellx, cellz, pp);
				//sprintf( debugstring, "cellx: %d, cellz: %d, density: %f\n", cellx, cellz, pp );
				//OutputDebugString(debugstring);
			}
		}
	}

	Normalized = false; 
}

void DiscomfortNavigationField2D::convert(const ObstaclePtr obstacle)
{
		float xmax = obstacle->xmax; 
		float zmax = obstacle->zmax; 
		float xmin = obstacle->xmin; 
		float zmin = obstacle->zmin; 
		float aabbcpx = obstacle->posx; 
		float aabbcpz = obstacle->posz; 

		int cxmax, czmax, cxmin, czmin; 
		int aabbccellx, aabbccellz; 

		float currentcpx, currentcpz; //looped cell
		
		UtilHelper::PositionToCell(xmax, zmax, cxmax, czmax); 
		UtilHelper::PositionToCell(xmin, zmin, cxmin, czmin); 
		UtilHelper::PositionToCell(aabbcpx, aabbcpz, aabbccellx, aabbccellz); 


		//more loop 
		float pos[2]; 
		float distance; 
		for(int cx = cxmin ; cx <= cxmax ; cx++){
			for(int cz = czmin ; cz<= czmax ; cz++){
				//if(cx == cxmin-1 && cz == czmin-1){
				//	continue;
				//}
				//if(cx == cxmin-1 && cz == czmax +1){
				//	continue;
				//}
				//if(cx == cxmax +1 && cz == czmin-1){
				//	continue;
				//}
				//if(cx == cxmax +1 && cz == czmax +1){
				//	continue; 
				//}

			/*	if(cx >= cxmax +1 || cz>=czmax +1 || cx <= cxmax -1 || cz<=czmax -1 )
				{
					DiscomfortPixel dp = this->readDiscomfortPixel(cx, cz); 
					dp.discomfort =1*POSINFINITE; 
					this->write(cx, cz, dp);
				}
				else
				{*/
				float dp = this->readDiscomfortNavigationField2D(cx, cz); 
					dp += 0.99; 
					if(dp >= 0.99 )
						dp = POSINFINITE;
					this->write(cx, cz, dp);
				//}
			}
		}

		//outer loop




		////current cellx, cellz and to the center of the model
		//float intersectionpx, intersectionpz; 
		//float distance;
		//float dtx, dtz; 
		//float ratio;
		//char debugstring[256];


		//for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
		//	for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++)
		//	{
		//		if(cellx>=cxmin && cellx <=cxmax && cellz>=czmin && cellz<=czmax)
		//		{
		//			DiscomfortPixel dp = this->readDiscomfortPixel(cellx, cellz); 
		//			dp.discomfort += 1000.0f; 
		//			UtilHelper::clampless(dp.discomfort, ObstaclediscomfortMax); 
		//			this->write(cellx, cellz, dp);
		//		}
		//		else
		//		{

		//			UtilHelper::CellToPosition(cellx, cellz, currentcpx, currentcpz); 

		//			UtilHelper::LineAABBIntersection(aabbcpx, aabbcpz,  currentcpx, currentcpz, 
		//				xmax,zmax,xmin, zmin,  intersectionpx, intersectionpz); 

		//			//decide the distance
		//			dtx = intersectionpx - currentcpx;  //currentcellcenter and the bounding box
		//			dtz = intersectionpz - currentcpz; 

		//			distance = sqrtf( dtx * dtx + dtz * dtz );

		//			//sprintf( debugstring, "cellx: %d, cellz: %d, intersectionpx: %f, intersectionpz: %f, distance: %f\n", cellx, cellz, intersectionpx, intersectionpz, distance);
		//	        //OutputDebugString(debugstring);

		//			//clamp to zero when far enough
		//			ratio = distance/discomfortZeroThreshold; 
		//			if(ratio >=1)
		//			{
		//				DiscomfortPixel dp = this->readDiscomfortPixel(cellx, cellz); 
		//				dp.discomfort += 0.0f; 
		//				UtilHelper::clampless(dp.discomfort, ObstaclediscomfortMax); 
		//				this->write(cellx, cellz, dp);
		//			}
		//			else{
		//				DiscomfortPixel dp = this->readDiscomfortPixel(cellx, cellz); 
		//				dp.discomfort += 1000*(1 - ratio); 
		//				UtilHelper::clampless(dp.discomfort, ObstaclediscomfortMax); 
		//				this->write(cellx, cellz, dp);
		//			}
		//		}
		//	}
		//}

		//all the other cell should be looped and 
}


void DiscomfortNavigationField2D::clearAll(){
	Texture2DBase::clearAll(); 
	this->m_discomfortNavigationField2D.resize(Texture2DBase::m_grid*Texture2DBase::n_grid, 0.0f);
}

float DiscomfortNavigationField2D::readDiscomfortNavigationField2D(int px, int py){

	return m_discomfortNavigationField2D.at(mapping(px, py));

}

void DiscomfortNavigationField2D::setAll(float value){

	int pixel_num = Texture2DBase::getGridX() * Texture2DBase::getGridY() ; 
	this->m_discomfortNavigationField2D.resize(pixel_num, value);

}
void DiscomfortNavigationField2D::write(int px, int py,  float value)
{
	int pixel_num = mapping(px, py); 
	this->m_discomfortNavigationField2D.at(pixel_num) = value; 
}