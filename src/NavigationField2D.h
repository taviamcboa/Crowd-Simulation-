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

#ifndef _NavigationField2D_H_
#define _NavigationField2D_H_

#include <vector>
#include <string>
#include "Texture2DBase.h"
#include "SketchyInterface.h"
#include "math_3d.h"

#include "CostNavigationField2D.h"

struct PathCost{

	bool updateneighbor; 

	int neighbor[4]; 

	float pathcost; 

	int knowstate; 

	int neighborstate; 

	int cellx; 

	int cellz; 

	int updatecount; 

	float naviX; 

	float naviZ; 

	PathCost()
	{
		pathcost = -1;
		knowstate = 0; 
		neighborstate = 0; 
		cellx = -1; 
		cellz = -1; 
		naviX = 0.0f; 
		naviZ = 0.0f;
		updatecount = 0; 
		updateneighbor = false;
		for(int i = 0; i<4; i++){
			neighbor[i] = 0;
		}
	}

	friend bool operator<(const PathCost& a, const PathCost& b)
    {
        return (a.pathcost < b.pathcost);
    }

	friend bool operator>(const PathCost& a, const PathCost& b)
    {
        return (a.pathcost > b.pathcost);
    }


	PathCost(float _pathcost, int _knowstate, int _neighborstate, int _cellx =-1, int _cellz = -1, 
		float _naviX = 0.0f, float _naviZ = 0.0f, int _updatecount = 0)
	{
		pathcost = _pathcost;
		knowstate = _knowstate; 
		neighborstate = _neighborstate; 
		cellx = _cellx; 
		cellz = _cellz; 
		naviX = _naviX; 
		naviZ = _naviZ; 

		updatecount = _updatecount;
		updateneighbor = false; 
		for(int i = 0; i<4; i++){
			neighbor[i] = 0;
		}
	}

	PathCost& operator=(const PathCost& rhs)
	{
		pathcost = rhs.pathcost; 
		knowstate = rhs.knowstate; 
		neighborstate = rhs.neighborstate;
		cellx = rhs.cellx; 
		cellz = rhs.cellz; 
		naviX = rhs.naviX;
		naviZ = rhs.naviZ; 

		updatecount = rhs.updatecount; 
		updateneighbor = rhs.updateneighbor; 
		for(int i = 0; i<4; i++){
			neighbor[i] = rhs.neighbor[i]; 
		}
		return *this;
	};
};

struct State
{
	float gscost; 
	float rhscost; 
	int cellx, cellz; 
	int n1x, n1z; 
	int n2x, n2z; 
	int updatecount; 
	bool known; 
	bool consider; 
	bool visited; 
	float navix, naviz; 
	bool goal; 

	friend bool operator<(const State& a, const State& b)
    {
		float costa = minf(a.gscost, a.rhscost); 
		float costb = minf(b.gscost, b.rhscost); 

        return (costa < costb);
    }

	friend bool operator>(const State& a, const State& b)
    {
		float costa = maxf(a.gscost, a.rhscost); 
		float costb = maxf(b.gscost, b.rhscost); 

        return (costa > costb);
    }

	State(int _cellx =-1, int _cellz = -1, int _n1x = -1, int _n1z = -1, int _n2x = -1, int _n2z = -1, float _gscost = FLT_MAX, float _rhscost = FLT_MAX, bool _goal = false, bool _known = false, bool _consider = false, bool _visited = false, 
		float _navix = 0.0f, float _naviz = 0.0f, int _updatecount = 0)
	{
		gscost = _gscost; 
		rhscost = _rhscost; 
		known = _known; 
		consider = _consider; 
		visited = _visited;
		cellx = _cellx; 
		cellz = _cellz; 
		n1x = _n1x; 
		n1z = _n1z; 
		n2x = _n2x; 
		n2z = _n2z; 
		navix = _navix; 
		naviz = _naviz; 
		updatecount = _updatecount; 
		goal = _goal; 
	}

	State& operator=(const State& rhs)
	{
		gscost = rhs.gscost; 
		rhscost = rhs.rhscost; 
		known = rhs.known; 
		consider = rhs.consider; 
		visited = rhs.visited; 
		cellx = rhs.cellx; 
		cellz = rhs.cellz; 
		n1x = rhs.n1x; 
		n1z = rhs.n1z; 
		n2x = rhs.n2x; 
		n2z = rhs.n2z; 
		navix = rhs.navix; 
		naviz = rhs.naviz; 
		updatecount = rhs.updatecount; 
		goal = rhs.goal; 
		return *this;
	};
}; 

struct StateCandidate{
	float rhscost;
	float navix, naviz; 
	int n1x, n1z; 
	int n2x, n2z; 

	StateCandidate& operator=(const StateCandidate& rhs)
	{
		rhscost = rhs.rhscost; 
		navix = rhs.navix; 
		naviz = rhs.naviz; 

		n1x = rhs.n1x; 
		n1z = rhs.n1z; 
		n2x = rhs.n2x; 
		n2z = rhs.n2z; 
		return *this;
	};

	StateCandidate(float _rhscost = FLT_MAX, float _navix = 0.0f , float _naviz = 0.0f, int _n1x = -1, int _n1z = -1, int _n2x = -1, int _n2z = -1)
	{
		rhscost = _rhscost;
		navix = _navix; 
		naviz = _naviz;  
		n1x = _n1x; 
		n1z = _n1z; 
		n2x = _n2x; 
		n2z = _n2z; 
	}
};


class NavigationField2D : public Texture2DBase
{
public:
	NavigationField2D(int m=0, int n=0);

	void InitializeGoalCell(std::vector<int>& cellx, std::vector<int>& cellz);

	virtual void write(int px, int py, PathCost pathcost);

	void writeState(int px, int py, State state); 

	virtual void clearAll(); // 

	virtual void setAll(PathCost pathcost); 

	virtual PathCost readPathCost(int px, int py);

	State readState(int px, int py); 

//	void init(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad, bool testgrad); 

	void init_version2(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad); 

	void Normalize(); 

	void DeNormalize(); 

protected:
	void GetPathCost(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad, std::vector<NeighboringGrid>& currentNeighboringGrid, /*OUTPUT*/std::vector<PathCost>& unsortedPathCost); 
	
//	void UpdatePathCost(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad, std::vector<NeighboringGrid>& currentNeighboringKnownGrid, std::vector<PathCost>& unsortedPathCost); 

	void UpdateMarkedCell(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad, int direction, int fromcellx, int fromcellz);
	//virtual int mapping(int px, int py); 

	//Compute Cost
	StateCandidate ComputeCost(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad,const State& cellm, const State& celln1, const State& celln2); 
	
	void UpdateState(CostNavigationField2D& cost_navi2D, SketchyInterface& sketchy_grad, State& celli, std::vector<State>& CandidateSet); 

	void ComputeShortestPath(CostNavigationField2D& cost_navi2D,SketchyInterface& sketchy_grad, std::vector<State>& CandidateSet); 

public:
	//if no pointer, no need for destructor
	std::vector<PathCost> m_navigationfield2D; 

	std::vector<State> m_state2D; 

	std::vector<int> seedgridcellx; 
	std::vector<int> seedgridcellz; 

	float maxpathcost;
	float minpathcost; 

	bool Normalized; 
};

#endif // _PhiTexture2D_H_
