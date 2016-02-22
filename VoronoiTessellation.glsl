layout (binding = 0) readonly buffer position
{
  vec3 Positions[];
} PositionBuffer;

interface VSOutput
{
	vec3 Position; 
	flat int InstanceID; 
};

interface GSOutput
{
	vec3 Position; 
	flat int InstanceID;
}; 

//uniform int num_cone_slices; 
//uniform float radius; 
const float M_PI = 3.14159265358979323846f;
const float height = 1.99f;

ivec4 color_encode_index(const in int instanceID)
{
	ivec4 rgba; 

	rgba.r = ((instanceID >> 16) & 255) << 0;
    rgba.g = ((instanceID >> 8) & 255)  << 0;
    rgba.b = ((instanceID >> 0) & 255)  << 0;
	rgba.a = ((instanceID) & 0)  << 0;
    
	return rgba; 

}
   
shader VSmain(out VSOutput VSout)
{	
	VSout.Position = PositionBuffer.Positions[gl_VertexID];
	VSout.InstanceID = gl_VertexID;
};

shader GSmain(in VSOutput GSin[], out GSOutput GSout)
{
	//center point 
	vec3 v_center = GSin[0].Position; // 0--
	v_center.z = 1- height;

	//num_cone_slices = 126; 
	//radius = 2.0f;

	int num_cone_slices = 126; 
	float radius = 2.0f; 
	
	float angle_incr = 2.0 * M_PI / num_cone_slices; 
	vec3 v_vertex[500];  //500 bigger than num_cone_slices
	for(int i =0 ; i< num_cone_slices; i++)
	{
		v_vertex[i].x = radius * cos(i * angle_incr); 
	//	v_vertex[i].x = v_vertex[i].x / 1600.f; 

        v_vertex[i].y = radius * sin(i * angle_incr); 
	//	v_vertex[i].y = v_vertex[i].y / 900.f; 
		v_vertex[i].z = height - radius;
	}
	v_vertex[num_cone_slices].x = radius; 
	v_vertex[num_cone_slices].y = 0.0f; 
	v_vertex[num_cone_slices].z = height - radius;

	//triangle formation
	int index_1, index_2; 
	vec3 vertex_1, vertex_2; 
	for(int i = 0 ; i < num_cone_slices; i++)
	{
		index_1 = i % num_cone_slices; 
		index_2 = (i+1) %num_cone_slices; 

		vertex_1 = v_vertex[index_1]; 
		vertex_1.xyz += v_center.xyz;
		GSout.Position = vertex_1; 
		GSout.InstanceID = GSin[0].InstanceID; 
		gl_Position = vec4(2*(vertex_1.x-0.5), 2*(vertex_1.y-0.5), vertex_1.z, 1.0); 
		EmitVertex(); 

		vertex_2 = v_vertex[index_2]; 
		vertex_2.xyz += v_center.xyz;
		GSout.Position = vertex_2;
		GSout.InstanceID = GSin[0].InstanceID; 
		gl_Position = vec4(2*(vertex_2.x-0.5), 2*(vertex_2.y-0.5), vertex_2.z, 1.0); 
		EmitVertex(); 

		GSout.Position = v_center; 
		GSout.InstanceID = GSin[0].InstanceID; 
		gl_Position = vec4(2*(v_center.x-0.5), 2*(v_center.y-0.5), v_center.z, 1.0); 
		EmitVertex(); 

		EndPrimitive(); 
	}
}

shader FSmain(in GSOutput FSin, out vec4 FragColor)
{
	 float norm = 1/ 255.f; 
	 FragColor = vec4(color_encode_index(FSin.InstanceID) * norm); 
};

program DrawAgentsTechnique
{
    vs(430) = VSmain();
	gs(430)=GSmain() : in(points), out(triangle_strip, max_vertices = 150);
    fs(430) = FSmain();
};