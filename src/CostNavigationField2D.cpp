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

#include "CostNavigationField2D.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Windows.h"

CostNavigationField2D::CostNavigationField2D(int m, int n) : Texture2DBase(m,n),
	NormalizedE(false),
	NormalizedN(0), NormalizedW(0), NormalizedS(0), costMaxE(0), costMaxN(0), costMaxW(0), costMaxS(0), 
		costMinE(0), costMinN(0), costMinW(0), costMinS(0)
{
	int pixel_num = m * n; 
	//MAC STYLE
	CostVector defvalue; 
	defvalue.push_back(0);
	defvalue.push_back(0); 
	defvalue.push_back(0); 
	defvalue.push_back(0); 
	defvalue.push_back(0); 

	this->m_costNavigationField.resize(pixel_num, defvalue); 
}

void CostNavigationField2D::clearAll(){
	Texture2DBase::clearAll(); 
	this->m_costNavigationField.clear();
	
	CostVector defvalue; 
	defvalue.push_back(0);
	defvalue.push_back(0); 
	defvalue.push_back(0); 
	defvalue.push_back(0); 

	this->m_costNavigationField.resize(Texture2DBase::m_grid*Texture2DBase::n_grid,defvalue);
}

void CostNavigationField2D::Normalize(int direction){

	switch(direction)
	{
	case EAST:
		if(NormalizedE == false){
		for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
			for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){


				CostVector costmac = this->readCostMac(cellx, cellz); 
				costmac[0]/= costMaxE; 
				this->write(cellx, cellz, costmac); 
			}
		}
		
		NormalizedE = true;
		}
		break;
	case NORTH:
		if(NormalizedN == false){
		for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
			for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

				CostVector costmac = this->readCostMac(cellx, cellz); 
				costmac[1]/= costMaxN; 
				this->write(cellx, cellz, costmac); 
			}
		}
		NormalizedN = true;
		}
		break;
	case WEST:
		if(NormalizedW == false){
			for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
				for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

					CostVector costmac = this->readCostMac(cellx, cellz); 
					costmac[2]/= costMaxW; 
					this->write(cellx, cellz, costmac); 
				}
			}
			NormalizedW = true;
		}
		break;
	case SOUTH:
		if(NormalizedS == false){
		for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
			for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

				CostVector costmac = this->readCostMac(cellx, cellz); 
				costmac[3]/= costMaxS; 
				this->write(cellx, cellz, costmac); 
			}
		}
		NormalizedS = true;
		}
		break;
	}
}

void CostNavigationField2D::DeNormalize(int direction)
{

	switch(direction)
	{
		case EAST:
		if(NormalizedE == true){
			for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
				for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

					CostVector costmac = this->readCostMac(cellx, cellz); 
					costmac[0] *= costMaxE; 
					this->write(cellx, cellz, costmac); 
				}
			}
			NormalizedE = false;
		}
		break;
	case NORTH:
		if(NormalizedN == true){
			for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
				for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

					CostVector costmac = this->readCostMac(cellx, cellz); 
					costmac[1] *= costMaxN; 
					this->write(cellx, cellz, costmac); 
				}
			}
			NormalizedN = false;
		}
		break;
	case WEST:
		if(NormalizedW == true){
			for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
				for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

					CostVector costmac = this->readCostMac(cellx, cellz); 
					costmac[2] *= costMaxW; 
					this->write(cellx, cellz, costmac); 
				}
			}
			NormalizedW = false;
		}
		break;
	case SOUTH:
		if(NormalizedS == true){
			for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
				for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

					CostVector costmac = this->readCostMac(cellx, cellz); 
					costmac[3] *= costMaxS; 
					this->write(cellx, cellz, costmac); 
				}
			}
			NormalizedS = false;
		}
		break;
	}
}

void CostNavigationField2D::init(DiscomfortNavigationField2D& discomfort_navi2d)
{
	
	char debugstring[256];
	//core algorithm 
	//iterate each pixel and calculate anisotropic cost
	int eastcellx, northcellx, westcellx, southcellx; 
	int eastcellz, northcellz, westcellz, southcellz; 

	float costE, costN, costW, costS; 
	float gE, gN, gW, gS; 

	costMaxN = costMaxE = costMaxW = costMaxS = 0;
	costMinN = costMinE = costMinW = costMinS = 0; 
	
	for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
		for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++)
		{
			CostVector cv = this->readCostMac(cellx, cellz); 

			if(discomfort_navi2d.readDiscomfortNavigationField2D(cellx, cellz) == POSINFINITE){

				cv[4] = 1; 
			}

			eastcellx = cellx + 1; 
			eastcellz = cellz ; 

			northcellx = cellx; 
			northcellz = cellz + 1; 

			westcellx = cellx -1; 
			westcellz = cellz; 

		    southcellx = cellx; 
			southcellz = cellz -1; 

			if(!UtilHelper::CellOutofBoundary(eastcellx, eastcellz)){

				gE = discomfort_navi2d.readDiscomfortNavigationField2D(eastcellx, eastcellz); 

				if(gE == POSINFINITE){

					cv[EAST] = POSINFINITE;
				}
				else{

					cv[EAST] = 1.0f; 
				}
			}
			else{

				cv[EAST] = POSINFINITE;  
			}

			if(!UtilHelper::CellOutofBoundary(northcellx, northcellz))
			{
				gN = discomfort_navi2d.readDiscomfortNavigationField2D(northcellx, northcellx); 

				if(gN == POSINFINITE){

					cv[NORTH] = POSINFINITE;
				}
				else{

					cv[NORTH] = 1.0f; 
				}
			}
			else{
				cv[NORTH] = POSINFINITE;  
			}

			if(!UtilHelper::CellOutofBoundary(westcellx, westcellz)){
				gW = discomfort_navi2d.readDiscomfortNavigationField2D(westcellx, westcellz); 

				if(gW == POSINFINITE){

					cv[WEST] = POSINFINITE;
				}
				else{

					cv[WEST] = 1.0f; 
				}
			}
			else{

				cv[WEST] = POSINFINITE;  
			}

			if(!UtilHelper::CellOutofBoundary(southcellx, southcellz)){
				gS = discomfort_navi2d.readDiscomfortNavigationField2D(southcellx, southcellz); 

				if(gS == POSINFINITE){

					cv[SOUTH] = POSINFINITE;
				}
				else{

					cv[SOUTH] = 1.0f; 
				}
			}
			else{

				cv[SOUTH] = POSINFINITE;  
			}

			//Updating the pixel value

			if(cv[EAST]!=POSINFINITE &&cv[EAST] > costMaxE)
			{
				costMaxE = cv[EAST]; 
			}

			if(cv[EAST]!=POSINFINITE && cv[EAST] < costMinE)
			{
				costMinE = cv[EAST];
			}

			if(cv[NORTH]!=POSINFINITE &&cv[NORTH] > costMaxN)
			{
				costMaxN = cv[NORTH];
			}

			if(cv[NORTH]!=POSINFINITE &&cv[NORTH] < costMinN)
			{
				costMinN = cv[NORTH];
			}

			if(cv[WEST]!=POSINFINITE &&cv[WEST] > costMaxW)
			{
				costMaxW = cv[WEST];
			}

			if(cv[WEST]!=POSINFINITE &&cv[WEST] < costMinW)
			{
				costMinW = cv[WEST];
			}

		    if(cv[SOUTH]!=POSINFINITE &&cv[SOUTH] > costMaxS)
			{
				costMaxS = cv[SOUTH];
			}

			if(cv[SOUTH]!=POSINFINITE &&cv[SOUTH] < costMinS)
			{
				costMinS = cv[SOUTH];
			}

			this->write(cellx, cellz, cv);

			if(cellx == 13 && cellz == 16){
				cellx = cellx; 
			}
			printf("cellx: %d, cellz: %d, costE:%f, costN:%f, costW:%f, costS:%f\n", cellx, cellz, cv[0], cv[1], cv[2], cv[3]);

			//if(cv[1] > 400.f){
			//sprintf( debugstring, "cellx: %d, cellz: %d, costE:%f, costN:%f, costW:%f, costS:%f\n", 
			//	cellx, cellz, cv[0], cv[1], cv[2], cv[3]);
			//	OutputDebugString(debugstring);
			//}
			//if(cv[0] > 400.f){
			//	sprintf( debugstring, "cellx: %d, cellz: %d, fE: %f, gE: %f, costE: %f\n", cellx, cellz, fE, gE, cv[0]);
			//	OutputDebugString(debugstring);
			//}
		}

		
	}
	costMinN = costMinN;
}

CostVector CostNavigationField2D::readCostMac(int px, int py)
{
	return m_costNavigationField.at(mapping(px, py));
}

void CostNavigationField2D::setAll(CostVector cost_mac)
{
	int pixel_num = Texture2DBase::getGridX() * Texture2DBase::getGridY(); 
	this->m_costNavigationField.resize(pixel_num, cost_mac);
}

void CostNavigationField2D::write(int px, int py,  CostVector cost_mac)
{
	int pixel_num = mapping(px, py); 
	this->m_costNavigationField.at(pixel_num) = cost_mac; 
}