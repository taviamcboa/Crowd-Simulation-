
layout (local_size_x = 16, local_size_y = 16) in; 
layout (binding = 0, rgba32f) uniform readonly image2D gDensityVelocityImage; 
layout (binding = 1, rgba32f) uniform writeonly image2D gSpeedNESWImage; 


const vec4 ZEROS = vec4(0.0f); 
const vec4 ONES = vec4(1.0f); 
const vec4 INFINITIES = vec4(9999.0f);


const vec4 MINIMUM_SPEED = vec4(0.001f); 
const vec4 MINIMUM_DENSITY = vec4(0.5f); 
const vec4 MAXIMUM_DENSITY = vec4(0.8f); 
const vec4 MAXIMUM_SPEED = vec4(2.0f); 

bool isInside(ivec2 p, ivec2 grid_dimension)
{
	return (p.x >=0 )&&(p.x <grid_dimension.x)&&(p.y >=0 )&&(p.y <grid_dimension.y); 
}


vec4 getValueOrInfinity(ivec2 p, ivec2 grid_dimension)
{
	vec4 v; 

	if(isInside(p, grid_dimension) == false)
	{
		v = INFINITIES; 
	}
	else
	{
		v = imageLoad(gDensityVelocityImage, p); 
	}
	return v; 
}

vec4 getValueOrZero(ivec2 p, ivec2 grid_dimension)
{
	vec4 v; 

	if(isInside(p, grid_dimension) == false)
	{
		v = ZEROS; 
	}
	else
	{
		v = imageLoad(gDensityVelocityImage, p); 
	}
	return v; 
}

vec4 getNESWNeighboringXComponentOrInfinity(ivec2 p, ivec2 grid_dimension)
{
	ivec2 pNorth = p; 
	ivec2 pEast = p; 
	ivec2 pWest = p; 
	ivec2 pSouth = p; 

	pNorth.y +=1; 
	pEast.x +=1; 
	pWest.x -=1; 
	pSouth.y -=1; 

	vec4 neighborsX; 

	neighborsX.x = getValueOrInfinity(pNorth, grid_dimension).x; 
	neighborsX.y = getValueOrInfinity(pEast, grid_dimension).x; 
	neighborsX.z = getValueOrInfinity(pSouth, grid_dimension).x; 
	neighborsX.w = getValueOrInfinity(pWest, grid_dimension).x; 

	return neighborsX; 
}



/**
 * Returns the speed depending on flow speed, density and maximum allowed speed.
 *
 * Density at agent radius is 0.5, at 2 * r it is 0.0.
 * 
 * NOTE: All parameters represent N, E, S, W directions.
 */

vec4 getSpeed( const in vec4   flowSpeed,
               const in vec4   density,
               const in vec4   density_min,
               const in vec4   density_max,
               const in vec4   speed_max)
{
    // NOTE: a height map could be sampled here and a topographical speed computed
    //       In our examples everything is flat and we just use the maximum allowed speed.

    vec4 topographicalSpeed = speed_max;

    vec4 mediumSpeed = topographicalSpeed
                        - ( (density - density_min) / (density_max - density_min) )
                        * (topographicalSpeed - flowSpeed);

    bvec4 isGreaterEqualMaximum = greaterThanEqual(density,density_max);
    bvec4 isLessEqualMinimum    = lessThanEqual(density, density_min);
    
    // Speed in areas with no agents will be maximized even though the flowSpeed is zero.
    // This is because density is zero too. So maximum speed will be chosen.
       
    vec4 speed; 
	//north
	if(isLessEqualMinimum.r)
	{
		speed.r = speed_max.r; 
	}
	else if(isGreaterEqualMaximum.r)
	{
		speed.r = flowSpeed.r; 
	}
	else
	{
		speed.r = mediumSpeed.r; 
	}
	//east
	if(isLessEqualMinimum.g)
	{
		speed.g = speed_max.g; 
	}
	else if(isGreaterEqualMaximum.g)
	{
		speed.g = flowSpeed.g; 
	}
	else
	{
		speed.g = mediumSpeed.g; 
	}
	//south
	if(isLessEqualMinimum.b)
	{
		speed.b = speed_max.b; 
	}
	else if(isGreaterEqualMaximum.b)
	{
		speed.b = flowSpeed.b; 
	}
	else
	{
		speed.b = mediumSpeed.b; 
	}
	//west
	if(isLessEqualMinimum.a)
	{
		speed.a = speed_max.a; 
	}
	else if(isGreaterEqualMaximum.a)
	{
		speed.a = flowSpeed.a; 
	}
	else
	{
		speed.a = mediumSpeed.a; 
	}
    return speed;
}

shader CSmain()
{	
	vec4 texel; 
	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 


	// x component of density is density sum
    vec4 densityNESW = getNESWNeighboringXComponentOrInfinity(p, ivec2(128, 128));


    vec4 flowSpeedNESW;

	ivec2 positionNorth = p;
    ivec2 positionEast  = p;
    ivec2 positionSouth = p;
    ivec2 positionWest  = p;

    positionNorth.y += 1;
    positionEast.x  += 1;
    positionSouth.y -= 1;
    positionWest.x  -= 1;

	flowSpeedNESW.x =  getValueOrZero(positionNorth, ivec2(128, 128)).w;
    flowSpeedNESW.y =  getValueOrZero(positionEast,  ivec2(128, 128)).z;
    flowSpeedNESW.z = -getValueOrZero(positionSouth, ivec2(128, 128)).w;
    flowSpeedNESW.w = -getValueOrZero(positionWest,  ivec2(128, 128)).z;

	flowSpeedNESW = max(flowSpeedNESW, vec4(MINIMUM_SPEED));

	vec4 speedNESW = getSpeed( flowSpeedNESW, densityNESW, MINIMUM_DENSITY, MAXIMUM_DENSITY,MAXIMUM_SPEED ); 
           
	imageStore(gSpeedNESWImage, p, vec4(speedNESW)); 

	//imageStore(gSpeedNESWImage, p, vec4(-densityNESW.ba, 0, 0)); 
}

program SpeedCalculatorTechnique
{
	cs(430) = CSmain(); 
};