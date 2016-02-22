struct VSInput
{
    vec3 Position;

	vec3 buildingID;   //gid, b_id, function
};

interface VSOutput
{
	vec3 buildingID; 
};
   
const vec4 color_building[8] = 
{
      vec4(0,0,1,1),
	  vec4(0,1,0,1), 
	  vec4(0,1,1,1), 
	  vec4(1,0,0,1), 
	  vec4(1,0,1,1), 
	  vec4(1,1,0,1), 
	  vec4(0.7,1,1,1), 
	  vec4(0.5, 0.2, 0.8, 1)
};

shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{ 
	VSout.buildingID = VSin.buildingID; 


    gl_Position = vec4(2*(VSin.Position.x-0.5), 2*(VSin.Position.y-0.5), VSin.Position.z, 1.0); 
	//gl_PointSize = ivec2(VSin.buildingID.b * 640+1).r * 20; //5.83
};

shader FSmain(in VSOutput FSin, out vec4 FragColor)
{
	 FragColor = color_building[ivec2(FSin.buildingID.g).r %4]; 
	//FragColor = vec4(0.5f, 0.5f, 0.5f, 1.0); 
};

program DrawBuildingsTechnique
{
    vs(430) = VSmain();
    fs(430) = FSmain();
};