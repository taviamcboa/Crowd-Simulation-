struct VSInput
{
    vec3 Position;
	vec4 agentID;   //gid , aid, radius
};

interface VSOutput
{
	vec3 Position; 
	vec4 agentID; 
};

interface GSOutput
{
	vec4 agentID; 
}; 
   
//readonly uniform imageBuffer gMotionBufferRead; 
const int edges = 12; 
const float agentrad = 0.00390625 * 0.7;//0.004558 * 0.8; //7.2928/1600

const vec4 color_group[6] = 
{
      vec4(1,0,0,1),
	  vec4(0,1,0,1), 
	  vec4(0,0,1,1), 
	  vec4(1,1,0,1), 
	  vec4(1,0,1,1), 
	  vec4(1,0,1,1)
};



shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{ 
	VSout.agentID = VSin.agentID; 

    //gl_Position = vec4(2*(VSin.Position.x-0.5), 2*(VSin.Position.y-0.5), VSin.Position.z, 1.0); 
	VSout.Position =  VSin.Position;
	//gl_PointSize = ivec2(agentrad * 640+1).r; //5.83
	//gl_PointSize = 1; 
};

shader GSmain(in VSOutput GSin[], out GSOutput GSout)
{
	vec3 v_center = GSin[0].Position; // 0--1
	float degree_start = 0.0; 
	float radian_start; 

	float degree_max  = 360;  
	float degree_stride = degree_max / edges; 
	vec2 v_vertex[edges]; 
	for(int i =0 ; i< edges; i++)
	{
		radian_start = radians(degree_start); 

		v_vertex[i].x = cos(radian_start); 
		v_vertex[i].y = sin(radian_start); 

		degree_start += degree_stride; 
	}

	//triangle formation
	int index_1, index_2; 
	vec2 vertex_1, vertex_2; 
	for(int i = 0 ; i < edges; i++)
	{
		index_1 = i % edges; 
		index_2 = (i+1) %edges; 

		vertex_1 = v_vertex[index_1] * agentrad + v_center.xy; 
		GSout.agentID = GSin[0].agentID; 
		gl_Position = vec4(2*(vertex_1.x-0.5), 2*(vertex_1.y-0.5), v_center.z, 1.0); 
		EmitVertex(); 

		vertex_2 = v_vertex[index_2] * agentrad + v_center.xy; 
		GSout.agentID = GSin[0].agentID; 
		gl_Position = vec4(2*(vertex_2.x-0.5), 2*(vertex_2.y-0.5), v_center.z, 1.0); 
		EmitVertex(); 

		GSout.agentID = GSin[0].agentID; 
		gl_Position = vec4(2*(v_center.x-0.5), 2*(v_center.y-0.5), v_center.z, 1.0); 
		EmitVertex(); 

		EndPrimitive(); 
	}
}

shader FSmain(in GSOutput FSin, out vec4 FragColor)
{
	 FragColor = color_group[ivec2(FSin.agentID.r).r]; 
};

program DrawAgentsTechnique
{
    vs(430) = VSmain();
	gs(430)=GSmain() : in(points), out(triangle_strip, max_vertices = 36);
    fs(430) = FSmain();
};