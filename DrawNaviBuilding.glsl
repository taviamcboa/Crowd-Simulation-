layout (binding = 0, std430)  buffer position
{
  vec2 Positions_Center[];
} PositionBuffer;

layout (binding = 0, std430) buffer boundingboxCorner
{
	vec2 Positions_BBCorner[];
} BoundingboxCornerBuffer;

interface VSOutput
{
	vec2 Position_Center; 
	flat int id; 
};

interface VSOutput_Navi
{
	vec2 Position_Coords; 
};

interface GSOutput
{
	flat int NaviBuildingIDs; 

}; 

const vec4 color_building[8] = 
{
      vec4(1,0,0,0),
	  vec4(0,1,0,0), 
	  vec4(0,0,1,0), 
	  vec4(1,1,0,0), 
	  vec4(0,1,1,0), 
	  vec4(1,0,1,0), 
	  vec4(0.4,0.2,0.8,0), 
	  vec4(0.5, 0.2, 0.8, 0)
};

uniform vec2 widthheight; 
uniform bool xaligned; 

uniform sampler2D gEularianMap;  

shader VSmain(out VSOutput VSout)
{ 
	VSout.Position_Center = PositionBuffer.Positions_Center[gl_VertexID].xy;
	VSout.id = gl_VertexID;
};

shader NaviMask_VSmain(out VSOutput_Navi VSout)
{ 
	vec2 position = BoundingboxCornerBuffer.Positions_BBCorner[gl_VertexID].xy; 
	VSout.Position_Coords = position; 
	gl_Position = vec4(2*(position.x-0.5), 2*(position.y-0.5), 0.0, 1.0); 
};

shader GSmain(in VSOutput GSin[], out GSOutput GSout)
{
	vec2 v_center = GSin[0].Position_Center; // 0--1

	float width = widthheight.x; 
	float height = widthheight.y; 

	vec2 v0, v1, v2, v3; 
	v0 = v1 = v2 = v3 = v_center; 

	width = (xaligned == true ? width: height); 
	height = (xaligned == true ? height:widthheight.x); 

	v0.xy += vec2(-0.5 * width, -0.5 * height); 
	v1.xy += vec2(-0.5 * width, 0.5 * height);
	v2.xy += vec2(0.5 * width, 0.5 * height);
	v3.xy += vec2(0.5 * width, -0.5 * height); 
	
	//triangle formation
	//v0, v1, v2
	GSout.NaviBuildingIDs = GSin[0].id; 
	gl_Position = vec4(2*(v0.x-0.5), 2*(v0.y-0.5), 0.0, 1.0); 
	EmitVertex(); 

	GSout.NaviBuildingIDs = GSin[0].id; 
	gl_Position = vec4(2*(v1.x-0.5), 2*(v1.y-0.5), 0.0, 1.0); 
	EmitVertex(); 

	GSout.NaviBuildingIDs = GSin[0].id; 
	gl_Position = vec4(2*(v2.x-0.5), 2*(v2.y-0.5), 0.0, 1.0); 
	EmitVertex(); 

	EndPrimitive(); 

	//v2, v3, v0
	GSout.NaviBuildingIDs = GSin[0].id; 
	gl_Position = vec4(2*(v2.x-0.5), 2*(v2.y-0.5), 0.0, 1.0); 
	EmitVertex(); 

	GSout.NaviBuildingIDs = GSin[0].id; 
	gl_Position = vec4(2*(v3.x-0.5), 2*(v3.y-0.5), 0.0, 1.0); 
	EmitVertex(); 

	GSout.NaviBuildingIDs = GSin[0].id; 
	gl_Position = vec4(2*(v0.x-0.5), 2*(v0.y-0.5), 0.0, 1.0); 
	EmitVertex(); 

	EndPrimitive(); 
}

shader FSmain(in GSOutput FSin, out vec4 FragColor)
{
	// FragColor = color_building[FSin.NaviBuildingIDs]; 
	 FragColor = vec4(0.5f, 0.5f, 0.5f, 0.0);  //ALPHA = 0, MAKE SURE UNCERTAINTY
};

shader NaviMask_FSmain(in VSOutput_Navi FSin, out vec4 FragColor)
{
	 vec4 eularian_color =  texture(gEularianMap, FSin.Position_Coords.xy);  
	 
	 if(eularian_color.rgba != vec4(0,0,0,0))
	 {
		FragColor = vec4(0.0f); 
	 }
	 else
	 {
		FragColor = vec4(1, 0, 0, 0); 
	 }
};

program DrawNaviBuildingTechnique
{
    vs(430) = VSmain();
	gs(430) = GSmain() : in(points), out(triangle_strip, max_vertices = 8);
    fs(430) = FSmain();
};

program DrawNaviMaskTechnique
{
    vs(430) = NaviMask_VSmain();
    fs(430) = NaviMask_FSmain();
};