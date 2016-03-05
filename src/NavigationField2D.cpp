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

#include "NavigationField2D.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm> 
#include <functional>  
#include "Windows.h"
#include <fstream>
#include <sstream>
#include <iomanip>

NavigationField2D::NavigationField2D(int m, int n) : Texture2DBase(m,n), Normalized(false)
{ 

	int pixel_num = m * n; 
	PathCost initial(POSINFINITE, UNKNOWN, NOTNEIGHBOR); 

	this->m_navigationfield2D.resize(pixel_num, initial); 

	this->m_state2D.resize(pixel_num, State());

	for(int i = 0; i<UtilHelper::GridDimensionX; i++){
		for(int j = 0; j< UtilHelper::GridDimensionZ; j++){
			this->writeState(i, j, State(i, j)); 
		}
	}
}

void NavigationField2D::InitializeGoalCell(std::vector<int>& cellx, std::vector<int>& cellz)
{
	this->seedgridcellx = cellx;
	this->seedgridcellz = cellz;
}


void NavigationField2D::clearAll(){

	Texture2DBase::clearAll(); 
	this->m_navigationfield2D.clear();
	this->m_state2D.clear(); 

	PathCost initial(POSINFINITE, UNKNOWN, NOTNEIGHBOR); 
	this->m_navigationfield2D.resize(Texture2DBase::m_grid*Texture2DBase::n_grid, initial); 

	this->m_state2D.resize(Texture2DBase::m_grid*Texture2DBase::n_grid,  State()); 
		for(int i = 0; i<UtilHelper::GridDimensionX; i++){
		for(int j = 0; j< UtilHelper::GridDimensionZ; j++){
			this->writeState(i, j, State(i, j)); 
		}
	}
}


void NavigationField2D::GetPathCost(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad, std::vector<NeighboringGrid>& currentNeighboringGrid, 
	std::vector<PathCost>& unsortedPathCost)
{
	//iterate all neighboring cell and update their phi value, less than previous value
	int i = 0;
	char debugstring[256];
	for(int nb_id = 0; nb_id < currentNeighboringGrid.size(); nb_id++)
	{
		std::vector<NeighboringGrid> neighboringlist_nb_id; 
		int cellMx, cellMz; 
		int cellEastx, cellEastz; 
		int cellNorthx, cellNorthz;
		int cellWestx, cellWestz; 
		int cellSouthx, cellSouthz; 

		cellMx = currentNeighboringGrid[nb_id].gridx;
		cellMz = currentNeighboringGrid[nb_id].gridz; 

		if(this->readPathCost(cellMx, cellMz).neighborstate == 1){
			continue; 
		}

		neighboringlist_nb_id = getNeibhoringGrid(cellMx, cellMz); 

		cellEastx = neighboringlist_nb_id[0].gridx; 
		cellEastz = neighboringlist_nb_id[0].gridz; 

		cellNorthx = neighboringlist_nb_id[1].gridx; 
		cellNorthz = neighboringlist_nb_id[1].gridz;

		cellWestx = neighboringlist_nb_id[2].gridx; 
		cellWestz = neighboringlist_nb_id[2].gridz;

		cellSouthx = neighboringlist_nb_id[3].gridx; 
		cellSouthz = neighboringlist_nb_id[3].gridz;

		// 1 or infinite big
		CostVector costM = cost_navi2D.readCostMac(cellMx, cellMz); 

		float TNeighboringTraversal[4]; 
		float TNeighboring[4];
		float costMAC[4]; 
		costMAC[EAST] = costM[EAST];
		costMAC[NORTH] = costM[NORTH];
		costMAC[WEST] = costM[WEST];
		costMAC[SOUTH] = costM[SOUTH];

		int NotTraversable = 0; 

		//East
		if(cellEastx!=-1 && cellEastz != -1){//within the boundary
			PathCost pathcostE = this->readPathCost(cellEastx, cellEastz); 
			TNeighboring[EAST] = pathcostE.pathcost; 
			if(pathcostE.knowstate == KNOWN){
				//M to E traversal cost
				if(pathcostE.pathcost == -1){
					printf("error"); 
				}
				else{
					TNeighboringTraversal[EAST] = costM[EAST] + pathcostE.pathcost; // transporting direction
					if(TNeighboringTraversal[EAST] >= POSINFINITE){
						TNeighboringTraversal[EAST] = POSINFINITE;
					}
				}
			}
			else{
				//do nothing assume cost infinete
				TNeighboringTraversal[EAST] = POSINFINITE;
				NotTraversable++;
				//costMAC[EAST] = POSINFINITE;
			}
		}
		else{
				//do nothing assume cost infinete
				TNeighboringTraversal[EAST] = POSINFINITE;
				NotTraversable++; 
				//costMAC[EAST] = POSINFINITE;
		}

		//North
		if(cellNorthx!=-1 && cellNorthz != -1){//within the boundary
			PathCost pathcostN = this->readPathCost(cellNorthx, cellNorthz); 
			TNeighboring[NORTH] = pathcostN.pathcost;
			if(pathcostN.knowstate == KNOWN){
				//M to N Traversal cost
				if(pathcostN.pathcost == -1){
					printf("error"); 
				}
				else{
					TNeighboringTraversal[NORTH] = costM[NORTH] + pathcostN.pathcost;
					if(TNeighboringTraversal[NORTH] >= POSINFINITE){
						TNeighboringTraversal[NORTH] = POSINFINITE;
					}
					//costMAC[NORTH] = costM[NORTH];
				}
			}
			else{
				//do nothing assume cost infinete
				TNeighboringTraversal[NORTH] = POSINFINITE;
				NotTraversable++;
				//costMAC[NORTH] = POSINFINITE;
			}
		}
		else{
				//do nothing assume cost infinete
				TNeighboringTraversal[NORTH] = POSINFINITE;
				NotTraversable++;
				//costMAC[NORTH] = POSINFINITE;
		}
		//WEST
		if(cellWestx!=-1 && cellWestz != -1){//within the boundary

			PathCost pathcostW = this->readPathCost(cellWestx, cellWestz); 
			TNeighboring[WEST] = pathcostW.pathcost;
			if(pathcostW.knowstate == KNOWN){
				//M to W Traversal cost
				if(pathcostW.pathcost == -1){
					printf("error"); 
				}
				else{
					TNeighboringTraversal[WEST] = costM[WEST] + pathcostW.pathcost;
					if(TNeighboringTraversal[WEST] >= POSINFINITE){
						TNeighboringTraversal[WEST] = POSINFINITE;
					}
					//costMAC[WEST] = costM[WEST];
				}
			}
			else{
				//do nothing assume cost infinete
				TNeighboringTraversal[WEST] = POSINFINITE;
				NotTraversable++;
				//costMAC[WEST] = POSINFINITE;
			}
		}
		else{
				//do nothing assume cost infinete
				TNeighboringTraversal[WEST] = POSINFINITE;
				NotTraversable++;
				//costMAC[WEST] = POSINFINITE;
		}

		//SOUTH
		if(cellSouthx!=-1 && cellSouthz != -1){//within the boundary
			PathCost pathcostS = this->readPathCost(cellSouthx, cellSouthz); 
			TNeighboring[SOUTH] = pathcostS.pathcost;
			if(pathcostS.knowstate == KNOWN){
				//M to S Traversal cost
				if(pathcostS.pathcost == -1){
					printf("error"); 
				}
				else{
					TNeighboringTraversal[SOUTH] = costM[SOUTH] + pathcostS.pathcost;
					if(TNeighboringTraversal[SOUTH] >= POSINFINITE){
						TNeighboringTraversal[SOUTH] = POSINFINITE;
					}					
					//costMAC[SOUTH] = costM[SOUTH];
				}
			}
			else{
				//do nothing assume cost infinete
				TNeighboringTraversal[SOUTH] = POSINFINITE;
				NotTraversable++;
				//costMAC[SOUTH] = POSINFINITE;
			}
		}
		else{
				//do nothing assume cost infinete
				TNeighboringTraversal[SOUTH] = POSINFINITE;
				NotTraversable++;
				//costMAC[SOUTH] = POSINFINITE;
		}

		if( NotTraversable <2 && cellMx!=16 && cellMz !=16)
		{
			NotTraversable = NotTraversable;
		}
		
		//new version
		float TCurrentMin = POSINFINITE; 
		int XXmin, YYmin; 
		float avectormin[2]; 
		float TA_min,TB_min; 
		for(int XX = 0; XX < 2; XX++)
		{
			for(int YY = 0 ; YY < 2; YY++)
			{
				//three case discussion. 
				if(TNeighboringTraversal[2*XX] == POSINFINITE && TNeighboringTraversal[2*YY+1] == POSINFINITE)
				{
					continue; 
				}

				if(TNeighboringTraversal[2*XX] == POSINFINITE && TNeighboringTraversal[2*YY+1] != POSINFINITE)
				{
					float cMx, cMy; 
					cMx = costMAC[2 * XX]; 
					cMy = costMAC[2 * YY +1]; 
					float pathcosttraversal = TNeighboringTraversal[2*YY+1];  

					bool find = false; 
					float pathcostold = this->readPathCost(cellMx, cellMz).pathcost; 

					for(int i = 0; i< unsortedPathCost.size(); i++)
					{
						if(unsortedPathCost[i].cellx == cellMx && unsortedPathCost[i].cellz == cellMz){
							if(pathcosttraversal < pathcostold){

								unsortedPathCost[i].updatecount = unsortedPathCost[i].updatecount + 1; 
								unsortedPathCost[i].pathcost = pathcosttraversal; 

								if(YY == 0){
									unsortedPathCost[i].naviX = 0.0f; 
									unsortedPathCost[i].naviZ = 1.0f; 
								}
								else{
									unsortedPathCost[i].naviX = 0.0f; 
									unsortedPathCost[i].naviZ = -1.0f; 
								}
							}
							find = true;
							break;
						}
					}

					if(find == false){
						if(YY == 0)
						{
							unsortedPathCost.push_back(PathCost(pathcosttraversal, 0, 1, cellMx, cellMz,0.f, 1.f, 1));
						}
						else
						{
							unsortedPathCost.push_back(PathCost(pathcosttraversal, 0, 1, cellMx, cellMz, 0.f, -1.f, 1));
						}
					}

					continue; 
				}

				if(TNeighboringTraversal[2*XX] != POSINFINITE && TNeighboringTraversal[2*YY+1] == POSINFINITE)
				{
					float cMx, cMy; 
					cMx = costMAC[2 * XX]; 
					cMy = costMAC[2 * YY +1]; 
					float pathcosttraversal = TNeighboringTraversal[2*XX];  

					bool find = false; 
					float pathcostold = this->readPathCost(cellMx, cellMz).pathcost; 

					for(int i = 0; i< unsortedPathCost.size(); i++)
					{
						if(unsortedPathCost[i].cellx == cellMx && unsortedPathCost[i].cellz == cellMz){
							if(pathcosttraversal < pathcostold){

								unsortedPathCost[i].updatecount = unsortedPathCost[i].updatecount + 1; 
								unsortedPathCost[i].pathcost = pathcosttraversal; 

								if(XX == 0){
									unsortedPathCost[i].naviX = 1.0f; 
									unsortedPathCost[i].naviZ = 0.0f; 
								}
								else{
									unsortedPathCost[i].naviX = -1.0f; 
									unsortedPathCost[i].naviZ = 0.0f; 
								}
							}
							find = true;
							break;
						}
					}

					if(find == false){
						if(XX == 0)
						{
							unsortedPathCost.push_back(PathCost(pathcosttraversal, 0, 1, cellMx, cellMz,1.f, 0.f, 1));
						}
						else
						{
							unsortedPathCost.push_back(PathCost(pathcosttraversal, 0, 1, cellMx, cellMz, -1.f, 0.f, 1));
						}
					}
					continue; 
				}

				if(TNeighboringTraversal[2*XX] != POSINFINITE && TNeighboringTraversal[2*YY+1] != POSINFINITE)
				{
					float TCurrent; 
					float TCurrent2; 

					float avector[2]; 
					float avector2[2]; 
					float amag; 
					float sax; 

					float TA, TB; //A for x,  B for y. 
					float cMx, cMy; 

					TA =  TNeighboring[2*XX]; 
					TB =  TNeighboring[2*YY+1]; 
					cMx = costMAC[2 * XX]; 
					cMy = costMAC[2 * YY +1]; 

					float M; //TB - TA
					M = TB - TA; 

					float GradX, GradY; 			
					GradX = sketchy_grad.readSketchyGradientCopy(cellMx, cellMz).gradientx; 
					GradY = sketchy_grad.readSketchyGradientCopy(cellMx, cellMz).gradientz; 
					//update
					if(GradX !=0 || GradY!=0)
					{
						GradX = GradX;
					}
					//GradX = GradY = 0.0f;
					float D; 
					D =  (GradX * GradX + GradY * GradY - 1); 

					float gxgx = GradX * GradX + GradY * GradY; 

					float C; 
					float J, K, L; 
					float AA, BB,CC; 
					float Delta; 
					float alpha1,alpha2; 
					float dot_ga; 

					C = (GradX - GradY) - M * D; 
					J = (GradX - GradY) * (GradX - GradY) - 2 * D; 
					K = 2 * GradY * (GradX - GradY) + 2 * D; 
					L = GradY * GradY - D;  

					AA = 4 * J *J - 4 * J * C * C ; 
					BB = 4 * J *K - 4 * K * C * C ; 
					CC = K * K - 4 * C * C * L ; 

					Delta = BB * BB - 4 * AA * CC; 

					if(abs(AA) < 0.00001f)
					{
						alpha1 = -CC/BB; 

						if(XX == 0 && YY == 0)
						{
							avector[0] = alpha1; 
							avector[1] = 1 - alpha1; 
						}
						if(XX == 0 && YY == 1)
						{
							avector[0] = alpha1; 
							avector[1] = -1 + alpha1; 
						}
						if(XX == 1 && YY == 0)
						{
							avector[0] = -alpha1; 
							avector[1] = 1 - alpha1; 
						}
						if(XX == 1 && YY == 1)
						{
							avector[0] = -alpha1; 
							avector[1] = -1 + alpha1; 
						}
						amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);
						avector[0]/=amag;
						avector[1]/=amag;
						dot_ga = avector[0] * GradX + avector[1] * GradY; 
						//if G = 0; sax == 1
						sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
					
						if(sax < 0.00001)
						{
							//perfect 90 degree 
							TCurrent = POSINFINITE; 
						}
						else
						{
							TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
						}

						if((TCurrent <= TA && TCurrent > TB) || (TCurrent <= TB && TCurrent > TA))
						{
							TCurrent = TCurrent; 
						}
					}//End of AA == 0
					else
					{//exsited alpha squre
						if(Delta < 0)
						{
							//compare alpha 0 and 1
							if(XX == 0 && YY == 0)
							{
								if(TA > TB )
								{
									avector[0] = 0.0f; 
									avector[1] = 1.0f;
								}
								else
								{
									avector[0] = 1.0f; 
									avector[1] = 0.0f;
								}
							}
							if(XX == 0 && YY == 1)
							{
								if(TA > TB )
								{
									avector[0] = 0.0f; 
									avector[1] = -1.0f;
								}
								else
								{
									avector[0] = 1.0f; 
									avector[1] = 0.0f;
								}
							}
							if(XX == 1 && YY == 0)
							{
								if(TA > TB )
								{
									avector[0] = 0.0f; 
									avector[1] = 1.0f;
								}
								else
								{
									avector[0] = -1.0f; 
									avector[1] = 0.0f;
								}
							}
							if(XX == 1 && YY == 1)
							{
								if(TA > TB )
								{
									avector[0] = 0.0f; 
									avector[1] = -1.0f;
								}
								else
								{
									avector[0] = -1.0f; 
									avector[1] = 0.0f;
								}
							}

							TCurrent = TB + cMy; 
							TCurrent = TA + cMx; 
						}
						else
						{
							Delta = sqrt(Delta);

							alpha1 = (-BB + Delta)/(2 * AA); 
							
							alpha2 = (-BB - Delta)/(2 * AA); 

							bool nosolution = false; 

							if((alpha1 > 1 || alpha1 <0) && ( alpha2 > 1 || alpha2 <0))
							{
								nosolution = true; 
							}
							else if((alpha1 > 1 || alpha1 <0))
							{
								float temp; 
								temp = alpha1;
								alpha1 = alpha2;
								alpha2 = temp; 
							}

							if(nosolution){
								//compare alpha 0 and 1
								if(XX == 0 && YY == 0)
								{
									if(TA > TB )
									{
										avector[0] = 0.0f; 
										avector[1] = 1.0f;
									}
									else
									{
										avector[0] = 1.0f; 
										avector[1] = 0.0f;
									}
								}
								if(XX == 0 && YY == 1)
								{
									if(TA > TB )
									{
										avector[0] = 0.0f; 
										avector[1] = -1.0f;
									}
									else
									{
										avector[0] = 1.0f; 
										avector[1] = 0.0f;
									}
								}
								if(XX == 1 && YY == 0)
								{
									if(TA > TB )
									{
										avector[0] = 0.0f; 
										avector[1] = 1.0f;
									}
									else
									{
										avector[0] = -1.0f; 
										avector[1] = 0.0f;
									}
								}
								if(XX == 1 && YY == 1)
								{
									if(TA > TB )
									{
										avector[0] = 0.0f; 
										avector[1] = -1.0f;
									}
									else
									{
										avector[0] = -1.0f; 
										avector[1] = 0.0f;
									}
								}

								TCurrent = TB + cMy; 
								TCurrent = TA + cMx; 
							}
							else
							{
								if(XX == 0 && YY == 0)
								{
									avector[0] = alpha1; 
									avector[1] = 1 - alpha1; 
									avector2[0] = alpha2; 
									avector2[1] = 1 - alpha2; 
								}
								if(XX == 0 && YY == 1)
								{
									avector[0] = alpha1; 
									avector[1] = -1 + alpha1; 
									avector2[0] = alpha2; 
									avector2[1] = -1 + alpha2; 
								}
								if(XX == 1 && YY == 0)
								{
									avector[0] = -alpha1; 
									avector[1] = 1 - alpha1; 
									avector2[0] = -alpha2; 
									avector2[1] = 1 - alpha2;
								}
								if(XX == 1 && YY == 1)
								{
									avector[0] = -alpha1; 
									avector[1] = -1 + alpha1; 
									avector2[0] = -alpha2; 
									avector2[1] = -1 + alpha2;
								}

								amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

								if(amag > 0){
									avector[0]/= amag; 
									avector[1]/= amag;
								}

								dot_ga = avector[0] * GradX + avector[1] * GradY; 
								sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);

								if(sax < 0.00001){
									TCurrent = POSINFINITE; 
								}
								else{
									TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
								}

								float TCurrentBack = TCurrent; 

								if(alpha2 > 0 && alpha2 < 1){
									amag = sqrt(avector2[0] * avector2[0] + avector2[1] * avector2[1]);
									if(amag > 0){
										avector2[0]/= amag; 
										avector2[1]/= amag;
									}
									dot_ga = avector2[0] * GradX + avector2[1] * GradY; 
									sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
									if(sax < 0.00001){
										TCurrent = POSINFINITE; 
									}
									else{
										TCurrent = alpha2 * TA + (1 - alpha2) * TB + amag/sax; 
									}

									if(TCurrent > TCurrentBack){
										TCurrent = TCurrentBack; 
									}
									else{
										avector[0] = avector2[0]; 
										avector[1] = avector2[1]; 
									}
								}//end of alpha2 case if possible
							}
						}

						if(TCurrent < TCurrentMin){
							TCurrentMin = TCurrent; 
							XXmin = XX; 
							YYmin = YY; 
							avectormin[0] = avector[0]; 
							avectormin[1] = avector[1]; 
							TA_min = TA; 
							TB_min = TB; 
						}
					}
					//
				}
			}
		}//end of for loop to get the minimum T. 

		if(TCurrentMin == POSINFINITE){

			continue; 
		}

		////old version ---------------//

		//compare west and east 
		//float cMx, cMy; 
		//float TNeighboringMx, TNeighboringMy;  //x for EW, y for NS
		//bool NeighboringE,NeighboringS, NeighboringW, NeighboringN; 
		//NeighboringE = NeighboringS = NeighboringW = NeighboringN = false; 

		//if(TNeighboringTraversal[EAST] >= TNeighboringTraversal[WEST]){
		//	if(TNeighboringTraversal[WEST] != POSINFINITE){

		//		TNeighboringMx = this->readPathCost(cellWestx, cellWestz).pathcost; 
		//		cMx = costMAC[WEST];
		//		NeighboringW = true; 

		//	}
		//	else{
		//		cMx = POSINFINITE;
		//	}
		//}

		//if(TNeighboringTraversal[EAST] < TNeighboringTraversal[WEST]){
		//	if(TNeighboringTraversal[EAST] != POSINFINITE )
		//	{
		//		TNeighboringMx = this->readPathCost(cellEastx, cellEastz).pathcost; 
		//		cMx = costMAC[EAST];
		//		NeighboringE = true; 
		//	}
		//	else{
		//		cMx = POSINFINITE; 
		//	}
		//}

		////compare north and south
		//if(TNeighboringTraversal[NORTH] >= TNeighboringTraversal[SOUTH]){
		//	if(TNeighboringTraversal[SOUTH] != POSINFINITE)
		//	{
		//		TNeighboringMy = this->readPathCost(cellSouthx, cellSouthz).pathcost; 
		//		cMy = costMAC[SOUTH];
		//		NeighboringS = true; 
		//	}
		//	else{
		//		cMy = POSINFINITE;
		//	}
		//}
		//if(TNeighboringTraversal[NORTH] < TNeighboringTraversal[SOUTH]){
		//	if(TNeighboringTraversal[NORTH] != POSINFINITE)
		//	{
		//		TNeighboringMy = this->readPathCost(cellNorthx, cellNorthz).pathcost; 
		//		cMy = costMAC[NORTH];
		//		NeighboringN = true; 
		//	}
		//	else
		//	{
		//		cMy = POSINFINITE;
		//	}
		//}

		////cost 
		//if(cMx == POSINFINITE && cMy == POSINFINITE){
		//	printf("error\n"); 
		//	//unsortedPhi.push_back(Phi(POSINFINITE, 0, 1, cellMx, cellMz));
		//}

		//if(cMx == POSINFINITE && cMy != POSINFINITE){

		//					if(cellMx == 16 && cellMz == 26){

		//			cellMx =cellMx; 
		//		}

		//	float pathcosttraversal = cMy + TNeighboringMy;  
		//	bool find = false; 
		//	float pathcostold = this->readPathCost(cellMx, cellMz).pathcost; 

		//	for(int i = 0; i< unsortedPathCost.size(); i++)
		//	{
		//		if(unsortedPathCost[i].cellx == cellMx && unsortedPathCost[i].cellz == cellMz){
		//			if(pathcosttraversal < pathcostold){

		//				unsortedPathCost[i].updatecount = unsortedPathCost[i].updatecount + 1; 
		//				unsortedPathCost[i].pathcost = pathcosttraversal; 

		//				if(NeighboringN == true){
		//					unsortedPathCost[i].naviX = 0.0f; 
		//					unsortedPathCost[i].naviZ = 1.0f; 
		//				}
		//				else{
		//					unsortedPathCost[i].naviX = 0.0f; 
		//					unsortedPathCost[i].naviZ = -1.0f; 
		//				}
		//			}
		//			find = true;
		//			break;
		//		}
		//	}

		//	if(find == false){
		//		if(NeighboringN == true)
		//		{
		//			unsortedPathCost.push_back(PathCost(pathcosttraversal, 0, 1, cellMx, cellMz,0.f, 1.f, 1));
		//		}
		//		else
		//		{
		//			unsortedPathCost.push_back(PathCost(pathcosttraversal, 0, 1, cellMx, cellMz, 0.f, -1.f, 1));
		//		}
		//	}
		//}

		//if(cMy == POSINFINITE && cMx != POSINFINITE){

		//					if(cellMx == 16 && cellMz == 26){

		//			cellMx =cellMx; 
		//		}

		//	float pathcosttraversal = cMx + TNeighboringMx; 
		//	bool find = false;
		//	float pathcostold = this->readPathCost(cellMx, cellMz).pathcost; 

		//	for(int i = 0; i< unsortedPathCost.size(); i++){
		//		if(unsortedPathCost[i].cellx == cellMx && unsortedPathCost[i].cellz == cellMz){
		//			if(pathcosttraversal < pathcostold){
		//				unsortedPathCost[i].pathcost = pathcosttraversal; 
		//				unsortedPathCost[i].updatecount++; 

		//				if(NeighboringE == true){
		//					unsortedPathCost[i].naviX = 1.0f; 
		//					unsortedPathCost[i].naviZ = 0.0f; 
		//				}
		//				else{
		//					unsortedPathCost[i].naviX = -1.0f; 
		//					unsortedPathCost[i].naviZ = 0.0f; 
		//				}
		//			}
		//			find = true;
		//			break;
		//		}
		//	}

		//	if(find == false)
		//	{
		//		if(NeighboringE == true)
		//		{
		//			unsortedPathCost.push_back(PathCost(pathcosttraversal, 0, 1, cellMx, cellMz, 1.f, 0.f, 1));
		//		}
		//		else
		//		{
		//			unsortedPathCost.push_back(PathCost(pathcosttraversal, 0, 1, cellMx, cellMz, -1.f, 0.f, 1));
		//		}
		//	}
		//}

		//if(cMy != POSINFINITE && cMx != POSINFINITE){
		//	
		//	float TCurrent; 
		//	float avector[2]; 
		//	float amag = 0.0f; 
		//	float sax; 

		//	float TA, TB; //A for x,  B for y. 
		//	TA =TNeighboringMx; 
		//	TB =TNeighboringMy; 

		//	float M; //TB - TA
		//	M = TB - TA; 

		//	float GradX, GradY; 			
		//	GradX = sketchy_grad.readSketchyGradientCopy(cellMx, cellMz).gradientx; 
		//	GradY = sketchy_grad.readSketchyGradientCopy(cellMx, cellMz).gradientz; 

		//	if(GradX !=0 || GradY!=0)
		//	{
		//		GradX = GradX;

		//	}

		//	//GradX = GradY = 0.0f;

		//	//if(cellMz >= 20 && cellMx >= 20)
		//	//{

		//	//	GradX = -0.70; 
		//	//	GradY = -0.70; 
		//	//}

		//	//if(GradX > 0.2 && GradY > 0.2)
		//	//{

		//	//	GradX = GradX; 
		//	//}

		//	//GradX = -1.0f; 
		//	//GradY = 0.0f; 

		//	float D; 
		//	D =  (GradX * GradX + GradY * GradY - 1); 

		//	float gxgx = GradX * GradX + GradY * GradY; 
		//	//if(gxgx >1){
		//	//	gxgx = gxgx; 
		//	//}

		//	float C; 
		//	float J, K, L; 
		//	float AA, BB,CC; 
		//	float Delta; 
		//	float alpha1,alpha2; 
		//	float dot_ga; 
		//    
		//	if(NeighboringN == true && NeighboringE == true)
		//	{
		//		// first xiangxian //
		//		//if zero gradient; 
		//		//D = -1; 
		//		//C = TA -TB; //
		//		//J = 2; 
		//		// k = -2; 
		//		//L = 1; 
		//		//AA  = 16 - 8 * C * C; 
		//		//BB = -16 + 8 * C * C; >0
		//		//cc = 4 - 4 * C * C; 
		//		//

		//		C = (GradX - GradY) - M * D; 
		//		J = (GradX - GradY) * (GradX - GradY) - 2 * D; 
		//		K = 2 * GradY * (GradX - GradY) + 2 * D; 
		//		L = GradY * GradY - D;  

		//		AA = 4 * J *J - 4 * J * C * C ; 
		//		BB = 4 * J *K - 4 * K * C * C ; 
		//		CC = K * K - 4 * C * C * L ; 

		//		Delta = BB * BB - 4 * AA * CC; 

		//		if(abs(AA) < 0.00001f && abs(BB)< 0.00001) //not existed
		//		{
		//			//compare alpha 0 and 1
		//			if(TA > TB )
		//			{
		//				avector[0] = 0.0f; 
		//				avector[1] = 1.0f;
		//				TCurrent = TB + cMy; 
		//			}
		//			else{
		//				avector[0] = 1.0f; 
		//				avector[1] = 0.0f;
		//				TCurrent = TA + cMx; 
		//			}
		//			
		//		}//End of AA == 0
		//		else if(abs(AA) < 0.00001f && abs(BB) > 0.00001)
		//		{
		//			alpha1 = -CC/BB; 
		//			if(alpha1 > 1 || alpha1 <0)
		//			{
		//				//compare alpha 0 and 1
		//				if(TA > TB )
		//				{
		//					avector[0] = 0.0f; 
		//					avector[1] = 1.0f;
		//					TCurrent = TB + cMy; 
		//				}
		//				else{
		//					avector[0] = 1.0f; 
		//					avector[1] = 0.0f;
		//					TCurrent = TA + cMx; 
		//				}
		//			}
		//			else
		//			{
		//				avector[0] = alpha1; 
		//				avector[1] = 1 - alpha1; 
		//				amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);
		//				avector[0]/=amag;
		//				avector[1]/=amag;
		//				dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//				//if G = 0; sax == 1
		//				sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
		//			
		//				if(sax < 0.00001){
		//					//perfect 90 degree 
		//					TCurrent = POSINFINITE; 
		//				}
		//				else{

		//					TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
		//				}

		//				if((TCurrent <= TA && TCurrent > TB) || (TCurrent <= TB && TCurrent > TA))
		//				{
		//					TCurrent = TCurrent; 
		//				}
		//			}
		//		}
		//		else
		//		{//exsited alpha squre
		//			if(Delta < 0)
		//			{
		//				//compare alpha 0 and 1
		//				if(TA > TB )
		//				{
		//					avector[0] = 0.0f; 
		//					avector[1] = 1.0f;
		//					TCurrent = TB + cMy; 
		//				}
		//				else{
		//					avector[0] = 1.0f; 
		//					avector[1] = 0.0f;
		//					TCurrent = TA + cMx; 
		//				}
		//			}
		//			else
		//			{
		//				Delta = sqrt(Delta);

		//				alpha1 = (-BB + Delta)/(2 * AA); 
		//					
		//				alpha2 = (-BB - Delta)/(2 * AA); 

		//				bool nosolution = false; 

		//				if((alpha1 > 1 || alpha1 <0) && ( alpha2 > 1 || alpha2 <0))
		//				{
		//					nosolution = true; 
		//				}
		//				else if((alpha1 > 1 || alpha1 <0))
		//				{
		//					float temp; 
		//					temp = alpha1;
		//					alpha1 = alpha2;
		//					alpha2 = temp; 
		//				}

		//				if(nosolution){
		//					//compare alpha 0 and 1
		//					if(TA > TB )
		//					{
		//						avector[0] = 0.0f; 
		//						avector[1] = 1.0f;
		//						TCurrent = TB + cMy; 
		//					}
		//					else{
		//						avector[0] = 1.0f; 
		//						avector[1] = 0.0f;
		//						TCurrent = TA + cMx; 
		//					}
		//				}
		//				else
		//				{
		//					avector[0] = alpha1; 
		//					avector[1] = 1 - alpha1; 
		//					amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

		//					if(amag > 0){
		//						avector[0]/= amag; 
		//						avector[1]/= amag;
		//					}

		//					dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//					sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);

		//					if(sax < 0.00001){
		//						TCurrent = POSINFINITE; 
		//					}
		//					else{
		//						TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
		//					}

		//					float avectorback[2] = {avector[0], avector[1]};
		//					float TCurrentBack = TCurrent; 

		//					if(alpha2 > 0 && alpha2 < 1){
		//						avector[0] = alpha2; 
		//						avector[1] = 1 - alpha2; 
		//						amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

		//						if(amag > 0){
		//							avector[0]/= amag; 
		//							avector[1]/= amag;
		//						}
		//						dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//						sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
		//						if(sax < 0.00001){
		//							TCurrent = POSINFINITE; 
		//						}
		//						else{
		//							TCurrent = alpha2 * TA + (1 - alpha2) * TB + amag/sax; 
		//						}

		//						if(TCurrent > TCurrentBack){

		//							avector[0] = avectorback[0]; 
		//							avector[1] = avectorback[1]; 
		//							TCurrent = TCurrentBack; 
		//						}
		//					}//end of alpha2 case if possible
		//				}//atleast one solution between zero to one
		//			}//delta > 0 
		//		}
		//	}
		//	else if(NeighboringN == true && NeighboringW == true)
		//	{
		//		//ii xiangxian 
		//		//G = 0; 
		//		//C = //C = TA -TB; //
		//		//J = 2; 
		//		// K = -2; 
		//		// L = 1
		//		//AA = 16 - 8 * C * C
		//		//BB = -16 + 8 *C *C; 
		//		//CC = 4 - 4 * C * C 
		//		//Delta  = 8 * C *C(16- 8C)// 
		//		
		//		//IF (ta == tb), C = 0; if(c =1 ), 

		//		C = (-GradX - GradY) - M * D; 
		//		J = (GradX + GradY) * (GradX + GradY) - 2 * D; 
		//		K = -2 * GradY * (GradX + GradY) + 2 * D; 
		//		L = GradY * GradY - D; 

		//		AA = 4 * J *J - 4 * J * C * C ; 
		//		BB = 4 * J *K - 4 * K * C * C ; 
		//		CC = K * K - 4 * C * C * L ; 

		//		Delta = BB * BB - 4 * AA * CC; 

		//		if(abs(AA) < 0.00001f)
		//		{
		//			alpha1 = -CC/BB; 

		//			avector[0] = -alpha1; 
		//			avector[1] = 1 - alpha1; 

		//			amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);
		//			avector[0]/=amag;
		//			avector[1]/=amag;

		//			dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//			//if G = 0; sax == 1
		//			sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
		//			
		//			if(sax < 0.00001){
		//				//perfect 90 degree 
		//				TCurrent = POSINFINITE; 
		//			}
		//			else{

		//				TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
		//			}

		//			if((TCurrent <= TA && TCurrent > TB) || (TCurrent <= TB && TCurrent > TA))
		//			{
		//				TCurrent = TCurrent; 
		//			}
		//		}//End of AA == 0
		//		else
		//		{//exsited alpha squre
		//			if(Delta < 0)
		//			{
		//				//compare alpha 0 and 1
		//				if(TA > TB )
		//				{
		//					avector[0] = 0.0f; 
		//					avector[1] = 1.0f;
		//					TCurrent = TB + cMy; 
		//				}
		//				else{
		//					avector[0] = -1.0f; 
		//					avector[1] = 0.0f;
		//					TCurrent = TA + cMx; 
		//				}
		//			}
		//			else
		//			{
		//				Delta = sqrt(Delta);

		//				alpha1 = (-BB + Delta)/(2 * AA); 
		//					
		//				alpha2 = (-BB - Delta)/(2 * AA); 

		//				bool nosolution = false; 

		//				if((alpha1 > 1 || alpha1 <0) && ( alpha2 > 1 || alpha2 <0))
		//				{
		//					nosolution = true; 
		//				}
		//				else if((alpha1 > 1 || alpha1 <0))
		//				{
		//					float temp; 
		//					temp = alpha1;
		//					alpha1 = alpha2;
		//					alpha2 = temp; 
		//				}

		//				if(nosolution){
		//					//compare alpha 0 and 1
		//					if(TA > TB )
		//					{
		//						avector[0] = 0.0f; 
		//						avector[1] = 1.0f;
		//						TCurrent = TB+ cMy; 
		//					}
		//					else{
		//						avector[0] = -1.0f; 
		//						avector[1] = 0.0f;
		//						TCurrent = TA+ cMx ; 
		//					}
		//				}
		//				else
		//				{
		//					avector[0] = -alpha1; 
		//					avector[1] = 1 - alpha1; 
		//					amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

		//					if(amag > 0){
		//						avector[0]/= amag; 
		//						avector[1]/= amag;
		//					}

		//					dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//					sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);

		//					if(sax < 0.00001){
		//						TCurrent = POSINFINITE; 
		//					}
		//					else{
		//						TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
		//					}

		//					float avectorback[2] = {avector[0], avector[1]};
		//					float TCurrentBack = TCurrent; 

		//					if(alpha2 > 0 && alpha2 < 1){
		//						avector[0] = -alpha2; 
		//						avector[1] = 1 - alpha2; 
		//						amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

		//						if(amag > 0){
		//							avector[0]/= amag; 
		//							avector[1]/= amag;
		//						}
		//						dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//						sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
		//						if(sax < 0.00001){
		//							TCurrent = POSINFINITE; 
		//						}
		//						else{
		//							TCurrent = alpha2 * TA + (1 - alpha2) * TB + amag/sax; 
		//						}

		//						if(TCurrent > TCurrentBack){

		//							avector[0] = avectorback[0]; 
		//							avector[1] = avectorback[1]; 
		//							TCurrent = TCurrentBack; 
		//						}
		//					}//end of alpha2 case if possible
		//				}
		//			}
		//		}
		//	}
		//	else if(NeighboringS == true && NeighboringE == true)
		//	{
		//		//iii xiangxian 
		//		//ii xiangxian 
		//		//G = 0; 
		//		//C = //C = TA -TB; //
		//		//J = 2; 
		//		//K = -2; 
		//		//L = 1
		//		//AA = 16 - 8 * C * C
		//		//BB = -16 + 8 *C *C; 
		//		//CC = 4 - 4 * C * C 
		//		//Delta  = 8 * C *C(16- 8C)// 
		//		
		//		//IF (ta == tb), C = 0; if(c =1 ), 



		//		C = (-GradX + GradY) - M * D; 
		//		J = (-GradX + GradY) * (-GradX + GradY) - 2 * D; 
		//		K = 2 * GradY * (GradX - GradY) + 2 * D; 
		//		L = GradY * GradY - D;  

		//		AA = 4 * J *J - 4 * J * C * C ; 
		//		BB = 4 * J *K - 4 * K * C * C ; 
		//		CC = K * K - 4 * C * C * L ; 

		//		Delta = BB * BB - 4 * AA * CC; 

		//		if(abs(AA) < 0.00001f)
		//		{
		//			alpha1 = -CC/BB; 

		//			avector[0] = alpha1; 
		//			avector[1] =  -1 + alpha1; 

		//			amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);
		//			avector[0]/=amag;
		//			avector[1]/=amag;

		//			dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//			//if G = 0; sax == 1
		//			sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
		//			
		//			if(sax < 0.00001){
		//				//perfect 90 degree 
		//				TCurrent = POSINFINITE; 
		//			}
		//			else{

		//				TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
		//			}

		//			if((TCurrent <= TA && TCurrent > TB) || (TCurrent <= TB && TCurrent > TA))
		//			{
		//				TCurrent = TCurrent; 
		//			}
		//		}//End of AA == 0
		//		else
		//		{//exsited alpha squre
		//			if(Delta < 0)
		//			{
		//				//compare alpha 0 and 1
		//				if(TA > TB )
		//				{
		//					avector[0] = 0.0f; 
		//					avector[1] = -1.0f;
		//					TCurrent = TB + cMy; 
		//				}
		//				else{
		//					avector[0] = 1.0f; 
		//					avector[1] = 0.0f;
		//					TCurrent = TA + cMx; 
		//				}
		//			}
		//			else
		//			{
		//				Delta = sqrt(Delta);

		//				alpha1 = (-BB + Delta)/(2 * AA); 
		//					
		//				alpha2 = (-BB - Delta)/(2 * AA); 

		//				bool nosolution = false; 

		//				if((alpha1 > 1 || alpha1 <0) && ( alpha2 > 1 || alpha2 <0))
		//				{
		//					nosolution = true; 
		//				}
		//				else if((alpha1 > 1 || alpha1 <0))
		//				{
		//					float temp; 
		//					temp = alpha1;
		//					alpha1 = alpha2;
		//					alpha2 = temp; 
		//				}

		//				if(nosolution){
		//					//compare alpha 0 and 1
		//					if(TA > TB )
		//					{
		//						avector[0] = 0.0f; 
		//						avector[1] = -1.0f;
		//						TCurrent = TB +cMy; 
		//					}
		//					else{
		//						avector[0] = 1.0f; 
		//						avector[1] = 0.0f;
		//						TCurrent = TA + cMx; 
		//					}
		//				}
		//				else
		//				{
		//					avector[0] = alpha1; 
		//					avector[1] = -1 + alpha1; 
		//					amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

		//					if(amag > 0){
		//						avector[0]/= amag; 
		//						avector[1]/= amag;
		//					}

		//					dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//					sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);

		//					if(sax < 0.00001){
		//						TCurrent = POSINFINITE; 
		//					}
		//					else{
		//						TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
		//					}
		//					float avectorback[2] = {avector[0], avector[1]};
		//					float TCurrentBack = TCurrent; 

		//					if(alpha2 > 0 && alpha2 < 1){
		//						avector[0] = alpha2; 
		//						avector[1] = -1 + alpha2; 
		//						amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

		//						if(amag > 0){
		//							avector[0]/= amag; 
		//							avector[1]/= amag;
		//						}
		//						dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//						sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
		//						if(sax < 0.00001){
		//							TCurrent = POSINFINITE; 
		//						}
		//						else{
		//							TCurrent = alpha2 * TA + (1 - alpha2) * TB + amag/sax; 
		//						}

		//						if(TCurrent > TCurrentBack){

		//							avector[0] = avectorback[0]; 
		//							avector[1] = avectorback[1]; 
		//							TCurrent = TCurrentBack; 
		//						}
		//					}//end of alpha2 case if possible
		//				}
		//			}
		//		}
		//	}
		//	else if(NeighboringS == true && NeighboringW == true)
		//	{
		//		//iv xiang xian
		//		//G = 0; 
		//		//C = //C = TA -TB; //
		//		//J = 2; 
		//		//K = -2; 
		//		//L = 1
		//		//AA = 16 - 8 * C * C
		//		//BB = -16 + 8 *C *C; 
		//		//CC = 4 - 4 * C * C 
		//		//Delta  = 8 * C *C(16- 8C)// 
		//		
		//		//IF (ta == tb), C = 0; if(c =1 ), 

		//		C = (GradX + GradY) - M * D; 
		//		J = (GradX + GradY) * (GradX + GradY) - 2 * D; 
		//		K = -2 * GradY * (GradX + GradY) + 2 * D; 
		//		L = GradY * GradY - D;  

		//		AA = 4 * J *J - 4 * J * C * C ; 
		//		BB = 4 * J *K - 4 * K * C * C ; 
		//		CC = K * K - 4 * C * C * L ; 

		//		Delta = BB * BB - 4 * AA * CC; 

		//		if(abs(AA) < 0.00001f && abs(BB) > 0.0001)
		//		{

		//			alpha1 = -CC/BB; 

		//			avector[0] = -alpha1; 
		//			avector[1] =  -1 + alpha1; 

		//			amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);
		//			avector[0]/=amag;
		//			avector[1]/=amag;

		//			dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//			//if G = 0; sax == 1
		//			sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
		//			
		//			if(sax < 0.00001){
		//				//perfect 90 degree 
		//				TCurrent = POSINFINITE; 
		//			}
		//			else{

		//				TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
		//			}

		//			if((TCurrent <= TA && TCurrent > TB) || (TCurrent <= TB && TCurrent > TA))
		//			{
		//				TCurrent = TCurrent; 
		//			}
		//		}//End of AA == 0
		//		else if(abs(AA) < 0.00001f && abs(BB) < 0.00001f)
		//		{

		//			Delta = BB * BB - 4 * AA * CC; 

		//			if(abs(AA) < 0.00001f)
		//			{
		//				//compare alpha 0 and 1
		//				if(TA > TB )
		//				{
		//					avector[0] = 0.0f; 
		//					avector[1] = -1.0f;
		//					TCurrent = TB + cMy; 
		//				}
		//				else{
		//					avector[0] = -1.0f; 
		//					avector[1] = 0.0f;
		//					TCurrent = TA + cMx; 
		//				}
		//			}
		//			else
		//			{
		//				Delta = sqrt(Delta);

		//				alpha1 = (-BB + Delta)/(2 * AA); 
		//					
		//				alpha2 = (-BB - Delta)/(2 * AA); 

		//				bool nosolution = false; 

		//				if((alpha1 > 1 || alpha1 <0) && ( alpha2 > 1 || alpha2 <0))
		//				{
		//					nosolution = true; 
		//				}
		//				else if((alpha1 > 1 || alpha1 <0))
		//				{
		//					float temp; 
		//					temp = alpha1;
		//					alpha1 = alpha2;
		//					alpha2 = temp; 
		//				}

		//				if(nosolution){
		//					//compare alpha 0 and 1
		//					if(TA > TB )
		//					{
		//						avector[0] = 0.0f; 
		//						avector[1] = -1.0f;
		//						TCurrent = TB + cMy; 
		//					}
		//					else{
		//						avector[0] = -1.0f; 
		//						avector[1] = 0.0f;
		//						TCurrent = TA + cMx; 
		//					}
		//				}
		//				else
		//				{
		//					avector[0] = -alpha1; 
		//					avector[1] = -1 + alpha1; 
		//					amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

		//					if(amag > 0){
		//						avector[0]/= amag; 
		//						avector[1]/= amag;
		//					}

		//					dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//					sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);

		//					if(sax < 0.00001){
		//						TCurrent = POSINFINITE; 
		//					}
		//					else{
		//						TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
		//					}

		//				}

		//			}
		//		}
		//		else
		//		{//exsited alpha squre
		//			if(Delta < 0)
		//			{
		//				//compare alpha 0 and 1
		//				if(TA > TB )
		//				{
		//					avector[0] = 0.0f; 
		//					avector[1] = -1.0f;
		//					TCurrent = TB + cMy; 
		//				}
		//				else{
		//					avector[0] = -1.0f; 
		//					avector[1] = 0.0f;
		//					TCurrent = TA + cMx; 
		//				}
		//			}
		//			else
		//			{
		//				Delta = sqrt(Delta);

		//				alpha1 = (-BB + Delta)/(2 * AA); 
		//					
		//				alpha2 = (-BB - Delta)/(2 * AA); 

		//				bool nosolution = false; 

		//				if((alpha1 > 1 || alpha1 <0) && ( alpha2 > 1 || alpha2 <0))
		//				{
		//					nosolution = true; 
		//				}
		//				else if((alpha1 > 1 || alpha1 <0))
		//				{
		//					float temp; 
		//					temp = alpha1;
		//					alpha1 = alpha2;
		//					alpha2 = temp; 
		//				}

		//				if(nosolution){
		//					//compare alpha 0 and 1
		//					if(TA > TB )
		//					{
		//						avector[0] = 0.0f; 
		//						avector[1] = -1.0f;
		//						TCurrent = TB + cMy; 
		//					}
		//					else{
		//						avector[0] = -1.0f; 
		//						avector[1] = 0.0f;
		//						TCurrent = TA + cMx; 
		//					}
		//				}
		//				else
		//				{
		//					avector[0] = -alpha1; 
		//					avector[1] = -1 + alpha1; 
		//					amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

		//					if(amag > 0){
		//						avector[0]/= amag; 
		//						avector[1]/= amag;
		//					}

		//					dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//					sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);

		//					if(sax < 0.00001){
		//						TCurrent = POSINFINITE; 
		//					}
		//					else{
		//						TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
		//					}
		//					float avectorback[2] = {avector[0], avector[1]};
		//					float TCurrentBack = TCurrent; 

		//					if(alpha2 > 0 && alpha2 < 1){
		//						avector[0] = -alpha2; 
		//						avector[1] = -1 + alpha2; 
		//						amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

		//						if(amag > 0){
		//							avector[0]/= amag; 
		//							avector[1]/= amag;
		//						}
		//						dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//						sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
		//						if(sax < 0.00001){
		//							TCurrent = POSINFINITE; 
		//						}
		//						else{
		//							TCurrent = alpha2 * TA + (1 - alpha2) * TB + amag/sax; 
		//						}

		//						if(TCurrent > TCurrentBack){

		//							avector[0] = avectorback[0]; 
		//							avector[1] = avectorback[1]; 
		//							TCurrent = TCurrentBack; 
		//						}
		//					}//end of alpha2 case if possible
		//				}
		//			}
		//		}
		//	}

		bool updateMarked = false; 
		int numofneighboringless = 0 ; 
		//float deltaA = TA_min - TCurrentMin; 
		//float deltaB = TB_min - TCurrentMin; 
		int direction[4] = {0,0,0,0};

		for(int i = 0; i< 4; i++){
			if(TNeighboring[i] == 0.0f){
				updateMarked = false; 
				break; 
			}
			if(TNeighboringTraversal[i] !=POSINFINITE && TNeighboring[i] > TCurrentMin){
				numofneighboringless++;
				direction[i] = 1; 
				updateMarked = true; 
			}
		}

		if(updateMarked && numofneighboringless == 1){
			numofneighboringless = numofneighboringless; 
		}
		else if(updateMarked && numofneighboringless >1){
			numofneighboringless = numofneighboringless;
			updateMarked = false; 
		}

		PathCost current = this->readPathCost(cellMx, cellMz);
		float currentpathcost = current.pathcost; 
		
		bool find = false;

		for(int i = 0; i< unsortedPathCost.size(); i++){
			if(unsortedPathCost[i].cellx == cellMx && unsortedPathCost[i].cellz == cellMz){
				if(TCurrentMin < currentpathcost)
				{
					unsortedPathCost[i].updatecount++; 
					unsortedPathCost[i].pathcost = TCurrentMin; 
					unsortedPathCost[i].naviX = avectormin[0]; 
					unsortedPathCost[i].naviZ = avectormin[1]; 

		/*			if(updateMarked){
						int jj; 
						for(int j = 0; j<4; j++){
							unsortedPathCost[i].neighbor[j] = 0; 
							if(direction[j])
							{
								jj = j;  
							}
						}
						unsortedPathCost[i].updateneighbor = true; 
						unsortedPathCost[i].neighbor[jj] = 1; 

						sprintf( debugstring, "Warning: cellMx: %d, cellMz:%d, i:%d\n", cellMx, cellMz, jj);
						OutputDebugString(debugstring);
					}*/
				}
				find = true;
				break;
			}
		}

		if(find == false){
			unsortedPathCost.push_back(PathCost(TCurrentMin, 0, 1, cellMx, cellMz, avectormin[0], avectormin[1], 1));
		}
	//	}
	}
}

void NavigationField2D::UpdateMarkedCell(CostNavigationField2D& cost_navi2D, 
	SketchyInterface& sketchy_grad,  int direction, int fromcellx, int fromcellz)
{
	std::vector<NeighboringGrid> neighboringlist_nb_id;

	//iteration, 
	int neighboringx, neighboringz; 
	switch(direction)
	{
		case EAST:
			neighboringx = fromcellx + 1; 
			neighboringz = fromcellz; 
			break;
		case NORTH:
			neighboringx = fromcellx; 
			neighboringz = fromcellz + 1; 
			break;
		case WEST:
			neighboringx = fromcellx - 1; 
			neighboringz = fromcellz; 
			break;
		case SOUTH:
			neighboringx = fromcellx; 
			neighboringz = fromcellz - 1; 
			break;
	}

	int cellMx, cellMz; 
	int cellEastx, cellEastz; 
	int cellNorthx, cellNorthz; 
	int cellWestx, cellWestz; 
	int cellSouthx, cellSouthz; 
	cellMx = neighboringx;
	cellMz = neighboringz; 
	
	if(cellMx == -1 ||cellMz == -1 ||this->readPathCost(cellMx, cellMz).neighborstate == 0 && 
		this->readPathCost(cellMx, cellMz).knowstate == 0){ //include the marked one
		return; 
	}

	if(this->readPathCost(cellMx, cellMz).neighborstate == 0 && 
		this->readPathCost(cellMx, cellMz).knowstate == 1){
		//already been marked
			if(this->readPathCost(cellMx, cellMz).updatecount>=5){
				return; 
			}
	}

	neighboringlist_nb_id = getNeibhoringGrid(cellMx, cellMz); 
	cellEastx = neighboringlist_nb_id[0].gridx; 
	cellEastz = neighboringlist_nb_id[0].gridz; 

	cellNorthx = neighboringlist_nb_id[1].gridx; 
	cellNorthz = neighboringlist_nb_id[1].gridz;

	cellWestx = neighboringlist_nb_id[2].gridx; 
	cellWestz = neighboringlist_nb_id[2].gridz;

	cellSouthx = neighboringlist_nb_id[3].gridx; 
	cellSouthz = neighboringlist_nb_id[3].gridz;

	CostVector costM = cost_navi2D.readCostMac(cellMx, cellMz); 

	float TNeighboringTraversal[4]; 
	float TNeighboring[4];
	float costMAC[4]; 

	costMAC[EAST] = costM[EAST];
	costMAC[NORTH] = costM[NORTH];
	costMAC[WEST] = costM[WEST];
	costMAC[SOUTH] = costM[SOUTH];

	int NotTraversable = 0; 

	//East
	if(cellEastx!=-1 && cellEastz != -1){//within the boundary

		PathCost pathcostE = this->readPathCost(cellEastx, cellEastz); 
		TNeighboring[EAST] = pathcostE.pathcost; 

		if(pathcostE.knowstate == KNOWN){
			//M to E cost
			if(pathcostE.pathcost == -1){
				printf("error"); 
			}
			else{
				TNeighboringTraversal[EAST] = costM[EAST] + pathcostE.pathcost; 
				if(TNeighboringTraversal[EAST]>= POSINFINITE){
					TNeighboringTraversal[EAST] = POSINFINITE;
				}
				//costMAC[EAST] = costM[EAST];
			}
		}
		else{
			//do nothing assume cost infinete
			TNeighboringTraversal[EAST] = POSINFINITE;
			NotTraversable++; 
			//costMAC[EAST] = POSINFINITE;
		}
	}
	else{
			//do nothing assume cost infinete
			TNeighboringTraversal[EAST] = POSINFINITE;
			NotTraversable++; 
			//costMAC[EAST] = POSINFINITE;
	}

	//North
	if(cellNorthx!=-1 && cellNorthz != -1){//within the boundary

		PathCost pathcostN = this->readPathCost(cellNorthx, cellNorthz); 
		TNeighboring[NORTH] = pathcostN.pathcost; 

		if(pathcostN.knowstate == KNOWN){
			//M to E cost
			if(pathcostN.pathcost == -1){
				printf("error"); 
			}
			else{
				TNeighboringTraversal[NORTH] = costM[NORTH] + pathcostN.pathcost; 
				if(TNeighboringTraversal[NORTH]>= POSINFINITE){
					TNeighboringTraversal[NORTH] = POSINFINITE;

				}
				//costMAC[NORTH] = costM[NORTH];
			}
		}
		else{
			//do nothing assume cost infinete
			TNeighboringTraversal[NORTH] = POSINFINITE;
			NotTraversable++; 
			//costMAC[NORTH] = POSINFINITE;
		}
	}
	else{
			//do nothing assume cost infinete
			TNeighboringTraversal[NORTH] = POSINFINITE;
			NotTraversable++; 
			//costMAC[NORTH] = POSINFINITE;
	}

	//WEST
	if(cellWestx!=-1 && cellWestz != -1){//within the boundary

		PathCost pathcostW = this->readPathCost(cellWestx, cellWestz); 
		TNeighboring[WEST] = pathcostW.pathcost; 

		if(pathcostW.knowstate == KNOWN){
			//M to W cost
			if(pathcostW.pathcost == -1){
				printf("error"); 
			}
			else{
				TNeighboringTraversal[WEST] = costM[WEST] + pathcostW.pathcost; 
				if(TNeighboringTraversal[WEST]>= POSINFINITE){
					TNeighboringTraversal[WEST] = POSINFINITE;
				}
				//costMAC[WEST] = costM[WEST];
			}
		}
		else{
			//do nothing assume cost infinete
			TNeighboringTraversal[WEST] = POSINFINITE;
			NotTraversable++; 
			//costMAC[WEST] = POSINFINITE;
		}
	}
	else{
			//do nothing assume cost infinete
			TNeighboringTraversal[WEST] = POSINFINITE;
			NotTraversable++; 
			//costMAC[WEST] = POSINFINITE;
	}

	//SOUTH
	if(cellSouthx!=-1 && cellSouthz != -1){//within the boundary

		PathCost pathcostS = this->readPathCost(cellSouthx, cellSouthz); 			
		TNeighboring[SOUTH] = pathcostS.pathcost; 

		if(pathcostS.knowstate == KNOWN){
			//M to S cost
			if(pathcostS.pathcost == -1){
				printf("error"); 
			}
			else{
				TNeighboringTraversal[SOUTH] = costM[SOUTH] + pathcostS.pathcost; 
				if(TNeighboringTraversal[SOUTH]>= POSINFINITE){
					TNeighboringTraversal[SOUTH] = POSINFINITE;
				}
				//costMAC[SOUTH] = costM[SOUTH];
			}
		}
		else{
			//do nothing assume cost infinete
			TNeighboringTraversal[SOUTH] = POSINFINITE;
			NotTraversable++; 
			//costMAC[SOUTH] = POSINFINITE;
		}
	}
	else{
			//do nothing assume cost infinete
			TNeighboringTraversal[SOUTH] = POSINFINITE;
			NotTraversable++; 
			//costMAC[SOUTH] = POSINFINITE;
	}

	if( NotTraversable <2 && cellMx!=16 && cellMz !=16)
	{
		NotTraversable = NotTraversable;
	}

	float TCurrentMin = POSINFINITE; 
	int XXmin, YYmin; 
	float avectormin[2]; 
	float TA_min,TB_min; 

	for(int XX = 0; XX < 2; XX++)
	{
		for(int YY = 0 ; YY < 2; YY++)
		{
			//three case discussion. 
			if(TNeighboringTraversal[2*XX] == POSINFINITE && TNeighboringTraversal[2*YY+1] == POSINFINITE)
			{
				continue; 
			}

			if(TNeighboringTraversal[2*XX] == POSINFINITE && TNeighboringTraversal[2*YY+1] != POSINFINITE)
			{
				continue; 
			}

			if(TNeighboringTraversal[2*XX] != POSINFINITE && TNeighboringTraversal[2*YY+1] == POSINFINITE)
			{
				continue; 
			}

			if(TNeighboringTraversal[2*XX] != POSINFINITE && TNeighboringTraversal[2*YY+1] != POSINFINITE)
			{
				float TCurrent; 
				float TCurrent2; 

				float avector[2]; 
				float avector2[2]; 
				float amag; 
				float sax; 

				float TA, TB; //A for x,  B for y. 
				float cMx, cMy; 

				TA =  TNeighboring[2*XX]; 
				TB =  TNeighboring[2*YY+1]; 
				cMx = costMAC[2 * XX]; 
				cMy = costMAC[2 * YY +1]; 

				float M; //TB - TA
				M = TB - TA; 

				float GradX, GradY; 			
				GradX = sketchy_grad.readSketchyGradientCopy(cellMx, cellMz).gradientx; 
				GradY = sketchy_grad.readSketchyGradientCopy(cellMx, cellMz).gradientz; 
				//update
				if(GradX !=0 || GradY!=0)
				{
					GradX = GradX;
				}
			
				//GradX = GradY = 0.0f;

				float D; 
				D =  (GradX * GradX + GradY * GradY - 1); 

				float gxgx = GradX * GradX + GradY * GradY; 

				float C; 
				float J, K, L; 
				float AA, BB,CC; 
				float Delta; 
				float alpha1,alpha2; 
				float dot_ga; 

				C = (GradX - GradY) - M * D; 
				J = (GradX - GradY) * (GradX - GradY) - 2 * D; 
				K = 2 * GradY * (GradX - GradY) + 2 * D; 
				L = GradY * GradY - D;  

				AA = 4 * J *J - 4 * J * C * C ; 
				BB = 4 * J *K - 4 * K * C * C ; 
				CC = K * K - 4 * C * C * L ; 

				Delta = BB * BB - 4 * AA * CC; 

				if(abs(AA) < 0.00001f)
				{
					alpha1 = -CC/BB; 

					if(XX == 0 && YY == 0)
					{
						avector[0] = alpha1; 
						avector[1] = 1 - alpha1; 
					}
					if(XX == 0 && YY == 1)
					{
						avector[0] = alpha1; 
						avector[1] = -1 + alpha1; 
					}
					if(XX == 1 && YY == 0)
					{
						avector[0] = -alpha1; 
						avector[1] = 1 - alpha1; 
					}
					if(XX == 1 && YY == 1)
					{
						avector[0] = -alpha1; 
						avector[1] = -1 + alpha1; 
					}
					amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);
					avector[0]/=amag;
					avector[1]/=amag;
					dot_ga = avector[0] * GradX + avector[1] * GradY; 
					//if G = 0; sax == 1
					sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
					
					if(sax < 0.00001)
					{
						//perfect 90 degree 
						TCurrent = POSINFINITE; 
					}
					else
					{
						TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
					}

					if((TCurrent <= TA && TCurrent > TB) || (TCurrent <= TB && TCurrent > TA))
					{
						TCurrent = TCurrent; 
					}
				}//End of AA == 0
				else
				{//exsited alpha squre
					if(Delta < 0)
					{
						//compare alpha 0 and 1
						if(XX == 0 && YY == 0)
						{
							if(TA > TB )
							{
								avector[0] = 0.0f; 
								avector[1] = 1.0f;
							}
							else
							{
								avector[0] = 1.0f; 
								avector[1] = 0.0f;
							}
						}
						if(XX == 0 && YY == 1)
						{
							if(TA > TB )
							{
								avector[0] = 0.0f; 
								avector[1] = -1.0f;
							}
							else
							{
								avector[0] = 1.0f; 
								avector[1] = 0.0f;
							}
						}
						if(XX == 1 && YY == 0)
						{
							if(TA > TB )
							{
								avector[0] = 0.0f; 
								avector[1] = 1.0f;
							}
							else
							{
								avector[0] = -1.0f; 
								avector[1] = 0.0f;
							}
						}
						if(XX == 1 && YY == 1)
						{
							if(TA > TB )
							{
								avector[0] = 0.0f; 
								avector[1] = -1.0f;
							}
							else
							{
								avector[0] = -1.0f; 
								avector[1] = 0.0f;
							}
						}

						TCurrent = TB + cMy; 
						TCurrent = TA + cMx; 
					}
					else
					{
						Delta = sqrt(Delta);

						alpha1 = (-BB + Delta)/(2 * AA); 
							
						alpha2 = (-BB - Delta)/(2 * AA); 

						bool nosolution = false; 

						if((alpha1 > 1 || alpha1 <0) && ( alpha2 > 1 || alpha2 <0))
						{
							nosolution = true; 
						}
						else if((alpha1 > 1 || alpha1 <0))
						{
							float temp; 
							temp = alpha1;
							alpha1 = alpha2;
							alpha2 = temp; 
						}

						if(nosolution){
							//compare alpha 0 and 1
							if(XX == 0 && YY == 0)
							{
								if(TA > TB )
								{
									avector[0] = 0.0f; 
									avector[1] = 1.0f;
								}
								else
								{
									avector[0] = 1.0f; 
									avector[1] = 0.0f;
								}
							}
							if(XX == 0 && YY == 1)
							{
								if(TA > TB )
								{
									avector[0] = 0.0f; 
									avector[1] = -1.0f;
								}
								else
								{
									avector[0] = 1.0f; 
									avector[1] = 0.0f;
								}
							}
							if(XX == 1 && YY == 0)
							{
								if(TA > TB )
								{
									avector[0] = 0.0f; 
									avector[1] = 1.0f;
								}
								else
								{
									avector[0] = -1.0f; 
									avector[1] = 0.0f;
								}
							}
							if(XX == 1 && YY == 1)
							{
								if(TA > TB )
								{
									avector[0] = 0.0f; 
									avector[1] = -1.0f;
								}
								else
								{
									avector[0] = -1.0f; 
									avector[1] = 0.0f;
								}
							}

							TCurrent = TB + cMy; 
							TCurrent = TA + cMx; 
						}
						else
						{
							if(XX == 0 && YY == 0)
							{
								avector[0] = alpha1; 
								avector[1] = 1 - alpha1; 
								avector2[0] = alpha2; 
								avector2[1] = 1 - alpha2; 
							}
							if(XX == 0 && YY == 1)
							{
								avector[0] = alpha1; 
								avector[1] = -1 + alpha1; 
								avector2[0] = alpha2; 
								avector2[1] = -1 + alpha2; 
							}
							if(XX == 1 && YY == 0)
							{
								avector[0] = -alpha1; 
								avector[1] = 1 - alpha1; 
								avector2[0] = -alpha2; 
								avector2[1] = 1 - alpha2;
							}
							if(XX == 1 && YY == 1)
							{
								avector[0] = -alpha1; 
								avector[1] = -1 + alpha1; 
								avector2[0] = -alpha2; 
								avector2[1] = -1 + alpha2;
							}

							amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

							if(amag > 0){
								avector[0]/= amag; 
								avector[1]/= amag;
							}

							dot_ga = avector[0] * GradX + avector[1] * GradY; 
							sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);

							if(sax < 0.00001){
								TCurrent = POSINFINITE; 
							}
							else{
								TCurrent = alpha1 * TA + (1 - alpha1) * TB + amag/sax; 
							}

							float TCurrentBack = TCurrent; 

							if(alpha2 > 0 && alpha2 < 1){
								amag = sqrt(avector2[0] * avector2[0] + avector2[1] * avector2[1]);
								if(amag > 0){
									avector2[0]/= amag; 
									avector2[1]/= amag;
								}
								dot_ga = avector2[0] * GradX + avector2[1] * GradY; 
								sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
								if(sax < 0.00001){
									TCurrent = POSINFINITE; 
								}
								else{
									TCurrent = alpha2 * TA + (1 - alpha2) * TB + amag/sax; 
								}

								if(TCurrent > TCurrentBack){
									TCurrent = TCurrentBack; 
								}
								else{
									avector[0] = avector2[0]; 
									avector[1] = avector2[1]; 
								}
							}//end of alpha2 case if possible
						}
					}

					if(TCurrent < TCurrentMin){
						TCurrentMin = TCurrent; 
						XXmin = XX; 
						YYmin = YY; 
						avectormin[0] = avector[0]; 
						avectormin[1] = avector[1]; 
						TA_min = TA; 
						TB_min = TB; 
					}
				}
				//
			}
		}
	}//end of for loop to get the minimum T. 
	if(TCurrentMin == POSINFINITE){
		return; 
	}

	bool updateMarked = false; 
	int numofneighboringless = 0 ; 
	float deltaA = TA_min - TCurrentMin; 
	float deltaB = TB_min - TCurrentMin; 
	int direction2[4] = {0,0,0,0};

	for(int i = 0; i< 4; i++){
		if(TNeighboring[i] == 0.0f){
			updateMarked = false; 
			break; 
		}
		if(TNeighboringTraversal[i] !=POSINFINITE && TNeighboring[i] > TCurrentMin){
			numofneighboringless++;
			direction2[i] = 1; 
			updateMarked = true; 
		}
	}

	if(updateMarked && numofneighboringless == 1)
	{
		numofneighboringless = numofneighboringless; 
	}
	else if(updateMarked && numofneighboringless >1)
	{
		numofneighboringless = numofneighboringless;
		updateMarked = false; 
	}

	PathCost currentpathcost = this->readPathCost(cellMx,cellMz); 
	char debugstring[256]; 
	if(TCurrentMin < currentpathcost.pathcost)
	{
		if(currentpathcost.knowstate!=1){
			//sprintf( debugstring, "error\n");
			//OutputDebugString(debugstring);
		}

		//sprintf( debugstring, "Propogate Before Updated: cellMx: %d, cellMz:%d, PathCost:%f, NaviX: %f, NaviZ:%f, UpdateCount:%d\n", 
		//	cellMx, cellMz, currentpathcost.pathcost, currentpathcost.naviX, currentpathcost.naviZ,
		//	currentpathcost.updatecount);
		//OutputDebugString(debugstring);
				
		currentpathcost.pathcost = TCurrentMin; 
		currentpathcost.naviX = avectormin[0];
		currentpathcost.naviZ = avectormin[1];
		currentpathcost.updatecount++; 

		//sprintf( debugstring, "Propogate After Updated: cellMx: %d, cellMz:%d, PathCost:%f, NaviX: %f, NaviZ:%f, UpdateCount:%d\n", 
		//cellMx, cellMz, currentpathcost.pathcost, currentpathcost.naviX, currentpathcost.naviZ,
		//	currentpathcost.updatecount);
		//OutputDebugString(debugstring);

		this->write(cellMx, cellMz, currentpathcost); 
	}
	else{
		updateMarked = false; 
	}

	//		
	//if(updateMarked == false){
	//	continue; 
	//}

	//for(int i = 0; i<4; i++){
	//	if(direction[i])
	//	{
	//		PathCost currentpathcost = this->readPathCost(cellMx,cellMz); 
	//		currentpathcost.updateneighbor = true; 
	//		currentpathcost.neighbor[i] = 1; 
	//		this->write(cellMx, cellMz, currentpathcost); 
	//	}
	//}

}


void NavigationField2D::init_version2(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad)
{
	//std::ifstream inf( "SketchyGrad1.txt");
	//float gx, gz; 
	//if( inf.good() )
	//{	
	//	for(int i = 0; i< UtilHelper::GridDimensionX; i++)
	//	{
	//		for(int j = 0; j< UtilHelper::GridDimensionZ; j++)
	//		{
	//			inf>>gx; 
	//			inf>>gz; 

	//			sketchy_grad.writeSketchyGradientCopy(i,j, SketchyGradient(gx, gz, 0)); 
	//		}
	//	}
	//}
	//else{
	//	//WRITE TO FILE
	//	std::ofstream outf;
	//	outf.open("SketchyGrad1.txt", std::ios::out );
	//	if( !outf.good() )
	//	{
	//		return;
	//	}

	//	float gradientx, gradientz; 
	//	for(int i = 0; i< UtilHelper::GridDimensionX; i++)
	//	{
	//		for(int j = 0; j< UtilHelper::GridDimensionZ; j++)
	//		{
	//			gradientx = sketchy_grad.readSketchyGradientCopy(i,j).gradientx; 
	//			gradientz = sketchy_grad.readSketchyGradientCopy(i,j).gradientz; 

	//			outf << gradientx <<"\n";
	//			outf << gradientz << "\n";
	//		}
	//	}


	//	outf.close();
	//}

	std::vector<State> OpenList; 
	int goalcellx, goalcellz; 
	for(int sid = 0; sid < seedgridcellx.size(); sid++)
	{
		goalcellx = this->seedgridcellx[sid];
	    goalcellz = this->seedgridcellz[sid];

		State goalstate = this->readState(goalcellx, goalcellz); 
		goalstate.cellx = goalcellx; 
		goalstate.cellz = goalcellz; 
		goalstate.goal = true; 
		goalstate.gscost = FLT_MAX; 
		goalstate.rhscost = 0.f; 
		goalstate.consider = true; 
		goalstate.known = true; 
		goalstate.navix = 0.0f; 
		goalstate.naviz = 0.0f; 
		goalstate.updatecount = 0; 
		goalstate.visited = true; 

		OpenList.push_back(goalstate); 

		this->writeState(goalcellx, goalcellz, goalstate);
		sketchy_grad.writeSketchyGradientCopy(goalcellx, goalcellz, SketchyGradient()); 
	}

	this->ComputeShortestPath(cost_navi2D, sketchy_grad, OpenList);
}

StateCandidate NavigationField2D::ComputeCost(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad, const State& cellm, const State& celln1, const State& celln2)
{
	// 1 or infinite big
	int cellMx, cellMz; 
	int cellN1x, cellN1z; 
	int cellN2x, cellN2z; 

	cellMx = cellm.cellx; 
	cellMz = cellm.cellz; 

	cellN1x = celln1.cellx; 
	cellN1z = celln1.cellz; 
	cellN2x = celln2.cellx; 
	cellN2z = celln2.cellz; 

	float T_n1, T_n2, T_m; 
	T_n1 = celln1.gscost; 
	T_n2 = celln2.gscost; 

	CostVector costM = cost_navi2D.readCostMac(cellMx, cellMz); 



	float cMx, cMy;
	cMx = cMy = FLT_MAX; 
	int directionx = -1; 
	int directiony = -1; 
	if(celln1.cellx == cellm.cellx + 1 && celln1.cellz == cellm.cellz){
		//East
		cMx = costM[EAST]; 
		directionx = 0; 
	}
	if(celln1.cellx == cellm.cellx - 1 && celln1.cellz == cellm.cellz){
		//West
		cMx = costM[WEST]; 
		directionx = 1;
	}
	if(celln2.cellx == cellm.cellx && celln2.cellz == cellm.cellz+1){
		//North
		cMy = costM[NORTH]; 
		directiony = 0; 
	}
	if(celln2.cellx == cellm.cellx && celln2.cellz == cellm.cellz-1){
		//South
		cMy = costM[SOUTH]; 
		directiony = 1; 
	}	
	if(celln1.cellx == -1 || celln1.cellz == -1||(T_n2 + cMy == FLT_MAX && T_n1 + cMx ==FLT_MAX)||costM[4]==1 ){
		return StateCandidate(); 
	}

	float GradX, GradY; 			
	GradX = sketchy_grad.readSketchyGradientCopy(cellMx, cellMz).gradientx; 
	GradY = sketchy_grad.readSketchyGradientCopy(cellMx, cellMz).gradientz;  

	if( T_n2 + cMy == FLT_MAX){

		if(directionx == 0)
		{
			float avector[2] = {1, 0}; 
			float dotag = GradX; 
			float gx2 = GradX * GradX + GradY * GradY; 
			float sax = dotag + sqrt(dotag * dotag - gx2 + 1); 

			if(cMx == FLT_MAX){
				return StateCandidate(cMx, 0.0f, 0.0f); 
			}
			else{
				return StateCandidate( T_n1 + 1/sax , 1.0, 0.0); 
			}
		}
		else if(directionx == 1)
		{
			float avector[2] = {1, 0}; 
			float dotag = -GradX; 
			float gx2 = GradX * GradX + GradY * GradY; 
			float sax = dotag + sqrt(dotag * dotag - gx2 + 1); 
			
			if(cMx == FLT_MAX){
				return StateCandidate(cMx, 0.0f, 0.0f); 
			}
			else{
				return StateCandidate( T_n1+ 1/sax, -1.0, 0.0); 
			}
		}
	}

	if(T_n1 + cMx == FLT_MAX){

		if(directiony == 0)
		{
			float avector[2] = {0, 1}; 
			float dotag = GradY; 
			float gx2 = GradX * GradX + GradY * GradY; 
			float sax = dotag + sqrt(dotag * dotag - gx2 + 1); 

						
			if(cMy == FLT_MAX){
				return StateCandidate(cMy, 0.0f, 0.0f); 
			}
			else{
				return StateCandidate( T_n2+ 1/sax, 0.0, 1.0); 
			}
		}
		else if(directiony == 1)
		{
			float avector[2] = {0, 1}; 
			float dotag = -GradY; 
			float gx2 = GradX * GradX + GradY * GradY; 
			float sax = dotag + sqrt(dotag * dotag - gx2 + 1); 

			if(cMy == FLT_MAX){
				return StateCandidate(cMy, 0.0f, 0.0f); 
			}
			else{
				return StateCandidate( T_n2+ 1/sax, 0.0, -1.0); 
			}
		}
	}


	//all four xiang xian will be mapped to xiang xian 1
	bool middlevalue;  
	float avector[2]; 
	float amag = 0.0f; 
	float sax; 

	float M; 
	M = T_n2 - T_n1;  //T y - Tx; 


	GradX = sketchy_grad.readSketchyGradientCopy(cellMx, cellMz).gradientx; 
	GradY = sketchy_grad.readSketchyGradientCopy(cellMx, cellMz).gradientz;  
	//GradX = GradY = 0.0f; 
	//mapping GradX and GradY into xiang xian 1
	if(directionx == 0 && directiony == 0){
		GradX = GradX; 
		GradY = GradY; 
	}
	else if(directionx == 1 && directiony == 0){
		GradX = -GradX; 
		GradY = GradY;
	}
	else if(directionx == 1 && directiony == 1){
		GradX = -GradX; 
		GradY = -GradY;
	}
	else if(directionx == 0 && directiony == 1){
		GradX = GradX; 
		GradY = -GradY;
	}


	float D; 
	D =  (GradX * GradX + GradY * GradY - 1); 

	float gxgx = GradX * GradX + GradY * GradY; 

	float C; 
	float J, K, L; 
	float AA, BB,CC; 
	float Delta; 
	float alpha1,alpha2; 
	float dot_ga; 

	C = (GradX - GradY) - M * D; 
	J = (GradX - GradY) * (GradX - GradY) - 2 * D; 
	K = 2 * GradY * (GradX - GradY) + 2 * D; 
	L = GradY * GradY - D;  

	AA = 4 * J *J - 4 * J * C * C ; 
	BB = 4 * J *K - 4 * K * C * C ; 
	CC = K * K - 4 * C * C * L ; 

	Delta = BB * BB - 4 * AA * CC; 
	if(abs(AA) < 0.00001f)
	{
		//return StateCandidate(); 

		alpha1 = -CC/BB; 
		avector[0] = alpha1; 
		avector[1] = 1 - alpha1; 

		amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);
		avector[0]/=amag;
		avector[1]/=amag;
		dot_ga = avector[0] * GradX + avector[1] * GradY; 
		//if G = 0; sax == 1
		sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
					
		if(sax < 0.00001){
			//perfect 90 degree 
			T_m = POSINFINITE; 
		}
		else{
			T_m = alpha1 * T_n1 + (1 - alpha1) * T_n2 + amag/sax; 
		}
	}//End of AA == 0
	else
	{//exsited alpha squre
		if(Delta < 0)
		{
			//return StateCandidate(); 


			//compare alpha 0 and 1
			if(T_n1 > T_n2 )
			{
				avector[0] = 0.0f; 
				avector[1] = 1.0f;

				float dotag = GradY; 
				float gx2 = GradX * GradX + GradY * GradY; 
				float sax = dotag + sqrt(dotag * dotag - gx2 + 1);

				T_m = T_n2 + 1/sax; 
			}
			else{
				avector[0] = 1.0f; 
				avector[1] = 0.0f;

				float dotag = GradX; 
				float gx2 = GradX * GradX + GradY * GradY; 
				float sax = dotag + sqrt(dotag * dotag - gx2 + 1);

				T_m = T_n1 + 1/sax; 
			}
		}
		else
		{
			Delta = sqrt(Delta);
			alpha1 = (-BB + Delta)/(2 * AA); 	
			alpha2 = (-BB - Delta)/(2 * AA); 

			bool nosolution = false; 
			if((alpha1 > 1 || alpha1 <0) && ( alpha2 > 1 || alpha2 <0))
			{
				nosolution = true; 
			}
			else if((alpha1 > 1 || alpha1 <0))
			{
				float temp; 
				temp = alpha1;
				alpha1 = alpha2;
				alpha2 = temp; 
			}
			if(nosolution){
			

				//compare alpha 0 and 1
				if(T_n1 > T_n2 )
				{
					avector[0] = 0.0f; 
					avector[1] = 1.0f;

									float dotag = GradY; 
				float gx2 = GradX * GradX + GradY * GradY; 
				float sax = dotag + sqrt(dotag * dotag - gx2 + 1);

					T_m = T_n2 + 1/sax; 
				}
				else{
					avector[0] = 1.0f; 
					avector[1] = 0.0f;

					float dotag = GradX; 
				float gx2 = GradX * GradX + GradY * GradY; 
				float sax = dotag + sqrt(dotag * dotag - gx2 + 1);

					T_m = T_n1 + 1/sax; 


				}
			}
			else
			{
				avector[0] = alpha1; 
				avector[1] = 1 - alpha1; 
				amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

				if(amag > 0){
					avector[0]/= amag; 
					avector[1]/= amag;
				}

				dot_ga = avector[0] * GradX + avector[1] * GradY; 
				sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);

				if(sax < 0.00001){
					T_m = POSINFINITE; 
				}
				else{
					T_m = alpha1 * T_n1 + (1 - alpha1) * T_n2 + amag/sax; 
				}
				float avectorback[2] = {avector[0], avector[1]};
				float T_mBack = T_m; 

				if(alpha2 > 0 && alpha2 < 1){
					avector[0] = alpha2; 
					avector[1] = 1 - alpha2; 
					amag = sqrt(avector[0] * avector[0] + avector[1] * avector[1]);

					if(amag > 0){
						avector[0]/= amag; 
						avector[1]/= amag;
					}
					dot_ga = avector[0] * GradX + avector[1] * GradY; 
					sax = dot_ga + sqrt(dot_ga * dot_ga - gxgx + 1);
					if(sax < 0.00001){
						T_m = POSINFINITE; 
					}
					else{
						T_m = alpha2 * T_n1 + (1 - alpha2) * T_n2 + amag/sax; 
					}

					if(T_m > T_mBack){
						avector[0] = avectorback[0]; 
						avector[1] = avectorback[1]; 
						T_m = T_mBack; 
					}
					middlevalue = true; 
				}//end of alpha2 case if possible
			}//end of alpha valid between zero to one
		}//end of delta > 0
	}

	if((T_m - T_n1)*(T_m -T_n2)<0)
	{
		T_m = T_m;
	}

	//mapping avector back to its real xiangxian
	if(directionx == 0 && directiony == 0){
		avector[0] = avector[0]; 
		avector[1] = avector[1]; 
	}
	else if(directionx == 1 && directiony == 0){
		avector[0] = -avector[0]; 
		avector[1] = avector[1]; 
	}
	else if(directionx == 1 && directiony == 1){
		avector[0] = -avector[0]; 
		avector[1] = -avector[1]; 
	}
	else if(directionx == 0 && directiony == 1){
		avector[0] = avector[0]; 
		avector[1] = -avector[1]; 
	}

	StateCandidate temp; 
	temp.navix = avector[0]; 
	temp.naviz = avector[1];
	temp.rhscost = T_m; 
 
	return temp; 
}

void NavigationField2D::UpdateState(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad, State& celli, std::vector<State>& CandidateSet)
{
	if(celli.updatecount>=7){
		return; 
	}

	if(celli.visited == false){
		celli.gscost = POSINFINITE; 
	}
	if(celli.goal!=true){
		//obtain minimum rhs(si)
		StateCandidate statecan[4]; 
		State stateM = celli; 

		State stateE, stateW, stateN, stateS; 

		if(UtilHelper::CellOutofBoundary(celli.cellx + 1, celli.cellz) == false){
			stateE = this->readState(celli.cellx + 1, celli.cellz); 
		}
		if(UtilHelper::CellOutofBoundary(celli.cellx - 1, celli.cellz) == false){
			stateW = this->readState(celli.cellx - 1, celli.cellz); 
		}
		if(UtilHelper::CellOutofBoundary(celli.cellx, celli.cellz + 1) == false){
			stateN = this->readState(celli.cellx, celli.cellz + 1); 
		}
		if(UtilHelper::CellOutofBoundary(celli.cellx, celli.cellz - 1) == false){
			stateS = this->readState(celli.cellx, celli.cellz - 1); 
		}
		

		statecan[0] = this->ComputeCost(cost_navi2D, sketchy_grad, stateM, stateE, stateN); 

		statecan[1] = this->ComputeCost(cost_navi2D, sketchy_grad, stateM, stateW, stateN); 

		statecan[2] = this->ComputeCost(cost_navi2D, sketchy_grad, stateM, stateW, stateS); 

		statecan[3] = this->ComputeCost(cost_navi2D, sketchy_grad, stateM, stateE, stateS); 

		//StateCandidate backstatecan[4] = {statecan[0], statecan[1], statecan[2], statecan[3]}; 

		//for(int i = 0 ; i<4; i++)
		//{
		//	if(statecan[i].navix * statecan[i].naviz == 0 &&celli.navix * celli.naviz !=0)
		//	{
		//		statecan[i].rhscost = FLT_MAX; 
		//	}
		//}

		//if(statecan[0].rhscost == FLT_MAX &&statecan[1].rhscost == FLT_MAX
		//	&&statecan[2].rhscost == FLT_MAX &&statecan[3].rhscost == FLT_MAX)
		//{
		//	statecan[0] = backstatecan[0]; 
		//	statecan[1] = backstatecan[1]; 
		//	statecan[2] = backstatecan[2]; 
		//	statecan[3] = backstatecan[3]; 
		//}


		StateCandidate minstate;  
		minstate.rhscost = FLT_MAX; 
		//for(int i = 0; i<4; i++){
		//	if(statecan[i].rhscost < minstate.rhscost && statecan[i].navix * statecan[i].naviz != 0 ){
		//		minstate = statecan[i]; 
		//	}
		//	else if(statecan[i].rhscost < minstate.rhscost && statecan[i].navix * statecan[i].naviz == 0 ){
		//		if(minstate.navix * minstate.naviz ==0){
		//			minstate = statecan[i]; 
		//		}
		//	}
		//}

		for(int i = 0; i<4; i++){
			if(statecan[i].rhscost < minstate.rhscost){
				minstate = statecan[i]; 
			}
		}

		celli.rhscost = minstate.rhscost; 
		celli.navix = minstate.navix;
		celli.naviz = minstate.naviz;
		celli.visited = true; 
		celli.consider = true; 
		celli.updatecount++; 

		char debugstring[256]; 

		//sprintf( debugstring, "update: cellMx: %d, cellMz:%d, rhs: %f, navix:%f, naviz:%f, update:%d\n", 
		//celli.cellx, celli.cellz, celli.rhscost, 
		//celli.navix, celli.naviz, celli.updatecount);
		//OutputDebugString(debugstring);

	/*	this->writeState(celli.cellx, celli.cellz, celli);*/
	}

	//step3;
	for(std::vector<State>::iterator itr =CandidateSet.begin() ; itr!=CandidateSet.end(); itr++)
	{
		if(itr->cellx == celli.cellx && itr->cellz == celli.cellz)
		{
			CandidateSet.erase(itr); 
			break; 
		}
	}

	if(celli.gscost!= celli.rhscost)
	{
		CandidateSet.push_back(celli);
	}
}

void NavigationField2D::ComputeShortestPath(CostNavigationField2D& cost_navi2D,SketchyInterface& sketchy_grad,
	std::vector<State>& CandidateSet)
{
	char debugstring[256]; 
	while(CandidateSet.size()!=0)
	{
		make_heap(CandidateSet.begin(), CandidateSet.end()); 
		sort_heap(CandidateSet.begin(), CandidateSet.end());  

		State minstate = CandidateSet[0]; 
		int min_cellx, min_cellz; 
		min_cellx = minstate.cellx; 
		min_cellz = minstate.cellz; 

		if(min_cellx == 20 && min_cellz == 16)
		{
			min_cellx = min_cellx; 
		}
		float  Gx = sketchy_grad.readSketchyGradientCopy(min_cellx, min_cellz).gradientx; 
	    float  Gy = sketchy_grad.readSketchyGradientCopy(min_cellx, min_cellz).gradientz;  

		//sprintf( debugstring, "pop: cellMx: %d, cellMz:%d, gs:%f, rhs: %f, navix:%f, naviz:%f, updatect: %d\n", 
		//	min_cellx, min_cellz, minstate.gscost, 
		//	minstate.rhscost, minstate.navix, minstate.naviz, minstate.updatecount);

		//OutputDebugString(debugstring);
		printf("pop: cellMx: %d, cellMz:%d, gs:%f, rhs: %f, navix:%f, naviz:%f, updatect: %d\n", 
			min_cellx, min_cellz, minstate.gscost, 
			minstate.rhscost, minstate.navix, minstate.naviz, minstate.updatecount);


		CandidateSet.erase(CandidateSet.begin());

		if(minstate.gscost > minstate.rhscost){
			minstate.gscost = minstate.rhscost; 
			this->writeState(minstate.cellx, minstate.cellz, minstate);

			//update its neighboring
			State stateE, stateN, stateW,stateS; 
			int cellEx, cellEz, cellNx, cellNz, cellWx, cellWz, cellSx, cellSz; 
			cellEx = min_cellx +1; 
			cellEz = min_cellz; 

			cellNx = min_cellx; 
			cellNz = min_cellz+1; 

			cellWx = min_cellx - 1; 
			cellWz = min_cellz; 

			cellSx = min_cellx; 
			cellSz = min_cellz -1;

			if(UtilHelper::CellOutofBoundary(cellEx, cellEz)== false){
				stateE = this->readState(cellEx, cellEz); 
				stateE.cellx = cellEx; 
				stateE.cellz = cellEz; 
				this->UpdateState(cost_navi2D, sketchy_grad, stateE, CandidateSet);
			}
			if(UtilHelper::CellOutofBoundary(cellNx, cellNz)== false){
				stateN = this->readState(cellNx, cellNz); 
				stateN.cellx = cellNx; 
				stateN.cellz = cellNz; 
				this->UpdateState(cost_navi2D, sketchy_grad, stateN, CandidateSet);
			}
			if(UtilHelper::CellOutofBoundary(cellWx, cellWz)== false){
				stateW = this->readState(cellWx, cellWz); 
				stateW.cellx = cellWx; 
				stateW.cellz = cellWz; 
				this->UpdateState(cost_navi2D, sketchy_grad, stateW, CandidateSet);
			}			
			if(UtilHelper::CellOutofBoundary(cellSx, cellSz)== false){
				stateS = this->readState(cellSx, cellSz); 
				stateS.cellx = cellSx; 
				stateS.cellz = cellSz; 
				this->UpdateState(cost_navi2D, sketchy_grad, stateS, CandidateSet);
			}	
		}
		else if(minstate.gscost <= minstate.rhscost){
			minstate.gscost = FLT_MAX;
			this->writeState(minstate.cellx, minstate.cellz, minstate);
			//update its neighboring AND ITSELF
			State stateM, stateE, stateN, stateW,stateS; 
			int cellEx, cellEz, cellNx, cellNz, cellWx, cellWz, cellSx, cellSz; 
			cellEx = min_cellx +1; 
			cellEz = min_cellz; 

			cellNx = min_cellx; 
			cellNz = min_cellz+1; 

			cellWx = min_cellx - 1; 
			cellWz = min_cellz; 

			cellSx = min_cellx; 
			cellSz = min_cellz -1;


			if(UtilHelper::CellOutofBoundary(cellEx, cellEz)== false){
				stateE = this->readState(cellEx, cellEz); 
				this->UpdateState(cost_navi2D, sketchy_grad, stateE, CandidateSet);
			}
			if(UtilHelper::CellOutofBoundary(cellNx, cellNz)== false){
				stateN = this->readState(cellNx, cellNz); 
				this->UpdateState(cost_navi2D, sketchy_grad, stateN, CandidateSet);
			}
			if(UtilHelper::CellOutofBoundary(cellWx, cellWz)== false){
				stateW = this->readState(cellWx, cellWz); 
				this->UpdateState(cost_navi2D, sketchy_grad, stateW, CandidateSet);
			}			
			if(UtilHelper::CellOutofBoundary(cellSx, cellSz)== false){
				stateS = this->readState(cellSx, cellSz); 
				this->UpdateState(cost_navi2D, sketchy_grad, stateS, CandidateSet);
			}	
			if(UtilHelper::CellOutofBoundary(min_cellx, min_cellz)== false){
				stateM = this->readState(min_cellx, min_cellz); 
				this->UpdateState(cost_navi2D, sketchy_grad, stateM, CandidateSet);
			}
		}

	}
}

void NavigationField2D::Normalize()
{
	if(Normalized==true){

		return; 
	}
	char debugstring[256];
	for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
		for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

			PathCost pathcost = this->readPathCost(cellx, cellz); 
			pathcost.pathcost /= maxpathcost; 
			this->write(cellx, cellz, pathcost); 

			//sprintf( debugstring, "cellx: %d, cellz: %d, phi: %f, eastwest: %d, northsouth: %d\n", cellx, cellz, phivalue.phivalue,
			//	phivalue.eastwest, phivalue.northsouth);
			//OutputDebugString(debugstring);
		}
	}
	Normalized = true;
}

void NavigationField2D::DeNormalize()
{
	if(	Normalized == false){
		return; 
	}
	
	if(Normalized == true){
	for(int cellx = 0; cellx < UtilHelper::GridDimensionX; cellx++){
		for(int cellz = 0; cellz < UtilHelper::GridDimensionZ; cellz++){

		     	PathCost pathcost = this->readPathCost(cellx, cellz); 
				pathcost.pathcost *= maxpathcost; 
				this->write(cellx, cellz, pathcost); 
			}
		}
	}

	Normalized = false; 
}


PathCost NavigationField2D::readPathCost(int px, int py)
{
	return m_navigationfield2D.at(mapping(px, py));
}

State NavigationField2D::readState(int px, int py)
{
	return m_state2D.at(mapping(px, py));
}


void NavigationField2D::setAll(PathCost pathcost)
{
	int pixel_num = Texture2DBase::getGridX() * Texture2DBase::getGridY(); 

	this->m_navigationfield2D.resize(pixel_num, pathcost);

}

void NavigationField2D::write(int px, int py,  PathCost pathcost)
{
	int pixel_num = mapping(px, py); 

	this->m_navigationfield2D.at(pixel_num) = pathcost; 
}

void NavigationField2D::writeState(int px, int py,  State state)
{
	int pixel_num = mapping(px, py); 

	this->m_state2D.at(pixel_num) = state; 
}
