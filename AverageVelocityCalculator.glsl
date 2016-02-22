
layout (local_size_x = 16, local_size_y = 16) in; 
layout (binding = 0, rgba32f) uniform image2D gDensityVelocityImage; 
 
shader CSmain()
{	
	vec4 texel; 
	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 

	texel = imageLoad(gDensityVelocityImage, p);
	float densitysum = texel.r; 
	vec2 velocitysum = texel.zw;
	vec2 averageVelocity; 

	if(densitysum!=0)
	{
		averageVelocity = velocitysum/densitysum; 
	}
	else
	{
		averageVelocity = vec2(0.0f); 
	}
	imageStore(gDensityVelocityImage, p, vec4(texel.rg,averageVelocity)); 
}

program AverageVelocityCalculatorTechnique
{
	cs(430) = CSmain(); 
};