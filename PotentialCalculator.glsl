layout (local_size_x = 16, local_size_y = 16) in; 
layout (binding = 0, rgba32f) uniform readonly image2D gCostImage; 
layout (binding = 1, rgba32f) uniform coherent image2D gPotentialImage; 
layout (binding = 2, rgba32f) uniform coherent imageBuffer gPotentialBuffer; 
layout (binding = 3, rgba32f) uniform imageBuffer gNeighboringCostBuffer; 
layout (binding = 4, rgba32f) uniform imageBuffer gNeighboringPotentialBuffer; 
uniform int loopcount;

//layout (binding = 1, r32i) uniform writeonly iimage2D gBaseGroupState; 

#define    EPSILON       0.001f
const vec4 EPSILONS = vec4(EPSILON, EPSILON, EPSILON, EPSILON);

const vec4 INFINITIES = vec4(9999.0f);

shared vec4 local_potential[324]; // 16 * 16 + 4 * 16

bool isInside(ivec2 p, ivec2 grid_dimension)
{
	return (p.x >=0 )&&(p.x <grid_dimension.x)&&(p.y >=0 )&&(p.y <grid_dimension.y); 
}


vec4 getPotentialValueOrInfinity(ivec2 p, ivec2 grid_dimension)
{
	vec4 v; 

	if(isInside(p, grid_dimension) == false)
	{
		v = INFINITIES; 
	}
	else
	{
		v = imageLoad(gPotentialImage, p); 
	}
	return v; 
}

/**
 * "A fast sweeping method..." gives the more complicated solution with the root term as backup solution.
 * TODO maple solve wrong?
 * 
 * Solving the equation (x - a)^+^2 + (x - b)^+^2 = h^2 with Maple gives the simpler solutions c + a or c + b.
 * "A fast sweeping method..." solution for 2d gives nice result.
 * 
 * (x)^+ = max(x, 0)
 */

vec4 solveQuadratic(vec4 potentialX, vec4 potentialY, vec4 cost)
{
    vec4 solution;

    vec4 potentialMin = min(potentialX, potentialY);
    
    vec4 difference = potentialX - potentialY;

    vec4 absoluteDifference = abs(difference);
    
    vec4 costSquared = cost * cost;
    
    vec4 solutionA  = potentialMin + cost;

    vec4 solutionB = ( potentialX + potentialY + sqrt(costSquared + costSquared - difference * difference) ) * 0.5f;
    
    bvec4 isGreaterEqual = greaterThanEqual(absoluteDifference, cost);

	solution.x = (isGreaterEqual.x == true ? solutionA.x : solutionB.x) ; 
	solution.y = (isGreaterEqual.y == true ? solutionA.y : solutionB.y) ; 
	solution.z = (isGreaterEqual.z == true ? solutionA.z : solutionB.z) ; 
	solution.w = (isGreaterEqual.w == true ? solutionA.w : solutionB.w) ; 


    return solution;
}

/**
 * Does it like "A fast sweeping method..."
 */


vec4 computeSimple(vec4 potential,
                     vec4 potentialN, vec4 potentialS, vec4 potentialW, vec4 potentialE,
                     vec4 costN, vec4 costS, vec4 costW, vec4 costE)
{
    bvec4 isLessThanWE = lessThanEqual(potentialW + costW,potentialE + costE ); 

    vec4 potentialX; 

	potentialX.x = (isLessThanWE.x == true ? potentialW.x : potentialE.x) ; 
	potentialX.y = (isLessThanWE.y == true ? potentialW.y : potentialE.y) ; 
	potentialX.z = (isLessThanWE.z == true ? potentialW.z : potentialE.z) ; 
	potentialX.w = (isLessThanWE.w == true ? potentialW.w : potentialE.w) ; 

    vec4 costX;
	costX.x = (isLessThanWE.x == true ? costW.x : costE.x) ; 
	costX.y = (isLessThanWE.y == true ? costW.y : costE.y) ; 
	costX.z = (isLessThanWE.z == true ? costW.z : costE.z) ; 
	costX.w = (isLessThanWE.w == true ? costW.w : costE.w) ; 
	
    bvec4 isLessThanSN = lessThanEqual(potentialS + costS, potentialN + costN);  
	
    
    vec4 potentialY; 
	potentialY.x = (isLessThanSN.x == true ? potentialS.x : potentialN.x) ; 
	potentialY.y = (isLessThanSN.y == true ? potentialS.y : potentialN.y ); 
	potentialY.z = (isLessThanSN.z == true ? potentialS.z : potentialN.z) ; 
	potentialY.w = (isLessThanSN.w == true ? potentialS.w : potentialN.w ); 

    vec4 costY;
	costY.x = (isLessThanSN.x == true ? costS.x : costN.x) ; 
	costY.y = (isLessThanSN.y == true ? costS.y : costN.y) ; 
	costY.z = (isLessThanSN.z == true ? costS.z : costN.z) ; 
	costY.w = (isLessThanSN.w == true ? costS.w : costN.w) ; 
	

    vec4 cost = costX + costY;
    vec4 solution;

    solution = solveQuadratic(potentialX, potentialY, cost);

    // Should be decreasing or same
    if(solution.x > 0)
		solution.x = min(solution.x, potential.x); //
	else
		solution.x = potential.x;

	if(solution.y > 0)
		solution.y = min(solution.y, potential.y); //
	else
		solution.y = potential.y;

	if(solution.z > 0)
		solution.z = min(solution.z, potential.z); //
	else
		solution.z = potential.z;

	if(solution.w > 0)
		solution.w = min(solution.w, potential.w); //
	else
		solution.w = potential.w;

	//if(potentialX.x == INFINITIES.x && potentialY.x == INFINITIES.x)
	//	solution.x = INFINITIES.x; 
	//if(potentialX.y == INFINITIES.y && potentialY.y == INFINITIES.y)
	//	solution.y = INFINITIES.y; 
	//if(potentialX.z == INFINITIES.z && potentialY.z == INFINITIES.z)
	//	solution.z = INFINITIES.z; 
	//if(potentialX.w == INFINITIES.w && potentialY.w == INFINITIES.w)
	//	solution.w = INFINITIES.w; 
    return solution;
}

void largeStep( const   ivec2   Grid_Dimension,
                const   ivec2   Local_Grid_Dimension,
                const   uint    loopCount)
{
	vec4 texel; 
	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 
	int globalX = p.x; 
	int globalY = p.y; 
	int globalLinear = p.x + p.y * 128; 

	vec4 costNESW = imageLoad(gCostImage, p); 
    vec4 costN = vec4(costNESW.x);
    vec4 costE = vec4(costNESW.y);
    vec4 costS = vec4(costNESW.z);
    vec4 costW = vec4(costNESW.w);

	vec4 signalcolor = vec4(0);  

	int localX = int(gl_LocalInvocationID.x);
    int localY = int(gl_LocalInvocationID.y);

	int linarID = int(gl_LocalInvocationIndex.x); 
	uint linearWorkGroupID = gl_WorkGroupID.x + gl_WorkGroupID.y * 8; 

	if(globalLinear <= 32)
	{
		 signalcolor = vec4(1);  
	}
	

    
	//leave new space for border
    int localX2 = localX + 1;
    int localY2 = localY + 1;
    
    int localSize = 16 + 2;

	int localI, localIW, localIE, localIS, localIN; 
	localI = localX2 + localY2 * localSize;
	localIW = localI - 1;
    localIE = localI + 1;
	localIS = localI - localSize;
    localIN = localI + localSize;
   
    vec4 originalPotential = imageLoad(gPotentialImage, p).rgba; 

	//test
	originalPotential = originalPotential.rgba;

	vec4 originalPotentialNorth = imageLoad(gPotentialImage, p + ivec2(0,1)).rgba;
	vec4 originalPotentialSouth = imageLoad(gPotentialImage, p + ivec2(0,-1)).rgba;
	vec4 originalPotentialEast =  imageLoad(gPotentialImage, p + ivec2(1, 0 )).rgba;
	vec4 originalPotentialWest =  imageLoad(gPotentialImage, p + ivec2(-1, 0 )).rgba;

	int localSizeSquared = localSize * localSize;

	local_potential[localI] = originalPotential;

	//
    // Initialize local elements outside work group (local border)
    //
	float testflag = -9.0; 
	if (localX2 == 1)
    {
        vec4 temporary = getPotentialValueOrInfinity(ivec2(globalX - 1, globalY), ivec2(128, 128));
        local_potential[localIW] = temporary;
		testflag = temporary.x; 

		
    }

	if (localX2 == (localSize - 2))
    {
        vec4 temporary = getPotentialValueOrInfinity(ivec2(globalX + 1, globalY), ivec2(128, 128));
        local_potential[localIE] = temporary;
		testflag = temporary.x; 


    }

	if (localY2 == 1)
    {
        vec4 temporary = getPotentialValueOrInfinity(ivec2(globalX, globalY - 1), ivec2(128, 128));
        local_potential[localIS] = temporary;
		testflag = temporary.x; 


    }

	if (localY2 == (localSize - 2))
    {
        vec4 temporary = getPotentialValueOrInfinity(ivec2(globalX, globalY + 1), ivec2(128, 128));
        local_potential[localIN] = temporary;
		testflag = temporary.x; 

		
    }


	//
    // Iterate on local field
    //
    // We need synchronisation. Without it a single work item might work ahead through
    // all iterations without getting any new information.
    //
    // We don't need ping-pong because it is good for fast information propagation
    // to read an updated neighbor early. Therefore the single buffer version can perform
    // equal or better than the ping-pong version, quality wise. We do lose determinism though.
    //

	vec4 solution = originalPotential;

	
    
    vec4 potentialN;
    vec4 potentialS;
    vec4 potentialW;
    vec4 potentialE;
    
    vec4 potential;
    vec4 previousPotential;
    //
    // Unrolling loop into 2 steps.
    //
    // NOTE: When changing unroll factor update class constant for GUI.
    //
	for (int i = 0; i < loopCount; i++)
    {
        // First iteration
        memoryBarrierShared();
        barrier();
        
        potential = solution;
        potentialN = local_potential[localIN];
        potentialS = local_potential[localIS];
        potentialW = local_potential[localIW];
        potentialE = local_potential[localIE];
        
        solution = computeSimple(potential, potentialN, potentialS, potentialW, potentialE, costN, costS, costW, costE);
        
        local_potential[localI] = solution;
 
        // Second iteration
        memoryBarrierShared();
        barrier();
        
        potential = solution;
		previousPotential = solution;
		potentialN = local_potential[localIN];
        potentialS = local_potential[localIS];
        potentialW = local_potential[localIW];
        potentialE = local_potential[localIE];
      
		solution = computeSimple(potential, potentialN, potentialS, potentialW, potentialE, costN, costS, costW, costE);
		local_potential[localI] = solution;

		//imageStore(gPotentialImage, p, solution ); 
    }

	imageStore(gPotentialImage, p, solution ); 
	
   // imageStore(gPotentialBuffer, globalLinear, vec4(p,solution.x, testflag));

   if(gl_LocalInvocationIndex == 0)
   {
	  int storestart = int(linearWorkGroupID) * 324;

	  for(int k = 0 ; k< 324; k++)
	  {
		 imageStore(gPotentialBuffer, storestart + k, vec4(local_potential[k].x, local_potential[k].x,
												local_potential[k].x, local_potential[k].x));
	  }
   }
   vec4 cost3 = vec4(costN.b, costS.b, costW.b, costE.b); 

	imageStore(gNeighboringCostBuffer, globalLinear, solution); 
	imageStore(gNeighboringPotentialBuffer, globalLinear, vec4( 
												local_potential[localIN].x, local_potential[localIS].x,
												local_potential[localIW].x, local_potential[localIE].x
												)); 	
}


shader CSmain() // initialize
{	
	vec4 texel; 
	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 
	vec4 potential_group = INFINITIES; 
	if(p.x >= 0 && p.x < 10 && p.y>=0 && p.y < 10)
	{
		potential_group.z = 0; 
	}
	if(p.x >= 0 && p.x < 10 && p.y>=118 && p.y < 128)
	{
		potential_group.w = 0; 
	}
	if(p.x >= 118 && p.x < 128 && p.y>=118 && p.y < 128)
	{
		potential_group.x = 0; 
	}
	if(p.x >= 118 && p.x < 128 && p.y>=0 && p.y < 10)
	{
		potential_group.y = 0; 
	}
    //potential_group = vec4(1,1,1,1); 
	imageStore(gPotentialImage, p, potential_group); 
}

shader CSmain2() // initialize
{	
	vec4 texel; 
	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 

	largeStep(ivec2(128, 128),
                      ivec2(16, 16),
                      loopcount); 
}

program PotentialInitTechnique
{
	cs(430) = CSmain(); 
};

program PotentialCalTechnique
{
	cs(430) = CSmain2(); 
};