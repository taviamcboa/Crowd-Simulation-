struct VSInput
{
    vec4 Position; //a is id
};

interface VSOutput
{
	vec4 GroupID; 
};
   
const vec4 color_group[8] = 
{
      vec4(1,0,0,1),
	  vec4(0,1,0,1), 
	  vec4(0,0,1,1), 
	  vec4(1,1,0,1), 
	  vec4(0,1,1,1), 
	  vec4(1,0,1,1), 
	  vec4(1,1,1,1), 
	  vec4(0.5, 0.2, 0.8, 1)
};

uniform float g_id; 

shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{ 
	VSout.GroupID = vec4(VSin.Position.a); 
    gl_Position = vec4(2*(VSin.Position.x-0.5), 2*(VSin.Position.y-0.5), VSin.Position.z, 1.0); 
	gl_PointSize = ivec2(10.83).r; //5.83
};

shader FSmain(in VSOutput FSin, out vec4 FragColor)
{	
     int groupID = ivec2(FSin.GroupID.r).r;
	 if(groupID != ivec2(g_id).r)
		discard;
	 FragColor = color_group[groupID]; 
};

program DrawStrokeTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};