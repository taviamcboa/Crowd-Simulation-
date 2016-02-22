#extension GL_ARB_shader_storage_buffer_object : require

struct VSInput
{
    vec3 Position;
	vec4 dir_speed_rad; //dir + speed + rad
};

interface VSOutput
{
	vec3 Position; 
	flat vec3 velocity_rad; 
};

interface GSOutput
{
	smooth vec2 TexCoord;
	flat vec2 Velocity; 
}; 

layout (binding = 0, std430)  buffer position
{
  vec2 Positions[];
} PositionBuffer;

layout (binding = 1, std430)  buffer direction
{
  vec2 Directions[];
} DirectionBuffer;

layout (binding = 2, std430)  buffer speed
{
  float Speeds[];
} SpeedBuffer;

layout (binding = 3, std430)  buffer gidaid
{
  ivec2 IDs[]; 
} iGIDAIDBuffer; 

layout (binding = 4, std430)  buffer aRads
{
  float rads[]; 
} RadiusBuffer; 

uniform sampler2D gAgentSphereMap;     

const float radius = 3.5 * 0.00390625f;  //0.5 * cellsize = 0.5 * 1/128 =

shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{ 
	VSout.Position =  VSin.Position;

	vec2 velocity_out = VSin.dir_speed_rad.xy * VSin.dir_speed_rad.z; 

	VSout.velocity_rad = vec3(velocity_out, VSin.dir_speed_rad.w); 
};

shader GSmain(in VSOutput GSin[], out GSOutput GSout)
{
	vec3 v_center = GSin[0].Position; // 0--1
	float agent_rad = radius; 
	//agent_rad = GSin[0].velocity_rad.z; 

	//vec3 v_texcoord =  GSin[0].TexCoord; 
	vec3 v_quad_vertex; 
	vec3 v_offset = vec3(-agent_rad, -agent_rad, 0 ); 

	v_quad_vertex = v_center + v_offset; 
	GSout.TexCoord = vec2(0.f, 0.f); 
	GSout.Velocity = GSin[0].velocity_rad.xy; 
	gl_Position = vec4(2*(v_quad_vertex.x-0.5), 2*(v_quad_vertex.y-0.5), v_quad_vertex.z, 1.0); 
	//gl_Position = vec4(v_quad_vertex.x, v_quad_vertex.y, v_quad_vertex.z, 1.0); 
	EmitVertex(); 

	v_offset  = vec3(-agent_rad, agent_rad, 0 );
	v_quad_vertex = v_center + v_offset; 
	GSout.TexCoord = vec2(0.f, 1.0f); 
	GSout.Velocity = GSin[0].velocity_rad.xy; 
	gl_Position = vec4(2*(v_quad_vertex.x-0.5), 2*(v_quad_vertex.y-0.5), v_quad_vertex.z, 1.0);
	//gl_Position = vec4(v_quad_vertex.x, v_quad_vertex.y, v_quad_vertex.z, 1.0); 
	EmitVertex(); 

	v_offset  = vec3(agent_rad, -agent_rad, 0 );
	v_quad_vertex = v_center + v_offset; 
	GSout.TexCoord = vec2(1.f, 0.0f); 
	GSout.Velocity = GSin[0].velocity_rad.xy; 
	gl_Position = vec4(2*(v_quad_vertex.x-0.5), 2*(v_quad_vertex.y-0.5), v_quad_vertex.z, 1.0);
	//gl_Position = vec4(v_quad_vertex.x, v_quad_vertex.y, v_quad_vertex.z, 1.0); 
	EmitVertex(); 

	v_offset  = vec3(agent_rad, agent_rad, 0 );
	v_quad_vertex = v_center + v_offset; 
	GSout.TexCoord = vec2(1.f, 1.0f); 
	GSout.Velocity = GSin[0].velocity_rad.xy; 
	gl_Position = vec4(2*(v_quad_vertex.x-0.5), 2*(v_quad_vertex.y-0.5), v_quad_vertex.z, 1.0);
	//gl_Position = vec4(v_quad_vertex.x, v_quad_vertex.y, v_quad_vertex.z, 1.0); 
	EmitVertex(); 

	EndPrimitive(); 
}

shader FSmain(in GSOutput FSin, out vec4 FragColor)
{
	vec4 c = texture(gAgentSphereMap, FSin.TexCoord); 
	float density = c.r * c.a; 
	FragColor = vec4(density, 0, FSin.Velocity);
};

program DensityVelocitySplatterTechnique2
{
    vs(430) = VSmain();
	gs(430) = GSmain() : in(points), out(triangle_strip, max_vertices = 8);
    fs(430) = FSmain();
};

shader VSBuffermain(out VSOutput VSout)
{ 
	int agentID = iGIDAIDBuffer.IDs[gl_VertexID].y;
	int groupID = iGIDAIDBuffer.IDs[gl_VertexID].x;
	float speed = SpeedBuffer.Speeds[agentID].x; 
	float _radius = RadiusBuffer.rads[agentID].x; 
	vec2 _position = PositionBuffer.Positions[agentID].xy;

	VSout.Position.xyz = vec3(_position.xy, 0);

	vec2 direction = DirectionBuffer.Directions[agentID].xy; 

	vec2 velocity_out = direction * speed; 

	VSout.velocity_rad = vec3(velocity_out, _radius); 
};

program DensityVelocitySplatterTechnique
{
    vs(430) = VSBuffermain();
	gs(430) = GSmain() : in(points), out(triangle_strip, max_vertices = 8);
    fs(430) = FSmain();
};