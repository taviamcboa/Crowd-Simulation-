layout (local_size_x = 16, local_size_y = 16) in; 
layout (binding = 0, rgba32f) uniform readonly image2D gPotentialImage; 
layout (binding = 1, rgba32f) uniform writeonly image2D gGradient12;
layout (binding = 2, rgba32f) uniform writeonly image2D gGradient34; 
layout (binding = 3, rgba32f) uniform writeonly imageBuffer gGradientTest; 


const vec4 INFINITIES = vec4(9999.0f);
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


shader CSmain()
{	
	vec4 texel; 

	ivec2 position = ivec2(gl_GlobalInvocationID.xy); 

	ivec2 positionNorth = position;
    ivec2 positionEast  = position;
    ivec2 positionSouth = position;
    ivec2 positionWest  = position;

    positionNorth.y += 1;
    positionEast.x  += 1;
    positionSouth.y -= 1;
    positionWest.x  -= 1;

	vec4 potentialN = getPotentialValueOrInfinity( positionNorth, ivec2(128, 128));
    vec4 potentialS = getPotentialValueOrInfinity( positionSouth, ivec2(128, 128));
    vec4 potentialW = getPotentialValueOrInfinity( positionWest,  ivec2(128, 128));
    vec4 potential  = getPotentialValueOrInfinity( position,      ivec2(128, 128));
    vec4 potentialE = getPotentialValueOrInfinity( positionEast,  ivec2(128, 128));

	//
    // Layers 0 and 1, group 0 and  group 1
    //
    vec4 positiveDirection01 = vec4(potentialE.x, potentialN.x, potentialE.y, potentialN.y);
    vec4 negativeDirection01 = vec4(potentialW.x, potentialS.x, potentialW.y, potentialS.y);
    vec4 center01            = vec4(potential.x,  potential.x,  potential.y,  potential.y);
    
	// Compute gradient against upwind direction (the negative gradient should point along the path to the destination)
	bvec4 isNegativeDirectionSmaller01 = lessThanEqual(negativeDirection01,positiveDirection01 );
	
	
	// Compute forward and backward difference quotient in both x and y
	vec4 forwardDifferenceQuotient01  = positiveDirection01 - center01;
    vec4 backwardDifferenceQuotient01 = center01 - negativeDirection01;
	vec4 gradient01; 
	if(isNegativeDirectionSmaller01.r)
	{
		gradient01.r = backwardDifferenceQuotient01.r; 
	}
	else
	{
		gradient01.r = forwardDifferenceQuotient01.r; 
	}

	if(isNegativeDirectionSmaller01.g)
	{
		gradient01.g = backwardDifferenceQuotient01.g; 
	}
	else
	{
		gradient01.g = forwardDifferenceQuotient01.g; 
	}

	if(isNegativeDirectionSmaller01.b)
	{
		gradient01.b = backwardDifferenceQuotient01.b; 
	}
	else
	{
		gradient01.b = forwardDifferenceQuotient01.b; 
	}
	if(isNegativeDirectionSmaller01.a)
	{
		gradient01.a = backwardDifferenceQuotient01.a; 
	}
	else
	{
		gradient01.a = forwardDifferenceQuotient01.a; 
	}
	
	bvec4 isInfinityPositiveDirection01_mannual = bvec4(potentialE.x == INFINITIES.x, 
														potentialN.x == INFINITIES.x, 
														potentialE.y == INFINITIES.x, 
														potentialN.y == INFINITIES.x); 
	bvec4 isInfinityNegativeDirection01_mannual = bvec4(potentialW.x == INFINITIES.x, 
														potentialS.x == INFINITIES.x, 
														potentialW.y == INFINITIES.x, 
														potentialS.y == INFINITIES.x); 


	bvec4 isInfinityPositiveDirection01 = isinf(vec4(potentialE.x, potentialN.x, potentialE.y, potentialN.y));
    bvec4 isInfinityNegativeDirection01 = isinf(vec4(potentialW.x, potentialS.x, potentialW.y, potentialS.y));
	bvec4 isInfinity01 = isInfinityPositiveDirection01 && isInfinityNegativeDirection01;
	isInfinity01 = isInfinity01 && isInfinityPositiveDirection01_mannual; 
	isInfinity01 = isInfinity01 && isInfinityNegativeDirection01_mannual; 
	// Mark invalid gradient with zeros
    if(isInfinity01.r == true)
	{
		gradient01.r = 0;
	}
	if(isInfinity01.g == true)
	{
		gradient01.g = 0;
	}
	if(isInfinity01.b == true)
	{
		gradient01.b = 0;
	}
	if(isInfinity01.a == true)
	{
		gradient01.a = 0;
	}
	//
    // Group 2 and 3
    //
	vec4 positiveDirection23 = vec4(potentialE.z, potentialN.z, potentialE.w, potentialN.w);
    vec4 negativeDirection23 = vec4(potentialW.z, potentialS.z, potentialW.w, potentialS.w);
    vec4 center23            = vec4(potential.z,  potential.z,  potential.w,  potential.w);

	// Compute gradient against upwind direction (the negative gradient should point along the path to the destination)
    bvec4 isNegativeDirectionSmaller23 = lessThanEqual(negativeDirection23,positiveDirection23 );
	
	
    // Compute forward and backward difference quotient in both x and y  
    vec4 forwardDifferenceQuotient23  = positiveDirection23 - center23;
    vec4 backwardDifferenceQuotient23 = center23 - negativeDirection23;
	vec4 gradient23;
	if(isNegativeDirectionSmaller23.r)
	{
		gradient23.r = backwardDifferenceQuotient23.r; 
	}
	else
	{
		gradient23.r = forwardDifferenceQuotient23.r; 
	}

	if(isNegativeDirectionSmaller23.g)
	{
		gradient23.g = backwardDifferenceQuotient23.g; 
	}
	else
	{
		gradient23.g = forwardDifferenceQuotient23.g; 
	}
	if(isNegativeDirectionSmaller23.b)
	{
		gradient23.b = backwardDifferenceQuotient23.b; 
	}
	else
	{
		gradient23.b = forwardDifferenceQuotient23.b; 
	}
	if(isNegativeDirectionSmaller23.a)
	{
		gradient23.a = backwardDifferenceQuotient23.a; 
	}
	else
	{
		gradient23.a = forwardDifferenceQuotient23.a; 
	}

	bvec4 isInfinityPositiveDirection23_mannual = bvec4(potentialE.z == INFINITIES.x, 
														potentialN.z == INFINITIES.x, 
														potentialE.w == INFINITIES.x, 
														potentialN.w== INFINITIES.x); 
	bvec4 isInfinityNegativeDirection23_mannual = bvec4(potentialW.z == INFINITIES.x, 
														potentialS.z == INFINITIES.x, 
														potentialW.w == INFINITIES.x, 
														potentialS.w == INFINITIES.x); 



	bvec4 isInfinityPositiveDirection23 = isinf(vec4(potentialE.z, potentialN.z, potentialE.w, potentialN.w));
    bvec4 isInfinityNegativeDirection23 = isinf(vec4(potentialW.z, potentialS.z, potentialW.w, potentialS.w));
	bvec4 isInfinity23 = isInfinityPositiveDirection23 && isInfinityNegativeDirection23;
	isInfinity23 = isInfinity23 && isInfinityPositiveDirection23_mannual; 
	isInfinity23 = isInfinity23 && isInfinityNegativeDirection23_mannual; 
	// Mark invalid gradient with zeros
    if(isInfinity23.r == true)
	{
		gradient23.r = 0;
	}
	if(isInfinity23.g == true)
	{
		gradient23.g = 0;
	}
	if(isInfinity23.b == true)
	{
		gradient23.b = 0;
	}
	if(isInfinity23.a == true)
	{
		gradient23.a = 0;
	}
	imageStore(gGradient12, position, gradient01 ); 
	imageStore(gGradient34, position, gradient23 ); 

	int globalLinear = position.x + position.y * 128; 
	imageStore(gGradientTest, globalLinear, gradient23); 
}

program GradientCalculatorTechnique
{
	cs(430) = CSmain(); 
};