layout (local_size_x = 16, local_size_y = 16) in; 
layout (binding = 0, rgba32f) uniform readonly image2D gGradientImage01; 
layout (binding = 1, rgba32f) uniform readonly image2D gGradientImage23; 
layout (binding = 2, rgba32f) uniform readonly image2D gSpeedImage; 
layout (binding = 3, rgba32f) uniform readonly image2D gPotentialImage; 


const int AGENT_COUNT_PARKED_BASE             =  4;
const float      TURN_RATE_FAST_FACTOR        = 4.0f;
const float      TURN_RATE_FAST_THRESHOLD     = 0.6f;



kernel void move(const  uint         BUFFER_SIDE_LENGTH,
                 read_only image2d_t gradientIn01,
                 read_only image2d_t gradientIn23,
                 global float2*      positions,
                 global float2*      directions,
                 global float*       speeds,
                 global float*       maximumSpeeds,
                 global uint*        ids,
                 global float*       radii,
                 global float*       turnRates,                 
                 global float4*      speedField,
                 const  float        deltaTime,
                 const  int          areaSideLength,
                 global float4*      potentials,
                 const  global int4* entries,
                 const  int          exitMode,
                 global uint*        agentCounts) // 0,1,2,3 groups, 4,5,6,7 parked


shader CSmain()
{	
	vec4 texel; 

	ivec2 p = ivec2(gl_GlobalInvocationID.xy); 

	
}

program AgentAdvectionTechnique
{
	cs(430) = CSmain(); 
};