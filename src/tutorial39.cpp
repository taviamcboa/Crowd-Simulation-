

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <fstream>
#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

#include "engine_common.h"
#include "util.h"
#include "pipeline.h"
#include "camera.h"
#include "texture.h"
//#include "silhouette_technique.h"
//#include "lighting_technique.h"
#include "glut_backend.h"
//#include "mesh.h"
#include "marker_mesh.h"
#include "agent_mesh.h"


//Various Manager
#include "AgentManager.h"
#include "BuildingManager.h"
#include "StrokeManager.h"
#include "NavigationField.h"
#include "SpriteTex.h"

#include "textureData.h" 
#include "textureArrayData.h" 
#include "GuidanceTextureArray.h"

//CPU-->GPU
#include "FullScreenQuad.h"
#include "MarkerGeometry.h"
#include "MotionComputingGeometry.h"
#include "BuildingGeometry.h"
#include "StrokeGeometry.h" 

//Buffer Object
#include "AgentBufferGeometry.h"
#include "MarkerBufferGeometry.h"
#include "NavigationBuildingBufferGeometry.h"


//various Shader Technique
#include "FullQuad_technique.h"
#include "MarkerSampling_technique.h"
#include "MarkerSamplingDebug_technique.h"
#include "MarkerSamplingBufferTechnique.h"
#include "MotionComputing_technique.h"
#include "CollisionAvoidance_technique.h"
#include "DrawAgentsSprite_technique.h"
#include "DrawBuildingSprite_technique.h"
#include "DrawStrokeSprite_technique.h" 
#include "DrawNaviFullScreen_technique.h" 
#include "DrawNaviFullScreenArray_Technique.h" 
#include "DensityVelocitySplatter_technique.h"
#include "FutureDiscomfortSplatterTechnique.h"
#include "AverageVelocityCalculatorTechnique.h"
#include "SpeedCalculatorTechnique.h"
#include "CostCalculatorTechnique.h"
#include "PotentialInitTechnique.h"
#include "PotentialCalculatorTechnique.h"
#include "GradientCalculator_Technique.h"
#include "AgentAdvectionTechnique.h"
#include "DrawAgentsBufferTechnique.h"
#include "DrawMarkerBufferTechnique.h"
#include "CollisionDetectionTechnique.h"
#include "TextureSubTechnique.h" 
#include "UncertaintySmootherTechnique.h"
#include "DrawNaviBuildingTechnique.h" 
#include "DrawNaviMaskTechnique.h" 
#include "MaskNaviGuidTechnique.h"
#include "DrawMaskedNaviGuidTechnique.h" 


//Buffer-Based Technqie assume performance enhancement
//#include "VoronoiTessallationTechnique.h"



#ifndef WIN32
#include "freetypeGL.h"
#endif

#include "imgui.h"
#include "imguiRenderGL.h"
#include <iomanip>
#ifdef WIN32
#	define snprintf _snprintf
#	define putenv _putenv
#endif



#include <limits.h>
#include <assert.h>
#include <time.h>
#include "config.h"
#include "voronoi.h"
#include "marker_map_fbo.h"
#include "agent_map_fbo.h"
#include "marker_sampling_fbo.h"
#include "marker_sampling_buffer_fbo.h" 
#include "motion_computing_fbo.h"
#include "collison_avoidance_fbo.h"
#include "Eularian_map_fbo.h"
#include "Density_Velocity_fbo.h"
#include "Speed_imageObject.h"
#include "Cost_imageObject.h"
#include "Potential_imageObject.h"
#include "Gradient_ImageObject.h"
#include "Uncertainty_imageObject.h" 
#include "MaskNaviGuid_imageObject.h"
#include "NaviMask_fbo.h"

//old Horde3d project class
#include "SketchyInterface.h" 
#include "DiscomfortNavigationField2D.h"
#include "CostNavigationField2D.h"


using namespace std;

#define WINDOW_WIDTH  1600
#define WINDOW_HEIGHT 900
#define MARKER_NUM 300000
#define AGENT_NUM 750
#define UNCERTAINTY_GROUP 1
#define TUNNEL_ENABLE 0
#define READING_PROFILE 0
#define NAVI_WIDTH  64
#define NAVI_HEIGHT 64

typedef std::vector<int> MarkerArray;

struct BuildingEntry
{
	int mousex; 
	int mousey; 
	float width;
	float height; 
	float cellpx; 
	float cellpy; 

	BuildingEntry(int& _mousex, int& _mousey, float& _width, float& _height, float& _cellpx, float& _cellpy)
	{
		mousex = _mousex; 
		mousey = _mousey; 
		width = _width;
		height = _height; 
		cellpx = _cellpx; 
		cellpy = _cellpy; 
	}

	BuildingEntry(const BuildingEntry& _entry)
	{
		mousex = _entry.mousex; 
		mousey = _entry.mousey; 
		width = _entry.width;
		height =_entry.height; 
		cellpx = _entry.cellpx; 
		cellpy = _entry.cellpy; 
	}
};

#ifndef WIN32
Markup sMarkup = { (char*)"Arial", 64, 1, 0, 0.0, 0.0,
                   {.1,1.0,1.0,.5}, {1,1,1,0},
                   0, {1,0,0,1}, 0, {1,0,0,1},
                   0, {0,0,0,1}, 0, {0,0,0,1} };
#endif

#ifdef WIN32
static long long GetCurrentTimeMillis()
{
	return GetTickCount();
}
#else
static long long GetCurrentTimeMillis()
{
	timeval t;
	gettimeofday(&t, NULL);

	long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
	return ret;
}
#endif
class Tutorial39 : public ICallbacks
{
public:

    Tutorial39() 
    {
        m_pGameCamera = NULL;
        m_scale = 0.0f;

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;  
        
        m_frameCount = 0;
        m_fps = 0.0f;

		//GUI
		m_mousex = m_mousey = 0; 
		m_mouseOverMenu = false;
		m_wasMouseOverMenu = false;
	    m_mouseScroll = 0;
	    m_mouseBut = 0;
		m_showMenu = true; 
		m_showTools = true; 
		m_propScroll = 0;

		m_drawUncertainty = false; 
		m_drawFutureDiscomfort = false;
		m_drawCost = false;
		m_drawMask = false; 
		m_drawMaskedNavi = false; 
		m_drawMaskedGuid = false; 

		m_randomMarker = false; 
		m_randomMarkerFinished = false; 

		m_randomAgent = false; 
		m_randomAgentFinished = false; 

		//m_drawAgent = false; 
		m_clinchAgent = false; 
		m_clinchAgentFinished = false; 
		m_drawMarkerSampling = false; 
		m_drawMarkers = false; 
		//m_drawMotionVector = false; 
		//m_drawFinalMotionVector = false; 
		m_launchSimulation = false; 
		m_showVoronoiAgent = false;
		m_showEularianMap = false;
		m_showGuidanceMap = false; 
		m_showNaviMap = false;
		m_renderVoronoiGS = false; 
		for(int i=0; i<4; i++)
		{
			m_showGuidanceMapSub[i] = false; 
			m_showNaviMapSub[i] = false; 
		}

		m_MarkerMesh = NULL; 
		m_FullScreenQuad = NULL; 
		m_AgentManager = NULL; 
		m_BuildingManager = NULL; 
		m_StrokeManager = NULL; 
		m_NavigationField = NULL; 

		m_SketchyInterface = NULL; 
		m_discomfortField2D = NULL; 
		m_costField2D = NULL; 
		m_SpriteTex_cost = NULL; 
		m_SpriteTex_discomfort = NULL; 
		m_SpriteTex = NULL; 

		m_MarkerGeometry = NULL; 
		//m_MotionComputingGeometry = NULL; 
		m_BuildingGeometry = NULL; 
		m_StrokeGeometry = NULL; 
		m_AgentBufferGeometry = NULL; 
		m_MarkerBufferGeometry = NULL; 
		m_NaviBuildBufferGeometry = NULL; 

		//
		m_edit = false; 
		m_editFinish = false; 
		m_edit_Show = false; 
		m_edit_navi = false; 
		m_edit_navi_Finish = false; 
		m_edit_ShowEularian = false; 
		m_edit_ShowDiscomfort = false; 
		m_edit_ShowCostField = false; 

		m_LoadFromFile = false; 

		m_main = true; 
		m_active_navi_group = -1; 
		m_active_shownavigation_group = -1; 
		m_active_showguidance_group = -1; 
		m_active_shownavigation_groupTex = -1; 
		m_sketchyMode = false; 

		m_groupnum = 0; 
		GPUCopy = true; 
		PrintMotionVector = false; 

		m_RegionMin.clear(); 
		m_RegionMax.clear(); 
		m_GoalMin.clear(); 
		m_GoalMax.clear(); 

		m_eularian_dirty = false; 
		m_eularian_dirty_draw = false; 

		Pointer_MarkerSampling = NULL; 
		MarkerAgentStats = NULL; 
		Pointer_MarkerSamplingDebug = NULL; 
		MarkerAgentStatsDebug = NULL; 
		Pointer_MotionSampling = NULL; 
		Pointer_CollisionAvoidance = NULL; 

		for(int i = 0; i< 4; i++)
		{
			Pointer_NavigationField[i] = NULL;  
		}
		Pointer_DensityField = NULL; 
		
		//
		m_pTexture = NULL;
	
}

    ~Tutorial39()
    {
        SAFE_DELETE(m_pGameCamera);
		SAFE_DELETE(m_MarkerMesh);
		SAFE_DELETE(m_AgentMesh);
		SAFE_DELETE(m_FullScreenQuad);
		SAFE_DELETE(m_AgentManager); 
		SAFE_DELETE(m_BuildingManager); 
		SAFE_DELETE(m_StrokeManager);
		SAFE_DELETE(m_NavigationField); 
		SAFE_DELETE(m_MarkerGeometry); 
		//SAFE_DELETE(m_MotionComputingGeometry); 
		SAFE_DELETE(m_BuildingGeometry); 
		SAFE_DELETE(m_StrokeGeometry); 
		SAFE_DELETE(m_AgentBufferGeometry); 
		SAFE_DELETE(m_MarkerBufferGeometry);
		SAFE_DELETE(m_NaviBuildBufferGeometry);

		for(int i = 0; i<m_groupnum; i++){
			delete m_SpriteTex[i]; 
			delete m_SketchyInterface[i]; 
			delete m_navigationField2D[i]; 
			delete m_navigationTex[i]; 
		}
		if(m_SketchyInterface){
			delete[] m_SketchyInterface; 
		}
		if(m_SpriteTex){
			delete[] m_SpriteTex; 
		}
		if(m_navigationField2D){
			delete[] m_navigationField2D; 
		}
		if(m_navigationTex){
			delete[] m_navigationTex; 
		}
		if(m_navigationArrayTex){
			delete m_navigationArrayTex; 
		}
		if(m_guidanceArrayTex)
		{
			delete m_guidanceArrayTex; 
		}
		if(m_costField2D){
			delete m_costField2D; 
		}
		if(m_discomfortField2D){
			delete m_discomfortField2D; 
		}

		for(int i = 0; i<ObstaclePtrList.size(); i++)
		{
			delete ObstaclePtrList[i]; 
		}
		ObstaclePtrList.clear(); 

		m_RegionMin.clear(); 
		m_RegionMax.clear(); 
		m_GoalMin.clear(); 
		m_GoalMax.clear(); 

		delete[] ptr;

		if(Pointer_MarkerSamplingDebug)
			delete[] Pointer_MarkerSamplingDebug; 

		if(Pointer_MarkerSampling)
			delete[] Pointer_MarkerSampling; 

		if(MarkerAgentStats)
			delete[] MarkerAgentStats; 

		if(MarkerAgentStatsDebug)
			delete[] MarkerAgentStatsDebug; 

		if(Pointer_MotionSampling)
			delete[] Pointer_MotionSampling; 

		if(Pointer_CollisionAvoidance)
			delete[] Pointer_CollisionAvoidance; 
		for(int i =0 ;i < m_groupnum; i++)
		{
			if(Pointer_NavigationField[i])
				delete[] Pointer_NavigationField[i]; 
		}

		if(Pointer_DensityField)
			delete[] Pointer_DensityField; 

		SAFE_DELETE(m_pTexture);
    }    

    bool Init()
    {
        Vector3f Pos(0.0f, 2.0f, -7.0f);
        Vector3f Target(0.0f, 0.0f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

		//group1 presetting
		m_RegionMin.push_back(Vector2f(0.05, 0.05)); 
		m_RegionMax.push_back(Vector2f(0.30, 0.30)); 
		m_GoalMin.push_back(Vector2f(0.85, 0.85)); 
		m_GoalMax.push_back(Vector2f(0.99, 0.99)); 

		//group 2 presetting
		m_RegionMin.push_back(Vector2f(0.05, 0.7)); 
		m_RegionMax.push_back(Vector2f(0.30, 0.95)); 
		m_GoalMin.push_back(Vector2f(0.85, 0.05)); 
		m_GoalMax.push_back(Vector2f(0.99, 0.15)); 
		//group 3 presetting
		m_RegionMin.push_back(Vector2f(0.7, 0.7)); 
		m_RegionMax.push_back(Vector2f(0.99, 0.99)); 
		m_GoalMin.push_back(Vector2f(0.05, 0.05)); 
		m_GoalMax.push_back(Vector2f(0.15, 0.15)); 
		//group 4 presetting
		m_RegionMin.push_back(Vector2f(0.7, 0.05)); 
		m_RegionMax.push_back(Vector2f(0.99, 0.30)); 
		m_GoalMin.push_back(Vector2f(0.05, 0.85)); 
		m_GoalMax.push_back(Vector2f(0.15, 0.99)); 
		//group 5 presetting
		m_RegionMin.push_back(Vector2f(0.36, 0.05)); 
		m_RegionMax.push_back(Vector2f(0.62, 0.30)); 
		m_GoalMin.push_back(Vector2f(-1, -1)); 
		m_GoalMax.push_back(Vector2f(-1, -1)); 
		//group 6 presetting
		m_RegionMin.push_back(Vector2f(0.36, 0.7)); 
		m_RegionMax.push_back(Vector2f(0.62, 0.99)); 
		m_GoalMin.push_back(Vector2f(-1, -1)); 
		m_GoalMax.push_back(Vector2f(-1, -1)); 

        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);
		m_MarkerMesh = new MarkerMesh(MARKER_NUM, WINDOW_WIDTH, WINDOW_HEIGHT);
		m_MarkerMesh->InitializeMesh(); 

		m_AgentMesh = new AgentMesh(m_groupnum, 150, WINDOW_WIDTH, WINDOW_HEIGHT);
		m_AgentManager = new AgentManager(WINDOW_WIDTH, WINDOW_HEIGHT); 

		m_BuildingManager = new BuildingManager(WINDOW_WIDTH, WINDOW_HEIGHT); 
		m_NavigationField = new NavigationField(NAVI_WIDTH, NAVI_HEIGHT); 

		m_StrokeManager = new StrokeManager(WINDOW_WIDTH, WINDOW_HEIGHT); 

		//for debuging the texture 2D
		m_FullScreenQuad = new FullScreenQuad(); 
		m_FullScreenQuad->Initialize(); 

		m_MarkerGeometry = new MarkerGeometry(WINDOW_WIDTH, WINDOW_HEIGHT); 
		//m_MotionComputingGeometry  = new MotionComputingGeometry(WINDOW_WIDTH, WINDOW_HEIGHT); 
		m_BuildingGeometry = new BuildingGeometry(WINDOW_WIDTH, WINDOW_HEIGHT); 
		m_StrokeGeometry = new StrokeGeometry(WINDOW_WIDTH, WINDOW_HEIGHT); 
		m_AgentBufferGeometry = new AgentBufferGeometry(WINDOW_WIDTH, WINDOW_HEIGHT); 
		m_MarkerBufferGeometry = new MarkerBufferGeometry(WINDOW_WIDTH, WINDOW_HEIGHT); 
		m_NaviBuildBufferGeometry = new NavigationBuildingBufferGeometry(WINDOW_WIDTH, WINDOW_HEIGHT);  
		m_NaviBuildBufferGeometry->Initialize(0.35, 0.50, true, 1, 11, 3, 7); 

		m_discomfortField2D = new DiscomfortNavigationField2D(UtilHelper::GridDimensionX, 
			UtilHelper::GridDimensionZ); 
		m_SpriteTex_discomfort = new Sprite(m_discomfortField2D, true) ; 

		m_costField2D = new CostNavigationField2D(UtilHelper::GridDimensionX, 
			UtilHelper::GridDimensionZ); 
		m_SpriteTex_cost = new Sprite(m_costField2D, true) ; 

		//render Target
		//marker mapping continuos
		if (!m_markerMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        }
		//agent voronoi mapping continuous
		if (!m_agentMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        }
		////marker mapping discrete
		//if (!m_markerSamplingFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT, AGENT_NUM, MARKER_NUM, true)) {
  //          return false;
  //      }
		if (!m_markerSamplingBufferFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        }
		//if (!m_markerSamplingDebugFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT, AGENT_NUM, MARKER_NUM, true)) {
  //          return false;
  //      }
		////agent: discrete
	 //   if (!m_motionComputingFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
  //          return false;
  //      }
		////agent: discrete
/* 	    if (!m_collisionAvoidanceFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        } */     
		//environment map
		//if(!m_eularianMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT))
		if(!m_eularianMapFBO_DownSampling.Init(128, 128))
		{
			return false;
		}
		if(!m_eularianMapFBO_FullResolution.Init(WINDOW_WIDTH, WINDOW_HEIGHT))
		{
			return false;
		}
		if(!m_densityVelocityFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT))
		{
			return false;
		}
		if(!m_speedIMO.Init(128, 128))
		{
			return false;
		}
		if(!m_costIMO.Init(128, 128))
		{
			return false; 
		}
		if(!m_potentialIMO.Init(128, 128))
		{
			return false; 
		}
		if(!m_gradientIMO12.Init(128, 128))
		{
			return false; 
		}
		if(!m_gradientIMO34.Init(128, 128))
		{
			return false; 
		}
		if(!m_uncertaintyIMO.Init(128, 128))
		{
			return false; 
		}
		if(!m_naviMaskFBO.Init(128, 128))
		{
			return false; 
		}

		if(!m_maskedNaviGuidIMO.Init(128, 128, 4))
		{
			return false; 
		}

		//GPU Technique 
        if (!m_fullQuadTech.Init()) {
            printf("Error initializing the full quad technique\n");
            return false;            
        }
		//Pass1: Technique
		////if (!m_markersamplingTech.Init()) {
  ////          printf("Error initializing the marker sampling technique\n");
  ////          return false;            
  ////      }
		if (!m_markersamplingBufferTech.Init()) {
            printf("Error initializing the marker sampling buffer technique\n");
            return false;            
        }
		//Pass1 Debug: 
		////if (!m_markersamplingDebugTech.Init()) {
  ////          printf("Error initializing the marker sampling debugging technique\n");
  ////          return false;            
  ////      }
		////Pass2: Technique
		//if (!m_motionComputingTech.Init()) {
  //          printf("Error initializing the motion Computing technique\n");
  //          return false;            
  //      }
		////Pass3: Technique
		//if (!m_collsionAvoidingTech.Init()) {
  //          printf("Error initializing the collision avoiding technique\n");
  //          return false;            
  //      }
		//
		//if (!m_drawAgentsTech.Init()) {
  //          printf("Error initializing the draw agents technique\n");
  //          return false;            
  //      }
		if (!m_drawBuildingsTech.Init()) {
            printf("Error initializing the draw buildings technique\n");
            return false;            
        }
		//Guidance Field
		if (!m_drawStrokeTech.Init()) {
            printf("Error initializing the draw strokes technique\n");
            return false;            
        }
		//NavigationTechnqie
		if (!m_drawNaviTech.Init()) {
            printf("Error initializing the draw navi technique\n");
            return false;            
        }
		if (!m_drawNaviArrayTech.Init()) {
            printf("Error initializing the draw navi array technique\n");
            return false;            
        }

		if(!m_splatterDensityVelocityTech.Init())
		{
			printf("Error initializing the density velocity splatter technique\n");
            return false; 
		}

		if(!m_splatterFutureDiscomfortTech.Init())
		{
			printf("Error initializing the future discomfort splatter technique\n");
            return false; 
		}
		//Computer Shader 
		if(!m_averageVelocityCalcTech.Init())
		{
			printf("Error initializing the average velocity calculator technique\n");
            return false; 
		}
		//Computer Shader
		if(!m_speedCalculatorTech.Init())
		{
			printf("Error initializing the speed calculator technique\n");
            return false;
		}
		//Computer Shader
		if(!m_costCalculatorTech.Init())
		{
			printf("Error initializing the speed calculator technique\n");
            return false;
		}
		if(!m_potentialInitTech.Init())
		{
			printf("Error initializing the potential init technique\n");
            return false;
		}
		if(!m_potentialCalculatorTech.Init())
		{
			printf("Error initializing the potential calculator technique\n");
            return false;
		}
		if(!m_gradientCalculatorTech.Init())
		{
			printf("Error initializing the gradient calculator technique\n");
            return false;
		}
		if(!m_agentAdvectionTech.Init())
		{
			printf("Error initializing the agent advection technique\n");
            return false;
		}
		if(!m_drawAgentsBufferTech.Init())
		{
			printf("Error initializing the draw agents technique\n");
            return false;
		}
		if(!m_drawMarkersBufferTech.Init())
		{
			printf("Error initializing the draw markers technique\n");
            return false;
		}
		if(!m_collisionDetectionTech.Init())
		{
			printf("Error initializing the collision detection technique\n");
            return false;
		}
		if(!m_textureSubTech.Init())
		{
			printf("Error initializing the Texture Sub technique\n");
            return false;
		}
		if(!m_uncertaintySmootherTech.Init())
		{
			printf("Error initializing the Uncertainty Smoothering technique\n");
            return false;
		}
		if(!m_drawNaviBuildingTech.Init())
		{
			printf("Error initializing the draw navigation building technique\n");
            return false;
		}
		if(!m_drawNaviMaskTech.Init())
		{
			printf("Error initializing the draw masking technique\n");
            return false;
		}
		if(!m_maskNaviGuidTech.Init())
		{
			printf("Error initializing the mask navi guid technique\n");
            return false;
		}
		if(!m_drawMaskedNaviGuidTech.Init())
		{
			printf("Error initializing the draw masked navi guid technique\n");
            return false;
		}
		/*if(!m_voronoiTessallationTech.Init())
		{
			printf("Error initializing the voronoi tessallation technique\n");
            return false;
		}*/
		m_fullQuadTech.Enable(); 
		m_fullQuadTech.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		m_fullQuadTech.SetDensityImageUnit(1);
		m_fullQuadTech.SetAntiGradientImageUnit(2);
		m_fullQuadTech.SetGuidTexArrayTextureUnit(3); 
		m_fullQuadTech.SetNaviTexArrayTextureUnit(4); 
		m_fullQuadTech.SetDensityVelocityTextureUnit(5); 

		m_fullQuadTech.SetGroupID(0);
		m_fullQuadTech.SetIntegerNaviGuidance(0);
		m_fullQuadTech.SetIntegerGuidance(0); 
		m_fullQuadTech.SetIntegerNavi(0);
		m_fullQuadTech.SetIntegerTextureFlag(0.0f); 

		//////Pass1: 
		//////input: AgentVoronoiTexture(UNIT 0) and Environment Texture (UNIT 1)
		//////determin each
		////m_markersamplingTech.Enable(); 
		////m_markersamplingTech.SetAgentVoronoiTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		////m_markersamplingTech.SetEularianTextureUnit(1);
		////m_markersamplingTech.SetMarkerVoronoiTextureUnit(2); 
		////m_markersamplingTech.SetWriteMarkerIDTextureUnit(3); 

		m_markersamplingBufferTech.Enable();
		m_markersamplingBufferTech.SetAgentVoronoiTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		m_markersamplingBufferTech.SetEularianTextureUnit(1);

		////////Pass1 Debug: 
		//////m_markersamplingDebugTech.Enable(); 
		//////m_markersamplingDebugTech.SetMarkersSamplingTextureUnit(0);
		//////m_markersamplingDebugTech.SetWriteMarkerIDTextureUnit(1); 

		////Pass2:
		////Input: AgentVoronoiTexture
		//m_motionComputingTech.Enable(); 
		//m_motionComputingTech.SetAgentVoronoiTextureUnit(AGENT_VORONOI_TEXTURE_UNIT_INDEX);
  //      m_motionComputingTech.SetMarkerSamplingTextureUnit(MARKER_SAMPLING_TEXTURE_UNIT_INDEX);
		//m_motionComputingTech.SetWriteMotionBufferTextureUnit(MOTION_TEXTURE_WRITE_UNIT_INDEX);
		//m_motionComputingTech.SetNaviTexArrayUnit(MOTION_NAVI_ARRAY_UNIT_INDEX); 
		//m_motionComputingTech.SetDensityTextureUnit(4);
		//m_motionComputingTech.SetAntiGradientTextureUnit(5);
		//m_motionComputingTech.SetGuidTexArrayUnit(6); 
		//m_motionComputingTech.SetPreviousMotionVectorUnit(7);

	//////	m_collsionAvoidingTech.Enable(); 
	//////	m_collsionAvoidingTech.SetAgentVoronoiTextureUnit(FINAL_AGENT_VORONOI_TEXTURE_UNIT_INDEX);
	//////	m_collsionAvoidingTech.SetMotionTextureUnit(FINAL_MOTION_TEXTURE_UNIT_INDEX);
	////////	m_collsionAvoidingTech.SetNaviTexArrayUnit(3);
	//////	m_collsionAvoidingTech.SetWriteFinalMotionBufferTextureUnit(FINAL_MOTION_TEXTURE_WRITE_UNIT_INDEX);
	//////	m_collsionAvoidingTech.SetWriteDensityFieldTextureUnit(FINAL_DENSITY_TEXTURE_WRITE_UNIT_INDEX);
	//////	m_collsionAvoidingTech.SetWriteDensityFieldBufferTextureUnit(FINAL_DENSITY_TEXTURE_BUFFER_WRITE_UNIT_INDEX);
	//////	//m_collsionAvoidingTech.SetReadFinalMotionBufferTextureUnitPrev(4); 

		m_splatterDensityVelocityTech.Enable(); 
		m_splatterDensityVelocityTech.SetAgentSphereTexureUnit(0); 

		m_splatterFutureDiscomfortTech.Enable(); 
		m_splatterFutureDiscomfortTech.SetAgentSphereTexureUnit(0);
		m_splatterFutureDiscomfortTech.SetDiscomfortCoeffUniform(1); 
		m_splatterFutureDiscomfortTech.SetTimeStepUniform(5); 
		m_splatterFutureDiscomfortTech.SetDeltaTimePerStepUniform(0.0056);

		m_averageVelocityCalcTech.Enable(); 
		m_averageVelocityCalcTech.SetDensityVelocityImageUnit(0); 

		m_speedCalculatorTech.Enable(); 
		m_speedCalculatorTech.SetReadDensityVelocityImageUnit(0); 
		m_speedCalculatorTech.SetWriteDensityVelocityImageUnit(1); 

		m_costCalculatorTech.Enable();
		m_costCalculatorTech.SetSpeedReadOnlyImageUnit(0); 
		m_costCalculatorTech.SetCostWriteOnlyImageUnit(1); 
		m_costCalculatorTech.SetEularianMapImageUnit(2); 
		m_costCalculatorTech.SetDensityVelocityReadOnlyImageUnit(3); 
		m_costCalculatorTech.SetWeightDiscomfort(3.0f); 
		m_costCalculatorTech.SetWeightPathLength(1.0f); 
		m_costCalculatorTech.SetWeightTime(1.0f); 
		m_costCalculatorTech.SetPredictable(1); 


		m_potentialInitTech.Enable(); 
		m_potentialInitTech.SetPotentialWriteImageUnit(1); 

		m_potentialCalculatorTech.Enable(); 
		m_potentialCalculatorTech.SetCostReadOnlyImageUnit(0); 
		m_potentialCalculatorTech.SetPotentialReadWriteImageUnit(1); 
		m_potentialCalculatorTech.SetPotentialWriteBufferUnit(2); 
		m_potentialCalculatorTech.SetNeighboringCostWriteBufferUnit(3); 
		m_potentialCalculatorTech.SetNeighboringPotentialWriteBufferUnit(4); 
		m_potentialCalculatorTech.SetLoopCount(10); 

		//
		m_gradientCalculatorTech.Enable(); 
		m_gradientCalculatorTech.SetPotentialReadImageUnit(0); 
		m_gradientCalculatorTech.SetGradient12WriteImageUnit(1); 
		m_gradientCalculatorTech.SetGradient34WriteImageUnit(2); 
		m_gradientCalculatorTech.SetGradientBufferWriteImageUnit(3); 
		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
		//
		m_agentAdvectionTech.Enable(); 
		m_agentAdvectionTech.SetGradient01ImageUnit(0); 
		m_agentAdvectionTech.SetGradient23ImageUnit(1);
		m_agentAdvectionTech.SetSpeedImageUnit(2); 
		m_agentAdvectionTech.SetPotentialImageUnit(3); 
		m_agentAdvectionTech.SetUncertaintyImageUnit(4); 
		m_agentAdvectionTech.SetMaskedGuidanceImageUnit(5);
		m_agentAdvectionTech.SetMarkerSamplingImageUnit(6);
		m_agentAdvectionTech.SetDeltaTimeLocation(0.0056); 
		m_agentAdvectionTech.SetExitMode(2); 

		m_collisionDetectionTech.Enable(); 
		m_collisionDetectionTech.SetAgentVoronoiImageUnit(0); 
		m_collisionDetectionTech.SetMarkerSamplingImageUnit(1); 
		m_collisionDetectionTech.SetEularianMapImageUnit(3); 
		m_collisionDetectionTech.SetGuidanceImageUnit(2);
		m_collisionDetectionTech.SetDeltaTimeUniform(0.0056); 
		m_collisionDetectionTech.SetHasGUIDUniformLocation(TUNNEL_ENABLE);

		m_textureSubTech.Enable(); 
		m_textureSubTech.SetMarkerSamplingTextureUnit1(0); 
		m_textureSubTech.SetMarkerSamplingTextureUnit2(1); 

		m_uncertaintySmootherTech.Enable(); 
		m_uncertaintySmootherTech.SetEularianImageUnit(0);
		m_uncertaintySmootherTech.SetUncertaintyImageUnit(1); 
		
		m_drawAgentsBufferTech.Enable();
		m_drawAgentsBufferTech.SetUncertaintyImageUnit(0); 

		m_drawNaviBuildingTech.Enable(); 
		m_drawNaviBuildingTech.SetWidthHeightUniform(Vector2f(1/64.f, 11/64.f)); 
		m_drawNaviBuildingTech.SetXAlignedUniform(true); 

		m_drawNaviMaskTech.Enable(); 
		m_drawNaviMaskTech.SetEularianTexture(0);

		m_maskNaviGuidTech.Enable(); 
		m_maskNaviGuidTech.SetNaviMaskImageUnit(0); 
		m_maskNaviGuidTech.SetNaviArrayImageUnit(1);
		m_maskNaviGuidTech.SetGuidArrayImageUnit(2); 
		m_maskNaviGuidTech.SetMaskedNaviArrayImageUnit(3); 
		m_maskNaviGuidTech.SetMaskedGuidArrayImageUnit(4); 

		m_drawMaskedNaviGuidTech.Enable(); 
		m_drawMaskedNaviGuidTech.SetMaskedNaviArrayTextureUnit(0); 
		m_drawMaskedNaviGuidTech.SetMaskedGuidArrayTextureUnit(1); 
		m_drawMaskedNaviGuidTech.SetDrawNaviUniform(false);
		m_drawMaskedNaviGuidTech.SetGroupIDUniform(0); 
		//buffer object
		//m_voronoiTessallationTech.Enable(); 
		//m_voronoiTessallationTech.SetNumConeSlices(126); 
		//m_voronoiTessallationTech.SetRadius(2.0f); 

		////m_drawAgentsTech.Enable();  // POINT RENDERING?? 
		m_drawBuildingsTech.Enable();  //QUAD

		m_drawStrokeTech.Enable(); 
		m_drawNaviTech.Enable(); 
		m_drawNaviArrayTech.Enable(); 

		m_pTexture = new Texture(GL_TEXTURE_2D, "agent.png");
		m_pTexture->Load();
       /* if (!m_pTexture->Load()) {
            return false;
        }*/



        //if (!m_LightingTech.Init()) {
        //    printf("Error initializing the lighting technique\n");
        //    return false;
        //}
        //

/*		m_LightingTech.Enable();
        
		m_LightingTech.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		m_LightingTech.SetDirectionalLight(m_directionalLight);
		m_LightingTech.SetMatSpecularIntensity(0.0f);
		m_LightingTech.SetMatSpecularPower(0);   */     

        //if (!m_mesh.LoadMesh("models/box.obj", true)) {
        //    printf("Mesh load failed\n");
        //    return false;            
        //}
        
#ifndef WIN32
        if (!m_fontRenderer.InitFontRenderer()) {
            return false;
        }
#endif        	
        m_glutTime = glutGet(GLUT_ELAPSED_TIME);
        m_startTime = GetCurrentTimeMillis();
        
        return true;
    }

    void Run()
    {
        GLUTBackendRun(this);
    }
         
    virtual void RenderSceneCB()
    {   
        CalcFPS();
        
        m_scale += 0.01f;
               
        m_pGameCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                      
        RenderScene();

		RenderFPS();
        
			glPopAttrib(); // pop back to default state

		RenderGUI();
			// Save the default state
	    glPushAttrib(GL_TEXTURE_BIT); 

        glutSwapBuffers();
	}

    virtual void IdleCB()
    {
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
        m_pGameCamera->OnKeyboard(Key);
    }

    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        switch (Key) {
            case 'q':
                glutLeaveMainLoop();
				imguiRenderGLDestroy();
                break;
        }
    }

    virtual void PassiveMouseCB(int x, int y)
    {
        m_pGameCamera->OnMouse(x, y);
		m_mousex = x; 
		m_mousey = WINDOW_HEIGHT - y; 
		//this->CallBack_StrokeMove(m_mousex, m_mousey); 
    }

	virtual void MouseMoveCB(int x, int y)
    {
       // m_pGameCamera->OnMouse(x, y);
		m_mousex = x; 
		m_mousey =WINDOW_HEIGHT - y; 
		this->CallBack_StrokeMove(m_mousex, m_mousey); 
    }
    
    virtual void MouseCB(int Button, int State, int x, int y)
    {
	//	m_sketchyMode = false; 
		if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
            m_leftMouseButton.IsPressed = (State == GLUT_DOWN);
            m_leftMouseButton.x = x;
            m_leftMouseButton.y = WINDOW_HEIGHT-y;
			//m_mousex = x; 
			//m_mousey = WINDOW_HEIGHT-y; 


			m_mouseBut |= IMGUI_MBUT_LEFT;
        }

		if (Button == GLUT_RIGHT_BUTTON&&State == GLUT_DOWN) {
            m_rightMouseButton.IsPressed = (State == GLUT_DOWN);
            m_rightMouseButton.x = x;
            m_rightMouseButton.y = WINDOW_HEIGHT-y;
			//m_mousex = x; 
			//m_mousey = WINDOW_HEIGHT-y; 

			m_mouseBut |= IMGUI_MBUT_RIGHT;

			ProceedStroke_pressed(m_rightMouseButton.x, m_rightMouseButton.y, false, State); 
        }

		if( Button == GLUT_LEFT_BUTTON && State == GLUT_UP )
		{
			m_mouseBut = 0;
			m_leftMouseButton.x = x;
            m_leftMouseButton.y = WINDOW_HEIGHT-y;

			ProceedSceneEditor(m_leftMouseButton.x, m_leftMouseButton.y, true );
		}
		if( Button == GLUT_RIGHT_BUTTON && State == GLUT_UP )
		{
			m_mouseBut = 0;
			m_rightMouseButton.x = x;
            m_rightMouseButton.y = WINDOW_HEIGHT-y;

			ProceedSceneEditor(m_rightMouseButton.x, m_rightMouseButton.y, false );
			ProceedStroke_released(m_rightMouseButton.x, m_rightMouseButton.y, false, State); 
			//ProceedStroke(m_rightMouseButton.x, m_rightMouseButton.y, false, State); 
		}
    }


private:
           
    void RenderScene()
    {
        //// Render the object as-is
        //m_LightingTech.Enable();
        //                             
        //Pipeline p;
        //p.SetPerspectiveProj(m_persProjInfo);
        //p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());        
        //p.WorldPos(m_boxPos);
        //m_LightingTech.SetWorldMatrix(p.GetWorldTrans());        
        //m_LightingTech.SetWVP(p.GetWVPTrans());        
        //m_mesh.Render();
        
        // Render the object's silhouette
/*        m_silhouetteTech.Enable();
        
        m_silhouetteTech.SetWorldMatrix(p.GetWorldTrans());        
        m_silhouetteTech.SetWVP(p.GetWVPTrans());        
        m_silhouetteTech.SetLightPos(Vector3f(0.0f, 10.0f, 0.0f));
        
        glLineWidth(5.0f);
        
        m_mesh.Render();   */     

		RandomizeMarkerPass(); 

		RandomizeAgentPass(); 

		if(!m_renderVoronoiGS)
			ClinchAgentPass(); 
		/*else
			RenderAgentVoronoi();*/

		RenderPass();

	//	RenderAgents(-1, true); 

		if(this->m_launchSimulation == true){
			int count = 75;
			static int uncertainty_prop = 1; 
			static int uncertainty_outloop = 1; 
			static int iterate_num = 0; 
			UpdateBuildingInfo();
			DrawBuildingToFBO(); 	
			if(TUNNEL_ENABLE)
			{
				ComputeNaviMask(); 
				MaskNaviGuidField();
			}

			if(uncertainty_prop > 0)
			{
				while(uncertainty_outloop >0)
				{
					uncertainty_outloop--; 
					DrawUncertaintyToFBO(iterate_num); 
					iterate_num++; 
				}
				uncertainty_prop = 0; 
			}

			//DrawDiscomfort(); 
			//RenderMarkerSamplingPass();
			RenderMarkerSamplingBufferPass();

			////RenderPass2(); 
			//RenderMotionPass();

			//RenderCollisionAvoidingPass(); 

			SplatterDensityVelocityPassBuffer();

			SplatterFutureDiscomfortPassBuffer(); 

			AverageVelocityCalcPass(); 

			//DrawAverageVelocityPass();

			SpeedCalculatorPass(); 

			CostCalculatorPass();
			//////DrawSpeedPass();
			//DrawCostPass(); 

			PotentialCalcultorCleanPass(); 

			while(count >0)
			{
				count--; 
				PotentialCalculatorPass(); 
			}
			GradientCalculatorPass();

			RandomizeProbability(); 

			AgentMovePass(); 

			ComputeCollisionDetectionPass(); 

			DrawAgentBuffer(); 

			TransferAgentPosFromGPU();

			if(READING_PROFILE == 0){
				StoreSceneProfile();
			}
			

			//DrawGradientPass(); 
			//////
			//DrawPotentialPass(); 
			////DrawAverageVelocity(); 

			////this->RenderDensityField(); 

			//this->ComputeAntiGradientField(); 

			////this->RenderAntiGradientField(); 

			////DrawAgent(); 

			////this->DrawBackground(); 

			////UpdateAgentState(); 
		}
		if(m_drawUncertainty == true)
		{
			RenderUncertainty(); 
		}
		if(m_drawFutureDiscomfort == true)
		{
			RenderFutureDiscomfort(); 
		}
		if(m_drawCost == true)
		{
			DrawCostPass(); 
		}
		if(m_drawMask == true)
		{
			DrawNaviMask();	
		}
		if(m_drawMaskedNavi == true)
		{
			DrawMaskedNavi(0);
		}
		if(m_drawMaskedGuid == true)
		{
			DrawMaskedGuid(0);
		}
		if(m_drawMarkers == true)
		{
			RenderMarkerSampleBuffer(); 
		}
		if(m_drawMarkerSampling == true){
			RenderSampledMarkers(); 
		}
		//if(m_drawMotionVector == true){
		//	RenderPass3(); 
		//}
	 //   if(m_drawFinalMotionVector == true){
		//	RenderPass4(); 
		//}
		if(m_showVoronoiAgent == true){
			this->RenderVoronoiAgents(); 
		}
		/*if(m_showGuidanceMap == true)
		{
			int activegroup; 
			for(int i = 0; i<4; i++)
			{
				if(m_showGuidanceMapSub[i] == true)
				{
					activegroup = i;
					break; 
				}
			}
			RenderGuidanceField(activegroup);
		}
		if(m_showNaviMap == true)
		{
			int activegroup; 
			for(int i = 0; i<4; i++)
			{
				if(m_showNaviMapSub[i] == true)
				{
					activegroup = i;
					break; 
				}
			}
			RenderNavigationField(activegroup);
		}*/

		if(m_edit_Show ){

			DrawSceneConfigurationBuffer(); //in the edit mode
			if(m_LoadFromFile == true && READING_PROFILE)
			{
				std::ifstream inf( "SceneProfile.txt");
				if( inf.good() )
				{	
					int nums; 
					int mx, my;
					float width, height, cellpx, cellpy; 
					inf>>nums;
					for(int i = 0; i< nums; i++)
					{
						inf>>mx; 
						inf>>my; 
						inf>>width; 
						inf>>height;
						inf>>cellpx; 
						inf>>cellpy; 

						m_BuildingManager->AddBuilding(mx, my, width, height, 4, 4); 
						Obstacle *obstacle = new Obstacle(cellpx, cellpy, width * WINDOW_WIDTH * 0.499, height * WINDOW_HEIGHT * 0.499); 
						obstacle->computeAABB(cellpx, cellpy, width * WINDOW_WIDTH * 0.499, height * WINDOW_HEIGHT * 0.499); 
						ObstaclePtrList.push_back(obstacle); 
					}
					CallBack_NewBuilding(); 
					m_eularian_dirty = true; 
					m_LoadFromFile = false; 
				}
				inf.close(); 
			}
		}
		//when stroke is on
		if(m_edit_navi){
			DrawStroke(); //in the edit mode
		}
		//
		if(m_edit == true && (m_active_shownavigation_group != -1
	        || m_active_showguidance_group!= -1))
		{
			if(m_active_shownavigation_group != -1)
				DrawEularianField(m_active_showguidance_group, m_active_shownavigation_group, true);
			if(m_active_showguidance_group != -1)
				DrawEularianField(m_active_showguidance_group, m_active_shownavigation_group, false);
		}
		if(m_edit == true && m_active_shownavigation_groupTex!=-1)
		{
			DrawNaviField(m_active_shownavigation_groupTex);
		}
		if(m_edit == true && m_edit_ShowDiscomfort == true)
		{
			DrawDiscomfortField();
		}
		if(m_edit == true && m_edit_ShowCostField == true)
		{
			DrawCostField();
		}

		//corresponding to the menu in the main menue
		if(m_showEularianMap  == true){
			DrawBuildingfromFBO();
		}
    }        
    void StoreSceneProfile()
	{
		static int drawValid = 1; 
		if(drawValid == 0)
			return; 
		std::ofstream outf;
		outf.open("SceneProfile.txt", std::ios::out );
		if( !outf.good() )
		{
			return;
		}

		for(int i = 0; i< this->m_buildingEntryVector.size(); i++)
		{
			outf << m_buildingEntryVector[i].mousex <<"\n";
			outf << m_buildingEntryVector[i].mousey << "\n";
			outf << m_buildingEntryVector[i].width <<"\n";
			outf << m_buildingEntryVector[i].height <<"\n";
			outf << m_buildingEntryVector[i].cellpx <<"\n";
			outf << m_buildingEntryVector[i].cellpy <<"\n";
		}
		outf.close();
		drawValid = 0; 
	}
    void CalcFPS()
    {
        m_frameCount++;
        
        int time = glutGet( GLUT_ELAPSED_TIME );

        if (time - m_glutTime > 1000) {
            m_fps = (float)m_frameCount * 1000.0f / (time - m_glutTime);
            m_glutTime = time;
            m_frameCount = 0;
        }
    }
        
    void RenderFPS()
    {
        char text[32];
        ZERO_MEM(text);        
        SNPRINTF(text, sizeof(text), "FPS: %.2f", m_fps);
#ifndef WIN32
        m_fontRenderer.RenderText(10, 10, text);        
#endif
    }   

	void RenderGUI()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(0);

		GLdouble proj[16];
		GLdouble model[16];
		GLint view[4];
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetDoublev(GL_MODELVIEW_MATRIX, model);
		glGetIntegerv(GL_VIEWPORT, view);

		GLdouble x, y, z;
		gluUnProject(this->m_mousex, this->m_mousey, 0.0f, model, proj, view, &x, &y, &z);
		//m_rays[0] = (float)x; m_rays[1] = (float)y; m_rays[2] = (float)z;
		gluUnProject(m_mousex, m_mousey, 1.0f, model, proj, view, &x, &y, &z);
		//m_raye[0] = (float)x; m_raye[1] = (float)y; m_raye[2] = (float)z;

		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, view[2], 0, view[3]);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();


		glColor4ub(255,255,255,255);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		m_mouseOverMenu = false;

		imguiBeginFrame( m_mousex,m_mousey,m_mouseBut,m_mouseScroll );

		const char msg[] = 
			"GPU Crowd Simulation with Uncertainty: GLSL 4.4";
		imguiDrawText(1600 * 0.5, view[3]-20, IMGUI_ALIGN_LEFT, msg, imguiRGBA(255,255,255,200));
	
		char msg1[64];
		_snprintf(msg1, 64, "FPS: %.3f, ActiveAgents: %d, MouseX: %.1d, MouseY: %d", m_fps, m_AgentMesh->highest_encoded_agents, m_mousex, m_mousey);
		imguiDrawText(WINDOW_WIDTH*0.5 -70, WINDOW_HEIGHT-35, IMGUI_ALIGN_LEFT, msg1, imguiRGBA(255,255,255,200));

		if (m_main)
		{
			if (imguiBeginScrollArea("Properties", WINDOW_WIDTH-150, 10, 145, WINDOW_HEIGHT - 10, &m_propScroll))
				m_mouseOverMenu = true;
			//imguiSlider("Weight1", &weight1, 0.1f, 20.0f, 0.1f);
			//imguiSlider("Weight2", &weight2, 0.1f, 20.0f, 0.1f);
			//imguiSlider("Weight3", &weight3, 0.0f, 10.0f, 0.1f);

			if (imguiButton("Go to Edit Mode"))
			{
				m_main = false; 
				m_edit = true; 
				m_editFinish = false; 
			}

			if (imguiCheck("GPU Copy", GPUCopy, true)){
				GPUCopy = !GPUCopy;
			}
			if (imguiCheck("PrintMotionVector", PrintMotionVector, true)){
				PrintMotionVector = !PrintMotionVector;
			}

			if (imguiButton("Set Random Markers"))
			{
				m_randomMarker = true; 
				m_randomMarkerFinished = false; 
				this->m_MarkerMesh->InitializeMesh(); 
			}

			if (imguiButton("Create New Agent Group"))
			{
				m_randomAgent = true; 
				m_randomAgentFinished = false;
				m_drawMarkers = false; 
				if(m_groupnum > 3)
				{
					m_AgentMesh->InitializeMesh(150, m_groupnum++); //lagrangian uncertainty
				}
				else{
					m_AgentMesh->InitializeMesh(150, m_groupnum++); 
				}
			}

			//if (imguiButton("Display existed Agents"))
			//{
			//	m_drawAgent = !m_drawAgent; 
			//}	
			if (imguiButton("Clinch Agents"))
			{
				m_clinchAgent = !m_clinchAgent; 
				m_drawMarkers = false; 
				CallBack_ClinchAgents(); 
			}	
			if (imguiButton("Render Agents BY SHADER"))
			{
				m_renderVoronoiGS = !m_renderVoronoiGS; 
			}	

			if (imguiButton("Launch Simulation"))
			{
				m_drawMarkers = false; 
				m_launchSimulation = true; 
			}	
			if (imguiButton("Draw Markers"))
			{
				m_drawUncertainty = false; 
				m_drawFutureDiscomfort = false; 
				m_drawMarkers = true; 
				m_drawMarkerSampling = false; 
				//m_drawMotionVector = false; 
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap= false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}

			if (imguiButton("Draw Future Discomfort"))
			{
				m_drawUncertainty = false; 
				m_drawFutureDiscomfort = true; 
				m_drawCost = false; 
				m_drawMask = false; 
				m_drawMarkers = false; 
				m_drawMarkerSampling = false; 
				/*m_drawMotionVector = false; 
				m_drawFinalMotionVector = false; */
				m_showVoronoiAgent = false; 
				m_showEularianMap= false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}

			if (imguiButton("Draw Uncertainty Field"))
			{
				m_drawUncertainty = true; 
				m_drawFutureDiscomfort = false; 
				m_drawCost = false; 
				m_drawMask = false; 
				m_drawMarkers = false; 
				m_drawMarkerSampling = false; 
/*				m_drawMotionVector = false; 
				m_drawFinalMotionVector = false;*/ 
				m_showVoronoiAgent = false; 
				m_showEularianMap= false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}
	         
			if (imguiButton("Draw Cost"))
			{
				m_drawFutureDiscomfort = false; 
				m_drawCost = true; 
				m_drawMask = false; 
				m_drawMarkers = false; 
				m_drawMarkerSampling = false; 
				//m_drawMotionVector = false; 
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap= false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}
			if (imguiButton("Draw Mask Map"))
			{
				m_drawMaskedNavi = false; 
				m_drawMaskedGuid = false; 
				m_drawMask = true; 
				m_drawFutureDiscomfort = false; 
				m_drawCost = false; 
				m_drawMarkers = false; 
				m_drawMarkerSampling = false; 
				//m_drawMotionVector = false; 
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap= false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}
			if (imguiButton("Draw Masked Navi"))
			{
				m_drawMaskedNavi = true; 
				m_drawMaskedGuid = false; 
				m_drawMask = false; 
				m_drawFutureDiscomfort = false; 
				m_drawCost = false; 
				m_drawMarkers = false; 
				m_drawMarkerSampling = false; 
				//m_drawMotionVector = false; 
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap= false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}
			if (imguiButton("Draw Masked Guid"))
			{
				m_drawMaskedGuid = true; 
				m_drawMaskedNavi = false; 
				m_drawMask = false; 
				m_drawFutureDiscomfort = false; 
				m_drawCost = false; 
				m_drawMarkers = false; 
				m_drawMarkerSampling = false; 
				//m_drawMotionVector = false; 
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap= false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}
			if (imguiButton("Marker Samling Pass"))
			{
				m_drawMarkerSampling = true; 
				m_drawMarkers = false; 
				//m_drawMotionVector = false; 
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap= false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}	
			if (imguiButton("Motion Vector Pass"))
			{
				m_drawMarkerSampling = false; 
				m_drawMarkers = false; 
				//m_drawMotionVector = true; 
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap = false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}	
			if (imguiButton("Final Motion Vector Pass"))
			{
				m_drawMarkerSampling = false; 
				m_drawMarkers = false; 
				//m_drawMotionVector = false;
				//m_drawFinalMotionVector = true; 
				m_showVoronoiAgent= false; 
				m_showEularianMap = false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}	
			if (imguiButton("Show Voronoi Agents"))
			{
				m_drawMarkerSampling = false; 
				//m_drawMotionVector = false;
				m_drawMarkers = false; 

				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = true; 
				m_showEularianMap = false; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}	
			if (imguiButton("Show Eularian Map"))
			{
				m_drawMarkers = false; 
				m_drawMarkerSampling = false; 
				//m_drawMotionVector = false;
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap = true; 
				m_showGuidanceMap = false; 
				m_showNaviMap = false; 
				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
			}	
			if (imguiButton("Show Guidance Map"))
			{
				m_drawMarkers = false; 
				m_drawMarkerSampling = false; 
				//m_drawMotionVector = false;
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap = false;
				m_showGuidanceMap = true;
				m_showNaviMap = false; 

				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
				m_showGuidanceMapSub[0] = true; 
			}
			if (imguiButton("Show Navigation Map"))
			{
				m_drawMarkers = false; 
				m_drawMarkerSampling = false; 
				//m_drawMotionVector = false;
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap = false;
				m_showGuidanceMap = false;
				m_showNaviMap = true; 

				for(int i =0; i<4; i++)
				{
					m_showNaviMapSub[i] = false; 
					m_showGuidanceMapSub[i] = false; 
				}
				m_showNaviMapSub[0] = true; 
			}
			if(m_showGuidanceMap)
			{
				if (imguiButton("Show Guidance Group 1 ")){
					m_showGuidanceMapSub[0] = true; 
					m_showGuidanceMapSub[1] = false; 
					m_showGuidanceMapSub[2] = false; 
					m_showGuidanceMapSub[3] = false; 
				}
				if (imguiButton("Show Guidance Group 2 ")){
					m_showGuidanceMapSub[0] = false; 
					m_showGuidanceMapSub[1] = true; 
					m_showGuidanceMapSub[2] = false; 
					m_showGuidanceMapSub[3] = false; 
				}
				if (imguiButton("Show Guidance Group 3 ")){
					m_showGuidanceMapSub[0] = false; 
					m_showGuidanceMapSub[1] = false; 
					m_showGuidanceMapSub[2] = true; 
					m_showGuidanceMapSub[3] = false; 
				}
				if (imguiButton("Show Guidance Group 4 ")){
					m_showGuidanceMapSub[0] = false; 
					m_showGuidanceMapSub[1] = false; 
					m_showGuidanceMapSub[2] = false; 
					m_showGuidanceMapSub[3] = true; 
				}
			}
			if(m_showNaviMap)
			{
				if (imguiButton("Show Navigation Group 1 ")){
					m_showNaviMapSub[0] = true; 
					m_showNaviMapSub[1] = false; 
					m_showNaviMapSub[2] = false; 
					m_showNaviMapSub[3] = false; 
				}
				if (imguiButton("Show Navigation Group 2 ")){
					m_showNaviMapSub[0] = false; 
					m_showNaviMapSub[1] = true; 
					m_showNaviMapSub[2] = false; 
					m_showNaviMapSub[3] = false; 
				}
				if (imguiButton("Show Navigation Group 3 ")){
					m_showNaviMapSub[0] = false; 
					m_showNaviMapSub[1] = false; 
					m_showNaviMapSub[2] = true; 
					m_showNaviMapSub[3] = false; 
				}
				if (imguiButton("Show Navigation Group 4 ")){
					m_showNaviMapSub[0] = false; 
					m_showNaviMapSub[1] = false; 
					m_showNaviMapSub[2] = false; 
					m_showNaviMapSub[3] = true; 
				}
			}

			if (imguiButton("Show Simulation Result"))
			{
				m_drawMarkerSampling = false; 
				//m_drawMotionVector = false;
				//m_drawFinalMotionVector = false; 
				m_showVoronoiAgent = false; 
				m_showEularianMap = false; 
			}	

			imguiEndScrollArea();
		}

		if (m_edit == true && m_editFinish == false)
		{
			if(m_edit_navi == false){
				if (imguiBeginScrollArea("Edit Scene", WINDOW_WIDTH-150, 10, 145, WINDOW_HEIGHT - 10, &m_propScroll))
					m_mouseOverMenu = true;

				//imguiSlider("Weight1", &weight1, 0.1f, 20.0f, 0.1f);
				//imguiSlider("Weight2", &weight2, 0.1f, 20.0f, 0.1f);
				//imguiSlider("Weight3", &weight3, 0.0f, 10.0f, 0.1f);

				//if (imguiCheck("GPU Copy", GPUCopy, true)){
				//	GPUCopy = !GPUCopy;
				//}

				if (imguiButton("Show Scene Configuration"))
				{
					m_edit_Show = !m_edit_Show; 
				}
				if(m_edit_Show)
				{
					if (imguiCheck("Load Scene From File", m_LoadFromFile, true)){
							m_LoadFromFile = !m_LoadFromFile;
					}
				}

				if (imguiButton("Edit Navigation Field"))
				{
					m_edit_navi = true; 
				}

				if(m_edit_navi_Finish == true)
				{
					//m_active_shownavigation_group = -1; 
					//m_active_showguidance_group = -1; 
					if (imguiButton("Show Eularian Fields"))
					{
						m_edit_ShowEularian = !m_edit_ShowEularian; 
					}
					if(m_edit_ShowEularian)
					{
						if (imguiCheck("Show Discomfort", m_edit_ShowDiscomfort))
						{
							m_edit_ShowDiscomfort = !m_edit_ShowDiscomfort;
							if(m_edit_ShowDiscomfort  == false){
								
							}

							if(m_edit_ShowDiscomfort == true){
								for(int k =0 ; k< m_edit_Show_Guidance.size(); k++){
									m_edit_Show_Guidance[k] = false;
								}
								for(int k =0 ; k< m_edit_Show_Navigation.size(); k++){
									m_edit_Show_Navigation[k] = false;
									m_edit_Show_NavigationTex[k] = false;
								}

								m_edit_ShowCostField = false; 

								m_active_showguidance_group = -1; 
								m_active_shownavigation_group = -1;
								m_active_shownavigation_groupTex = -1; 
							}
						}

						if (imguiCheck("Show Cost Field", m_edit_ShowCostField))
						{
							m_edit_ShowCostField = !m_edit_ShowCostField;
							if(m_edit_ShowCostField  == false){
								
							}

							if(m_edit_ShowCostField == true){
								for(int k =0 ; k< m_edit_Show_Guidance.size(); k++){
									m_edit_Show_Guidance[k] = false;
								}
								for(int k =0 ; k< m_edit_Show_Navigation.size(); k++){
									m_edit_Show_Navigation[k] = false;
									m_edit_Show_NavigationTex[k] = false;
								}
								m_edit_ShowDiscomfort  = false;

								m_active_showguidance_group = -1; 
								m_active_shownavigation_group = -1;
								m_active_shownavigation_groupTex = -1; 
							}
						}

						for(int i =0 ; i< m_edit_Show_Guidance.size(); i++)
						{
							char name[64];
							sprintf( name, "Show Guidance_%d\n", i);
							if (imguiCheck(name, m_edit_Show_Guidance[i]))
							{
								m_edit_Show_Guidance[i] = !m_edit_Show_Guidance[i];
								if(m_edit_Show_Guidance[i]  == false){
									m_active_showguidance_group = -1; 
								}

								if(m_edit_Show_Guidance[i] == true){
									for(int k =0 ; k< m_edit_Show_Guidance.size(); k++){
										m_edit_Show_Guidance[k] = false;
									}
									for(int k =0 ; k< m_edit_Show_Navigation.size(); k++){
										m_edit_Show_Navigation[k] = false;
										m_edit_Show_NavigationTex[k] = false;
									}
	                                m_active_shownavigation_group = -1; 
									m_active_shownavigation_groupTex = -1; 

									m_edit_Show_Guidance[i]  = true;
									m_active_showguidance_group = i; 
								}
							}
							sprintf( name, "Show Navi_%d\n", i);
							if (imguiCheck(name, m_edit_Show_Navigation[i]))
							{
								m_edit_Show_Navigation[i] = !m_edit_Show_Navigation[i];
								if(m_edit_Show_Navigation[i]  == false){
									m_active_shownavigation_group = -1; 
								}

								if(m_edit_Show_Navigation[i] == true){
									for(int k =0 ; k< m_edit_Show_Guidance.size(); k++){
										m_edit_Show_Guidance[k] = false;
									}
									for(int k =0 ; k< m_edit_Show_Navigation.size(); k++){
										m_edit_Show_Navigation[k] = false;
										m_edit_Show_NavigationTex[k] = false;
									}
									m_active_showguidance_group = -1; 
									m_active_shownavigation_groupTex = -1;
									m_edit_Show_Navigation[i]  = true;
									m_active_shownavigation_group = i; 
								}
							}
							sprintf( name, "Show NaviTex_%d\n", i);
							if (imguiCheck(name, m_edit_Show_NavigationTex[i]))
							{
								m_edit_Show_NavigationTex[i] = !m_edit_Show_NavigationTex[i];
								if(m_edit_Show_NavigationTex[i]  == false){
									m_active_shownavigation_groupTex = -1; 
								}

								if(m_edit_Show_NavigationTex[i] == true){
									for(int k =0 ; k< m_edit_Show_Guidance.size(); k++){
										m_edit_Show_Guidance[k] = false;
									}
									for(int k =0 ; k< m_edit_Show_Navigation.size(); k++){
										m_edit_Show_Navigation[k] = false;
										m_edit_Show_NavigationTex[k] = false;
									}
									m_active_showguidance_group = -1; 
									m_active_shownavigation_groupTex = -1;

									m_edit_Show_NavigationTex[i]  = true;
									m_active_shownavigation_groupTex = i; 
								}
							}
						}
					}
				}

				if (imguiButton("Finish Edit"))
				{
					m_editFinish = true; 
					m_edit_navi = false; 
					m_edit = false; 
					m_main = true; 
					m_edit_Show = false; 
					if(m_eularian_dirty){
						m_eularian_dirty = false; 
						m_eularian_dirty_draw = true; 
					}
				}

				imguiEndScrollArea();
			}
			else{
				//navigation edit mode
				if (imguiBeginScrollArea("Edit Navigation Field", WINDOW_WIDTH-150, 10, 145, WINDOW_HEIGHT - 10, &m_propScroll))
					m_mouseOverMenu = true;

				for(int i =0 ; i< m_edit_navi_group.size(); i++)
				{
					char name[64];
					sprintf( name, "Stroke Navi G_%d\n", i);
					if (imguiCheck(name, m_edit_navi_group[i]))
					{
						m_edit_navi_group[i] = !m_edit_navi_group[i];

						if(m_edit_navi_group[i] == true){
							for(int k =0 ; k< m_edit_navi_group.size(); k++){
								m_edit_navi_group[k] = false;
							}
							m_edit_navi_group[i]  = true;
							m_active_navi_group = i; 
						}
					}
				}
				this->m_drawStrokeTech.SetActiveGroupFloat(m_active_navi_group);

				if (imguiButton("Finish Edit Guidance Field"))
				{
					m_edit_navi = false; 
					m_edit_navi_Finish = true; 
					CallBack_FinishGuidanceField(); 
					Update_NavigationField(); 
					Bake_NavigationTex(); 
				}
				imguiEndScrollArea();
			}
		}

	    m_wasMouseOverMenu = m_mouseOverMenu;
		if(!m_wasMouseOverMenu)
		{
			// In case we move a GUI slider and leave the menu area
			m_mouseBut = 0;
		}

		m_mouseScroll = 0;
		imguiEndFrame();
		imguiRenderGLDraw();


		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	void RandomizeMarkerPass()
	{
		printf("start_RandomizeMarkerPass\n"); 
		if(this->m_randomMarker == false)
			return; 
		if(this->m_randomMarkerFinished == true)
			return; 

		glUseProgram(0);

		m_markerMapFBO.BindForWriting();

		m_MarkerMesh->Render(); //draw cone

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_markerMapFBO.getFBO());
		
		m_MarkerMesh->Read(); //fetch the centroid of each cell

		//m_markerMapFBO's data could be overwrite at this point
	    //m_markerMapFBO.BindForWriting();
		//m_MarkerMesh->DrawDebug(); 

		m_MarkerGeometry->Initialize(*m_MarkerMesh, true); 
		m_MarkerBufferGeometry->Initialize(*m_MarkerMesh, true);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

		this->m_randomMarkerFinished = true; 
		printf("end_RandomizeMarkerPass\n"); 
	}
	void RandomizeAgentPass()
	{
		static int time = 0; 
		printf("start_RandomizeAgentPass\n"); 
		if(this->m_randomAgent == false)
			return; 
		if(this->m_randomAgentFinished == true)
			return; 

		glUseProgram(0);

		//could be overwriting
		m_agentMapFBO.BindForWriting();

		m_AgentMesh->Render();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_agentMapFBO.getFBO());
		
		m_AgentMesh->Read();

	    m_agentMapFBO.BindForWriting();

		m_AgentMesh->DrawDebug(true, true); 

		float smin, smax; 
	
		switch(time)
		{
		case 0: 
			smin = 2.0; 
			smax = 2.0; 
			break;
		case 1:
			smin = 2.0; 
			smax = 2.5; 
			break; 

		case 2:
			smin = 2.5; 
			smax = 2.85; 
			break; 
		case 3:
			smin = 2.0; 
			smax = 2.5; 
			break; 
		case 4:
			smin = 2.0; 
			smax = 2.5; 
			break; 
		case 5:
			smin = 2.5; 
			smax = 2.85; 
			break; 
		}
		time++;
		m_AgentManager->AddGroup(m_AgentMesh->m_centroid_agents, this->m_RegionMin[this->m_AgentMesh->m_groupID], 
			this->m_RegionMax[this->m_AgentMesh->m_groupID], this->m_GoalMin[this->m_AgentMesh->m_groupID],
			this->m_GoalMax[this->m_AgentMesh->m_groupID], /*0.9, 1.5*/0.9*smin, 3.0*smax,this->m_AgentMesh->m_groupID); 


        glBindFramebuffer(GL_FRAMEBUFFER, 0);

		this->m_randomAgentFinished = true; 
		printf("end_RandomizeAgentPass\n"); 
	}
	void ClinchAgentPass()
	{
		if(this->m_clinchAgent == false)
			return; 
		if(this->m_clinchAgentFinished == true)
			return; 

		glUseProgram(0);

		//OverWriting 
		m_agentMapFBO.BindForWriting();

		m_AgentMesh->Render();

		//glBindFramebuffer(GL_READ_FRAMEBUFFER, m_agentMapFBO.getFBO());
		
		//m_AgentMesh->Read();

		//debug purpose
		//this->m_clinchAgentFinished = true; 

	}
	void RenderAgentVoronoi()
	{
		if(this->m_clinchAgent == false)
			return; 
		if(this->m_clinchAgentFinished == true)
			return; 

		//OverWriting 
		m_agentMapFBO.BindForWriting();

		glPointSize(1);
		glDisable(GL_DITHER);
		glEnable(GL_DEPTH_TEST);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //null region id
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//m_voronoiTessallationTech.Enable(); 
		//m_AgentBufferGeometry->Render();

		//glBindFramebuffer(GL_READ_FRAMEBUFFER, m_agentMapFBO.getFBO());
		
		//m_AgentMesh->Read();

		//debug purpose
		//this->m_clinchAgentFinished = true; 

	}
	void RenderPass()
	{
		//if(m_drawAgent ==true)
		//	return; 
		if(m_randomMarkerFinished == false && m_randomAgentFinished == false && m_clinchAgentFinished == false)
			return; 
		if(m_launchSimulation == true)
			return;
		if(GPUCopy == false){
		
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_fullQuadTech.Enable(); 
       
			m_agentMapFBO.BindForReading(COLOR_TEXTURE_UNIT);

			m_fullQuadTech.SetIntegerTextureFlag(0.0f); 

			m_FullScreenQuad->Render();
			
			if(m_clinchAgent){
				m_AgentMesh->DrawDebug(true, true); 
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}
 
		else{

		  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_agentMapFBO.getFBO());
		  glReadBuffer(GL_COLOR_ATTACHMENT0);
		  glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                          0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		  	if(m_clinchAgent){
				m_AgentMesh->DrawDebug(true, true); 
			}
		

		  glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}
	}

    void RenderVoronoiAgents()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_agentMapFBO.getFBO());
	    glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                          0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//void RenderGuidanceField(unsigned int group)
	//{
	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT);
	//	//glDrawBuffer(GL_NONE);//no fragment 
	//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//	m_fullQuadTech.Enable(); 
	//	m_agentMapFBO.BindForReading(COLOR_TEXTURE_UNIT);
	//	m_collisionAvoidanceFBO.BindForDensityImageReading(1); //no active for image unit
	//	m_collisionAvoidanceFBO.BindForAntiGradientWriting(GL_TEXTURE2, 2); //no active for image unit
	//	m_guidanceArrayTex->Bind(GL_TEXTURE3);
	//	m_navigationArrayTex->Bind(GL_TEXTURE4); 

	//	m_fullQuadTech.SetIntegerTextureFlag(4.0f); 
	//	m_fullQuadTech.SetGroupID(group);
	//	m_fullQuadTech.SetIntegerGuidance(1); 
	//	m_fullQuadTech.SetIntegerNavi(0); 
	//	m_fullQuadTech.SetIntegerNaviGuidance(1);

	//	m_FullScreenQuad->Render();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}
	//void RenderNavigationField(unsigned int group)
	//{
	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT);
	//	//glDrawBuffer(GL_NONE);//no fragment 
	//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//	m_fullQuadTech.Enable(); 
	//	m_agentMapFBO.BindForReading(COLOR_TEXTURE_UNIT);
	//	m_collisionAvoidanceFBO.BindForDensityImageReading(1); //no active for image unit
	//	m_collisionAvoidanceFBO.BindForAntiGradientWriting(GL_TEXTURE2, 2); //no active for image unit
	//	m_guidanceArrayTex->Bind(GL_TEXTURE3);
	//	m_navigationArrayTex->Bind(GL_TEXTURE4); 

	//	m_fullQuadTech.SetIntegerTextureFlag(4.0f); 
	//	m_fullQuadTech.SetGroupID(group);
	//	m_fullQuadTech.SetIntegerGuidance(0); 
	//	m_fullQuadTech.SetIntegerNavi(1); 
	//	m_fullQuadTech.SetIntegerNaviGuidance(1);

	//	m_FullScreenQuad->Render();
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}

	void RenderAgents(int group, bool all){

		if(!m_drawAgent)
			return; 

		//glUseProgram(0);

		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_AgentManager->DrawAgents(group, all); 

	}

	//void RenderMarkerSamplingPass()
	//{
	//	printf("start_RenderMarkerSamplingPass\n"); 
	//	m_markerSamplingFBO.BindForWriting();

	//	//for debug
	//	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // black means no sample existed for the current pixel

	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	m_markersamplingTech.Enable(); 

	//	m_markerSamplingFBO.BindForTextureWriting(MARKER_TEXTURE_WRITE_UNIT, MARKER_TEXTURE_WRITE_UNIT_INDEX); 

	//	m_markerMapFBO.BindForReading(GL_TEXTURE2); //read texture 0

	//	m_eularianMapFBO_FullResolution.BindForReading(GL_TEXTURE1);//read texture 1

	//	m_agentMapFBO.BindForReading(GL_TEXTURE0); 

	//	m_MarkerGeometry->Render();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	//////Debug Purposes
	//	//glBindBuffer(GL_ARRAY_BUFFER, this->m_markerSamplingFBO.getWriteBuffer());
	//	//int markernum = this->m_markerSamplingFBO.getMarkerNum();

	//	//if(Pointer_MarkerSampling == NULL)
	//	//	Pointer_MarkerSampling = new float[markernum*4]; 

	//	//float* Pointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, markernum*4*sizeof(float), GL_MAP_READ_BIT);
	//	//float t[4]; 
	//	//int agentID, markerID; 
	//	//if(MarkerAgentStats == NULL)
	//	//	MarkerAgentStats = new MarkerArray[600];  //estimate how many markers per agent voronoi region has
	//	//for(int i = 0 ; i< markernum; i++){
	//	//	Pointer_MarkerSampling[4*i] = Pointer[i*4]; 
	//	//	Pointer_MarkerSampling[4*i + 1] = Pointer[4*i + 1]; 
	//	//	Pointer_MarkerSampling[4*i + 2] = Pointer[4*i + 2]; 
	//	//	Pointer_MarkerSampling[4*i + 3] = Pointer[4*i + 3]; 

	//	//	t[3] = Pointer_MarkerSampling[i*4 + 3]; 
	//	//	t[2] = Pointer_MarkerSampling[i*4 + 2]; 
	//	//	t[1] = Pointer_MarkerSampling[i*4 + 1]; 
	//	//	t[0] = Pointer_MarkerSampling[i*4]; 

	//	//	agentID = t[1];
	//	//	markerID = i;

	//	//	if(agentID >= 600)
	//	//	{
	//	//		agentID = agentID; 
	//	//	}
	//	//	//printf("AgentID: %d, markerID: %d\n", agentID, i);
	//	//	MarkerAgentStats[agentID].push_back(markerID); 
	//	//}
	//	//int sum = 0; 
	//	//for(int i = 0 ; i< 600; i++){
	//	//	sum += MarkerAgentStats[i].size();
	//	//	//printf("AgentID: %d, SAMPLE: %d\n", i, MarkerAgentStats[i].size());
	//	//}
	//
	//	/////*float group_agent_id = *Pointer;*/
	//	//glUnmapBuffer(GL_ARRAY_BUFFER);
	//	printf("end_RenderMarkerSamplingPass\n"); 

	//	////Debug Sampling Result
	//	////m_markerSamplingDebugFBO.BindForWriting();
	//	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // black means no sample existed for the current pixel
	//	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	//m_markersamplingDebugTech.Enable(); 
	//	//m_markerSamplingDebugFBO.BindForTextureWriting(GL_TEXTURE1, 1); 
	//	//m_markerSamplingFBO.BindForReading(GL_TEXTURE0); //read texture 0

	//	//m_MarkerGeometry->Render();
	//	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//		
	//	//////Debug Purposes
	//	//glBindBuffer(GL_ARRAY_BUFFER, this->m_markerSamplingDebugFBO.getWriteBuffer());
	//	//int markernum2 = this->m_markerSamplingDebugFBO.getMarkerNum();
	//	//float* Pointer2 = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, markernum2*4*sizeof(float), GL_MAP_READ_BIT);
	//	//float t2[4]; 
	//	//float temp2; 
	//	//int agentID2, markerID2; 
	//	//if(Pointer_MarkerSamplingDebug == NULL)
	//	//	Pointer_MarkerSamplingDebug = new float[markernum*4]; 
	//	//if(MarkerAgentStatsDebug == NULL)
	//	//	MarkerAgentStatsDebug = new MarkerArray[600]; 

	//	//for(int i = 0 ; i< markernum2; i++){
	//	//	Pointer_MarkerSamplingDebug[4*i + 3] = Pointer2[i*4 + 3]; 
	//	//	Pointer_MarkerSamplingDebug[4*i + 2] = Pointer2[i*4 + 2]; 
	//	//	Pointer_MarkerSamplingDebug[4*i + 1] = Pointer2[i*4 + 1]; 
	//	//	Pointer_MarkerSamplingDebug[4*i + 0] = Pointer2[i*4]; 

	//	//	t2[3] = Pointer2[i*4 + 3]; 
	//	//	t2[2] = Pointer2[i*4 + 2]; 
	//	//	t2[1] = Pointer2[i*4 + 1]; 
	//	//	t2[0] = Pointer2[i*4]; 

	//	//	agentID2 = t2[1];
	//	//	markerID2 = i;

	//	//	//printf("Debug AgentID: %d, markerID: %d\n", agentID2,markerID2);
	//	//	MarkerAgentStatsDebug[agentID2].push_back(markerID2); 
	//	//}
	//	//glUnmapBuffer(GL_ARRAY_BUFFER);

	//	////Local Data Compare
	//	//
	//	//for(int i = 0 ; i< markernum2; i++)
	//	//{
	//	//	agentID = Pointer_MarkerSampling[4*i]; 
	//	//	agentID2 = Pointer_MarkerSamplingDebug[4*i]; 
	//	//	markerID = Pointer_MarkerSampling[4*i + 1];
	//	//	markerID2 = Pointer_MarkerSamplingDebug[4*i + 1];


	//	//	/*printf("AgentID: %d, markerID: %d, frag: %f, %f\n", agentID, markerID, Pointer_MarkerSampling[4*i + 2], 
	//	//		Pointer_MarkerSampling[4*i + 3]);
	//	//	printf("Debug AgentID2: %d, markerID2: %d, frag: %f, %f\n", agentID2, markerID2, Pointer_MarkerSamplingDebug[4*i + 2], 
	//	//		Pointer_MarkerSamplingDebug[4*i + 3]);
	//	//	printf("--------------------------------------------------\n"); */
	//	//}

	//	//for(int i =0 ; i < 600; i++)
	//	//{
	//	//	printf("AgentID: %d, Marker#: %d\n " , i, MarkerAgentStats[i].size());
	//	//	printf("Debug AgentID: %d, Marker#: %d\n " , i, MarkerAgentStatsDebug[i].size());
	//	//	printf("--------------------------------------------------\n"); 
	//	//}
	//}

	void RenderMarkerSamplingBufferPass()
	{
		m_markerSamplingBufferFBO.BindForWriting();

		//for debug
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // black means no sample existed for the current pixel
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_markersamplingBufferTech.Enable(); 
		m_eularianMapFBO_FullResolution.BindForReading(GL_TEXTURE1);//read texture 1
		m_agentMapFBO.BindForReading(GL_TEXTURE0); 

		m_MarkerBufferGeometry->DrawMarkersBuffer(); 

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//////Debug Purposes
		//glBindBuffer(GL_ARRAY_BUFFER, this->m_markerSamplingFBO.getWriteBuffer());
		//int markernum = this->m_markerSamplingFBO.getMarkerNum();

		//if(Pointer_MarkerSampling == NULL)
		//	Pointer_MarkerSampling = new float[markernum*4]; 

		//float* Pointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, markernum*4*sizeof(float), GL_MAP_READ_BIT);
		//float t[4]; 
		//int agentID, markerID; 
		//if(MarkerAgentStats == NULL)
		//	MarkerAgentStats = new MarkerArray[600];  //estimate how many markers per agent voronoi region has
		//for(int i = 0 ; i< markernum; i++){
		//	Pointer_MarkerSampling[4*i] = Pointer[i*4]; 
		//	Pointer_MarkerSampling[4*i + 1] = Pointer[4*i + 1]; 
		//	Pointer_MarkerSampling[4*i + 2] = Pointer[4*i + 2]; 
		//	Pointer_MarkerSampling[4*i + 3] = Pointer[4*i + 3]; 

		//	t[3] = Pointer_MarkerSampling[i*4 + 3]; 
		//	t[2] = Pointer_MarkerSampling[i*4 + 2]; 
		//	t[1] = Pointer_MarkerSampling[i*4 + 1]; 
		//	t[0] = Pointer_MarkerSampling[i*4]; 

		//	agentID = t[1];
		//	markerID = i;

		//	if(agentID >= 600)
		//	{
		//		agentID = agentID; 
		//	}
		//	//printf("AgentID: %d, markerID: %d\n", agentID, i);
		//	MarkerAgentStats[agentID].push_back(markerID); 
		//}
		//int sum = 0; 
		//for(int i = 0 ; i< 600; i++){
		//	sum += MarkerAgentStats[i].size();
		//	//printf("AgentID: %d, SAMPLE: %d\n", i, MarkerAgentStats[i].size());
		//}
	
		/////*float group_agent_id = *Pointer;*/
		//glUnmapBuffer(GL_ARRAY_BUFFER);
		printf("end_RenderMarkerSamplingPass\n"); 

		////Debug Sampling Result
		////m_markerSamplingDebugFBO.BindForWriting();
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // black means no sample existed for the current pixel
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//m_markersamplingDebugTech.Enable(); 
		//m_markerSamplingDebugFBO.BindForTextureWriting(GL_TEXTURE1, 1); 
		//m_markerSamplingFBO.BindForReading(GL_TEXTURE0); //read texture 0

		//m_MarkerGeometry->Render();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
		//////Debug Purposes
		//glBindBuffer(GL_ARRAY_BUFFER, this->m_markerSamplingDebugFBO.getWriteBuffer());
		//int markernum2 = this->m_markerSamplingDebugFBO.getMarkerNum();
		//float* Pointer2 = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, markernum2*4*sizeof(float), GL_MAP_READ_BIT);
		//float t2[4]; 
		//float temp2; 
		//int agentID2, markerID2; 
		//if(Pointer_MarkerSamplingDebug == NULL)
		//	Pointer_MarkerSamplingDebug = new float[markernum*4]; 
		//if(MarkerAgentStatsDebug == NULL)
		//	MarkerAgentStatsDebug = new MarkerArray[600]; 

		//for(int i = 0 ; i< markernum2; i++){
		//	Pointer_MarkerSamplingDebug[4*i + 3] = Pointer2[i*4 + 3]; 
		//	Pointer_MarkerSamplingDebug[4*i + 2] = Pointer2[i*4 + 2]; 
		//	Pointer_MarkerSamplingDebug[4*i + 1] = Pointer2[i*4 + 1]; 
		//	Pointer_MarkerSamplingDebug[4*i + 0] = Pointer2[i*4]; 

		//	t2[3] = Pointer2[i*4 + 3]; 
		//	t2[2] = Pointer2[i*4 + 2]; 
		//	t2[1] = Pointer2[i*4 + 1]; 
		//	t2[0] = Pointer2[i*4]; 

		//	agentID2 = t2[1];
		//	markerID2 = i;

		//	//printf("Debug AgentID: %d, markerID: %d\n", agentID2,markerID2);
		//	MarkerAgentStatsDebug[agentID2].push_back(markerID2); 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		////Local Data Compare
		//
		//for(int i = 0 ; i< markernum2; i++)
		//{
		//	agentID = Pointer_MarkerSampling[4*i]; 
		//	agentID2 = Pointer_MarkerSamplingDebug[4*i]; 
		//	markerID = Pointer_MarkerSampling[4*i + 1];
		//	markerID2 = Pointer_MarkerSamplingDebug[4*i + 1];


		//	/*printf("AgentID: %d, markerID: %d, frag: %f, %f\n", agentID, markerID, Pointer_MarkerSampling[4*i + 2], 
		//		Pointer_MarkerSampling[4*i + 3]);
		//	printf("Debug AgentID2: %d, markerID2: %d, frag: %f, %f\n", agentID2, markerID2, Pointer_MarkerSamplingDebug[4*i + 2], 
		//		Pointer_MarkerSamplingDebug[4*i + 3]);
		//	printf("--------------------------------------------------\n"); */
		//}

		//for(int i =0 ; i < 600; i++)
		//{
		//	printf("AgentID: %d, Marker#: %d\n " , i, MarkerAgentStats[i].size());
		//	printf("Debug AgentID: %d, Marker#: %d\n " , i, MarkerAgentStatsDebug[i].size());
		//	printf("--------------------------------------------------\n"); 
		//}
	}


	void RenderMarkerSampleBuffer()
	{
		//printf("start_DrawAgent\n"); 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH);

		//glEnable(GL_PROGRAM_POINT_SIZE);
        //glPointSize(32.0f);
		glDisable(GL_PROGRAM_POINT_SIZE);
		m_drawMarkersBufferTech.Enable(); 
		m_MarkerBufferGeometry->DrawMarkersBuffer(); 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
 //   void RenderMotionPass()
	//{
	//	printf("start_RenderMotionPass\n"); 
	//	m_test1.clear(); 

	//	m_motionComputingFBO.BindForWriting();

	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	m_motionComputingTech.Enable(); 

	//	m_motionComputingFBO.BindForTextureWriting(MOTION_TEXTURE_WRITE_UNIT, MOTION_TEXTURE_WRITE_UNIT_INDEX); 
 //      
	//	m_agentMapFBO.BindForReading(AGENT_VORONOI_TEXTURE_UNIT);

	//	m_markerSamplingFBO.BindForReading(MARKER_SAMPLING_TEXTURE_UNIT);

	//	m_navigationArrayTex->Bind(MOTION_NAVI_ARRAY_UNIT); 

	//	m_guidanceArrayTex->Bind(GL_TEXTURE6); 

	//	m_collisionAvoidanceFBO.BindForDensityImageReading(4); 

	//	m_collisionAvoidanceFBO.BindForAntiGradientImageReading(5); 

	//	m_collisionAvoidanceFBO.BindForTextureReading(GL_TEXTURE6, 7);

	//	m_MotionComputingGeometry->Render();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	//////GPU-->cpu
	//	if(Pointer_MotionSampling == NULL)
	//		Pointer_MotionSampling = new float[4 * 600]; 

	//	if(PrintMotionVector == true)
	//	{
	//		glBindBuffer(GL_ARRAY_BUFFER, this->m_motionComputingFBO.getWriteBuffer());
	//		float* Pointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600*4*sizeof(float), GL_MAP_READ_BIT);
	//		float t[4]; 
	//		int agentID; 
	//		int agentID2; 
	//		float ratio; 
	//		float motionvector_average[8];
	//		float max_gradient_mag = 0.0f; 
	//		for(int i =0; i< 8; i++)
	//		{
	//			motionvector_average[i] = 0.0f; 
	//		}
	//		int count = 0; 
	//		for(int i = 0 ; i< 600; i++){
	//			Pointer_MotionSampling[i*4 + 3] = Pointer[i*4 + 3]; 
	//			Pointer_MotionSampling[i*4 + 2] = Pointer[i*4 + 2]; 
	//			ratio = Pointer_MotionSampling[i*4 + 1] = Pointer[i*4 + 1]; 
	//			agentID = Pointer_MotionSampling[i*4] = Pointer[i*4]; 

	//			max_gradient_mag = Pointer_MotionSampling[i*4 + 1] > max_gradient_mag ? Pointer_MotionSampling[i*4 + 1] : max_gradient_mag; 

	//			if(i == 150 || i == 300 || i== 450)
	//			{
	//				i = i; 
	//			}

	//			printf("Navi: %f, %f, %f, %f\n", Pointer_MotionSampling[i*4], 
	//				Pointer_MotionSampling[i*4 + 1], Pointer_MotionSampling[i*4 + 2], Pointer_MotionSampling[i*4 + 3]);

	//			count+=(int)Pointer_MotionSampling[i*4 + 2];
	//			/*if(i < 150)
	//			{
	//				motionvector_average[0] += Pointer_MotionSampling[i*4 + 2]; 
	//				motionvector_average[1] += Pointer_MotionSampling[i*4 + 3]; 
	//			}
	//		
	//			if(i >= 150 && i < 300)
	//			{
	//				motionvector_average[2] += Pointer_MotionSampling[i*4 + 2]; 
	//				motionvector_average[3] += Pointer_MotionSampling[i*4 + 3]; 
	//			}
	//			if(i >= 300 && i < 450)
	//			{
	//				motionvector_average[4] += Pointer_MotionSampling[i*4 + 2]; 
	//				motionvector_average[5] += Pointer_MotionSampling[i*4 + 3]; 
	//			}
	//			if(i >= 450 && i < 600)
	//			{
	//				motionvector_average[6] += Pointer_MotionSampling[i*4 + 2]; 
	//				motionvector_average[7] += Pointer_MotionSampling[i*4 + 3]; 
	//			}*/
	//		}
	//		//for(int i =0; i< 8; i++)
	//		//{
	//		//	motionvector_average[i] /= 150; 
	//		//}
	//		glUnmapBuffer(GL_ARRAY_BUFFER);
	//	}

	//	
	//	printf("end_RenderMotionPass\n"); 
	//}
	
	//void SplatterDensityVelocityPass()
	//{
	//	//glViewport(0, 0, 128, 128);
	//	m_densityVelocityFBO.BindForWriting();
	//	
	//	glDisable(GL_DEPTH_TEST);
	//	glEnable(GL_BLEND);
	//	glBlendEquation(GL_FUNC_ADD);
	//	glBlendFunc(GL_ONE, GL_ONE);

	//    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT);

	//	m_splatterDensityVelocityTech.Enable(); 

	//	m_pTexture->Bind(GL_TEXTURE0);
 //      
	//	m_MotionComputingGeometry->SplatDensityVelocity();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	glDisable(GL_BLEND);
	//	//glViewport(0, 0, 1600, 900);

	//}

	void SplatterDensityVelocityPassBuffer()
	{
		glViewport(0, 0, 128, 128);
		m_densityVelocityFBO.BindForWriting();
		
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

	    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_splatterDensityVelocityTech.Enable(); 

		m_pTexture->Bind(GL_TEXTURE0);
       
		m_AgentBufferGeometry->RenderDensityVelocitySplat(); 

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_BLEND);
		glViewport(0, 0, 1600, 900);
	}
	void SplatterFutureDiscomfortPassBuffer()
	{
		glViewport(0, 0, 128, 128);
		m_densityVelocityFBO.BindForWriting();
		
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

	    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		m_splatterFutureDiscomfortTech.Enable(); 
		m_pTexture->Bind(GL_TEXTURE0);
		m_AgentBufferGeometry->RenderFutureDiscomfortSplat(); 

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_BLEND);
		glViewport(0, 0, 1600, 900);
	}
	void AverageVelocityCalcPass()
	{
		m_densityVelocityFBO.BindForImageUnitReadingWriting(0); 
		m_averageVelocityCalcTech.Enable();
		glDispatchCompute(128/16, 128/16, 1); 
	}
	void DrawAverageVelocityPass()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		
		m_densityVelocityFBO.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(6.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void SpeedCalculatorPass()
	{
		m_densityVelocityFBO.BindForImageUnitReading(0); 
		m_speedIMO.BindForImageUnitWriting(1); 
		m_speedCalculatorTech.Enable();
		glDispatchCompute(128/16, 128/16, 1); 
	}
	void CostCalculatorPass()
	{
		m_speedIMO.BindForImageUnitReading(0); 
		m_costIMO.BindForImageUnitWriting(1); 
		m_eularianMapFBO_DownSampling.BindForImageReading(2); 
		m_densityVelocityFBO.BindForImageUnitReading(3);
		m_costCalculatorTech.Enable();
		glDispatchCompute(128/16, 128/16, 1); 
	}
	void PotentialCalcultorCleanPass()
	{
		m_potentialIMO.BindForImageUnitWritingOnly(1);
		m_potentialInitTech.Enable();
		glDispatchCompute(128/16, 128/16, 1); 
	}

	void PotentialCalculatorPass()
	{
		m_costIMO.BindForImageUnitReading(0); 
		m_potentialIMO.BindForImageUnitReadingWriting(1);
		m_potentialIMO.BindForImageBufferWritingOnly(2); 
		m_potentialIMO.BindForNeighboringCostImageBufferWritingOnly(3); 
		m_potentialIMO.BindForNeighboringPotentialImageBufferWritingOnly(4); 
		m_potentialCalculatorTech.Enable();
		glDispatchCompute(128/16, 128/16, 1); 


		//glBindBuffer(GL_ARRAY_BUFFER, this->m_potentialIMO.getWriteBuffer());
		//float* Pointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 324 * 8 * 8 * 4 * sizeof(float), GL_MAP_READ_BIT);
		//int start_offset;  
		//int second_offset; 
		//float t[4];
		//float sharedvalue[324]; 
		/////*for(int i = 0 ; i< 8 * 8; i++)
		////{
		////	start_offset = i * 324; 
		////	for(int k = 0; k < 324; k++ )
		////	{ 
		////		second_offset = start_offset + k; 

		////		if(k == 323)
		////		{
		////			k = k ;
		////		}

		////		t[0] = Pointer[4 * second_offset]; 
		////		t[1] = Pointer[4 * second_offset + 1]; 
		////		t[2] = Pointer[4 * second_offset + 2]; 
		////		t[3] = Pointer[4 * second_offset + 3]; 


		////		if(t[0] != 9999 )
		////		{
		////			printf("groupid: %d threadid: %d value: %f, %f, %f, %f\n", i, k, t[0], t[1], t[2], t[3]);
		////		}
		////	}
		////}*/

		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, this->m_potentialIMO.getNeighboringCostWriteBuffer());
		//Pointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 128 * 128 *4*sizeof(float), GL_MAP_READ_BIT);
		//for(int i = 0 ; i< 128 * 128; i++)
		//{
		//	if(i%128 == 0)
		//	{
		//		i==i; 
		//	}

		//	
		//	    t[0] = Pointer[ 4 * i]; 
		//		t[1] = Pointer[ 4 * i + 1]; 
		//		t[2] = Pointer[ 4 * i + 2]; 
		//		t[3] = Pointer[ 4 * i + 3]; 

		//		
		//		printf("Cost:%d:  %f, %f, %f, %f\n", i,
		//			t[0], 
		//			t[1], 
		//			t[2], 
		//			t[3]
		//		);
		//		
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		///*glBindBuffer(GL_ARRAY_BUFFER, this->m_potentialIMO.getNeighboringPotentialWriteBuffer());
		//Pointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 128 * 128 *4*sizeof(float), GL_MAP_READ_BIT);
		//for(int i = 0 ; i< 128 * 128; i++)
		//{
		//	
		//	    t[0] = Pointer[ 4 * i]; 
		//		t[1] = Pointer[ 4 * i + 1]; 
		//		t[2] = Pointer[ 4 * i + 2]; 
		//		t[3] = Pointer[ 4 * i + 3]; 

		//		if(flag[i]==true)
		//		{
		//			printf("Potential:%d:  %f, %f, %f, %f\n", i,
		//			 t[0], 
		//			 t[1], 
		//			 t[2], 
		//			 t[3]
		//			);
		//		}
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);*/

	}

	void GradientCalculatorPass()
	{
		m_potentialIMO.BindForImageUnitReadingOnly(0); 
		m_gradientIMO12.BindForImageUnitWriting(1); 
		m_gradientIMO34.BindForBufferImageUnitWriting(3); 
		m_gradientIMO34.BindForImageUnitWriting(2); 

		m_gradientCalculatorTech.Enable();
		glDispatchCompute(128/16, 128/16, 1); 

		/*glBindBuffer(GL_ARRAY_BUFFER, this->m_gradientIMO34.getWriteBuffer());
		float* Pointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 128 * 128 * 4 * sizeof(float), GL_MAP_READ_BIT);
		int start_offset;  
		int second_offset; 
		float t[4];
		float grid_value[128 ]; 
		int row, col; 
		for(int i = 0 ; i< 128 * 128; i++)
		{
			row = i/128; 
			col = i- row * 128; 
		
			t[0] = Pointer[4 * i]; 
			t[1] = Pointer[4 * i + 1]; 
			t[2] = Pointer[4 * i + 2]; 
			t[3] = Pointer[4 * i + 3]; 
			
			printf("x: %d, y, %d:  %f, %f\n", row, col, t[2], t[3]);
	
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);*/
	}
	void RandomizeProbability()
	{
		m_AgentBufferGeometry->UpdateRandomPropability(*m_AgentManager);
	}
	void AgentMovePass()
	{

		m_gradientIMO12.BindForImageUnitReading(0); 
		m_gradientIMO34.BindForImageUnitReading(1); 
		m_speedIMO.BindForImageUnitReading(2); 
		m_potentialIMO.BindForImageUnitReadingOnly(3); 
		m_uncertaintyIMO.BindForImageUnitReading(4); 
		m_maskedNaviGuidIMO.BindForMaskedNaviImageArrayUnitReading(5); //it's a weired bug here
		m_markerSamplingBufferFBO.BindForImageReading(6); 

		//m_maskedNaviGuidIMO.BindForMaskedGuidImageArrayUnitReading(5);
		m_agentAdvectionTech.Enable();
		m_agentAdvectionTech.SetBuildingCt(m_BuildingManager->NumBuildings());
		m_agentAdvectionTech.SetHasGUID(TUNNEL_ENABLE); 
		m_AgentBufferGeometry->ComputeAdvectionBindingBuffer();
		glDispatchCompute(20, 1, 1);
		m_AgentBufferGeometry->ComputeAdvectionUnBindingBuffer(); 

		////output test
		//int *groupID = new int[600]; 
		//int *agentID = new int[600];
		//float *position = new float[2 * 600]; 
		//float *direction = new float[2 * 600]; 
		//float *speed = new float[600]; 
		//float *maxspeed = new float[600]; 
		//float *radians = new float[600]; 
		//float *turnrate = new float[600]; 
		//float *parkpos = new float[600 * 3]; 
		//float *originalpos = new float[600*2]; 
		//float *deltapos = new float[600*4]; 

		/*int *debugGuid = new int[800*4]; */
		glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->DebugGuidBuffer());
		float* fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, AGENT_NUM *4* sizeof(float), GL_MAP_READ_BIT);
		float guid_delta[4]; 
		for(int i = 0 ; i< AGENT_NUM; i++)
		{
			guid_delta[0] = fPointer[4 * i + 0]; 
			guid_delta[1] = fPointer[4 * i + 1]; 	
			guid_delta[2] = fPointer[4 * i + 2]; 
			guid_delta[3] = fPointer[4 * i + 3]; 

			if(i >= AGENT_NUM)
			{
				i==i;
				printf("%f, %f, %f, %f\n",guid_delta[0], guid_delta[1], guid_delta[2], guid_delta[3]);
			}
			
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->GidAidBuffer());
		//int* Pointer = (int*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * 2 * sizeof(int), GL_MAP_READ_BIT);
		//int groupid, agentid; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	groupid = Pointer[2 * i + 0]; 
		//	agentid = Pointer[2 * i + 1]; 	
		//	groupID[i] = groupid; 
		//	agentID[i] = agentid; 
		//	printf("groupID: %d, agentID: %d\n", groupid, agentid);
		//}

		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->PositionBuffer());
		//float* fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * 2 * sizeof(float), GL_MAP_READ_BIT);
		//float pos[2]; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	pos[0] = fPointer[2 * i + 0]; 
		//	pos[1] = fPointer[2 * i + 1]; 	
		//	position[2 * i] = pos[0]; 
		//	position[2 * i + 1] = pos[1]; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->DirectionBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * 2 * sizeof(float), GL_MAP_READ_BIT);
		//float _direction[2]; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	_direction[0] = fPointer[2 * i + 0]; 
		//	_direction[1] = fPointer[2 * i + 1]; 	
		//	direction[2 * i] = _direction[0]; 
		//	direction[2 * i + 1] = _direction[1]; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->SpeedBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * sizeof(float), GL_MAP_READ_BIT);
		//float fspeed; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	fspeed = fPointer[i];
		//	speed[i] = fspeed; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->MaxSpeedBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * sizeof(float), GL_MAP_READ_BIT);
		//float fmaxspeed; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	fmaxspeed = fPointer[i];
		//	maxspeed[i] = fmaxspeed; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->RadBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * sizeof(float), GL_MAP_READ_BIT);
		//float fradius; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	fradius = fPointer[i];
		//	radians[i] = fradius; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->TurnRateBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * sizeof(float), GL_MAP_READ_BIT);
		//float fturnrate; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	fturnrate = fPointer[i];
		//	turnrate[i] = fturnrate; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->ParkPosBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 *3 * sizeof(float), GL_MAP_READ_BIT);
		//float parkposlive[3]; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	parkposlive[0] = fPointer[3 * i];
		//	parkposlive[1] = fPointer[3 * i+1];
		//	parkposlive[2] = fPointer[3 * i+2];

		//	parkpos[3 * i] = parkposlive[0];
		//	parkpos[3 * i + 1] = parkposlive[1];
		//	parkpos[3 * i + 2] = parkposlive[2];
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->OrigPosBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 *2 * sizeof(float), GL_MAP_READ_BIT);
		//float _originalpos[2]; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	_originalpos[0] = fPointer[2 * i];
		//	_originalpos[1] = fPointer[2 * i+1];

		//	originalpos[2 * i] = _originalpos[0] ;
		//	originalpos[2 * i + 1] = _originalpos[1] ;
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);


		/*glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->DeltaPosBuffer());
		float* deltabufferPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 *4 * sizeof(float), GL_MAP_READ_BIT);
		float _deltapos[4]; 
		for(int i = 0 ; i< 600; i++)
		{
			_deltapos[0] = deltabufferPointer[4 * i];
			_deltapos[1] = deltabufferPointer[4 * i + 1];
			_deltapos[2] = deltabufferPointer[4 * i + 2];
			_deltapos[3] = deltabufferPointer[4 * i + 3];

			printf("delta0: %f, delta1: %f, status0: %f, status1: %f\n", _deltapos[0], _deltapos[1],
				_deltapos[2], _deltapos[3]);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);*/

		////delte translation
		//float dx, dy; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	if(i == 150 || i == 300 || i == 450)
		//	{
		//		i == i; 
		//	}
		//	dx = position[2 * i] - originalpos[2 * i]; 
		//	dy = position[2 * i + 1] - originalpos[2 * i + 1]; 
		//    printf("i: %d, dx: %f, dy: %f\n", i, position[2 * i], position[2 * i + 1]);
		//}

	}
	void DrawSpeedPass()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		
		m_speedIMO.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(6.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void DrawCostPass()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		
		m_costIMO.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(6.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void RenderUncertainty()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		
		m_uncertaintyIMO.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(8.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void RenderFutureDiscomfort()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		
		m_densityVelocityFBO.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(7.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void DrawPotentialPass()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		
		m_potentialIMO.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(6.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void DrawGradientPass()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		
		m_gradientIMO34.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(6.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	//void RenderDensityField()
	//{
	//	printf("start_RenderDensityField\n"); 
	//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT);

	//	m_fullQuadTech.Enable(); 
	//	m_agentMapFBO.BindForReading(COLOR_TEXTURE_UNIT);
	//	m_collisionAvoidanceFBO.BindForDensityImageReading(GL_TEXTURE1); 
	//	m_fullQuadTech.SetIntegerTextureFlag(1.0f); 
	//	m_FullScreenQuad->Render();


	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	printf("end_RenderDensityField\n"); 

	//}
	//void RenderAntiGradientField()
	//{
	//	printf("start_RenderAntiGradientField\n"); 
	//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT);

	//	m_fullQuadTech.Enable(); 
	//	m_agentMapFBO.BindForReading(COLOR_TEXTURE_UNIT);
	//	m_collisionAvoidanceFBO.BindForAntiGradientImageReading(2); 
	//	m_fullQuadTech.SetIntegerTextureFlag(3.0f); 
	//	m_FullScreenQuad->Render();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	printf("end_RenderAntiGradientField\n"); 
	//}
	//void ComputeAntiGradientField()
	//{
	//	printf("start_ComputeAntiGradientField\n"); 

	//	m_collisionAvoidanceFBO.BindForWritingAntiGradientFrameBuffer();
	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT);
	//	glDrawBuffer(GL_NONE);//no fragment 
	//	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //

	//	m_fullQuadTech.Enable(); 
	//	m_agentMapFBO.BindForReading(COLOR_TEXTURE_UNIT);
	//	m_collisionAvoidanceFBO.BindForDensityImageReading(1); //no active for image unit
	//	m_collisionAvoidanceFBO.BindForAntiGradientWriting(GL_TEXTURE2, 2); //no active for image unit
	//	m_fullQuadTech.SetIntegerTextureFlag(2.0f); 
	//	m_FullScreenQuad->Render();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	printf("end_ComputeAntiGradientField\n"); 
	//}
	//void RenderCollisionAvoidingPass()
	//{
	//	printf("start_RenderCollisionAvoidingPass\n"); 

	//	m_collisionAvoidanceFBO.BindForWritingDensityFrameBuffer();
	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT);
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	m_collisionAvoidanceFBO.BindForWriting();

	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	m_collsionAvoidingTech.Enable(); 

	//	m_collisionAvoidanceFBO.BindForTextureWriting(FINAL_MOTION_TEXTURE_WRITE_UNIT, FINAL_MOTION_TEXTURE_WRITE_UNIT_INDEX); 

	//	m_collisionAvoidanceFBO.BindForDensityWriting(FINAL_DENSITY_TEXTURE_WRITE_UNIT, FINAL_DENSITY_TEXTURE_WRITE_UNIT_INDEX);
 //       
	//	m_collisionAvoidanceFBO.BindForDensityBufferWriting(FINAL_DENSITY_TEXTURE_BUFFER_WRITE_UNIT, FINAL_DENSITY_TEXTURE_BUFFER_WRITE_UNIT_INDEX);

	//	m_agentMapFBO.BindForReading(FINAL_AGENT_VORONOI_TEXTURE_UNIT);

	//	m_motionComputingFBO.BindForReading(FINAL_MOTION_TEXTURE_UNIT);

	//	//m_navigationArrayTex->Bind(RANDOM_TEXTURE_UNIT); 

	//	m_MotionComputingGeometry->Render();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	glBindBuffer(GL_ARRAY_BUFFER, this->m_collisionAvoidanceFBO.getWriteBuffer());

	//	if(Pointer_CollisionAvoidance == NULL)
	//		Pointer_CollisionAvoidance = new float[600 * 4]; 

	//	float* Pointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600*4*sizeof(float), GL_MAP_READ_BIT);
	//	//memcpy(ptr, Pointer, 600*4*4); 
	//	int ct1; 
	//	int ct2; 
	//	int ct3; 
	//	int ct4; 
	//	ct1 = ct2 = ct3 = ct4 = 0; 

	//	for(int i = 0 ; i< 600; i++)
	//	{
	//		Pointer_CollisionAvoidance[i*4 + 3] = Pointer[i*4 + 3]; 
	//		Pointer_CollisionAvoidance[i*4 + 2] = Pointer[i*4 + 2]; 
	//		Pointer_CollisionAvoidance[i*4 + 1] = Pointer[i*4 + 1]; 
	//		Pointer_CollisionAvoidance[i*4] = Pointer[i*4]; 

	//		//if(i == 150 || i == 300 || i== 450)
	//		//{
	//		//	i = i; 
	//		//}

	//		//printf("Navi: %f, %f; %f, %f\n", Pointer_CollisionAvoidance[i*4], 
	//		//	Pointer_CollisionAvoidance[i*4 + 1], Pointer_CollisionAvoidance[i*4 + 2], 
	//		//	Pointer_CollisionAvoidance[i*4 + 3] );
	//		
	//		//if(i < 150 && Pointer_CollisionAvoidance[i*4 + 2] != 16 && Pointer[i*4 + 3] == 0){
	//		//	ct1++; 
	//		//}
	//		//
	//		//if(i >= 150 && i<300 &&  Pointer_CollisionAvoidance[i*4 + 2] != 16 && Pointer[i*4 + 3] == 0){
	//		//	ct2++; 
	//		//}
	//		//if(i >= 300 && i<450 &&  Pointer_CollisionAvoidance[i*4 + 2] != 16 && Pointer[i*4 + 3] == 0){
	//		//	ct3++; 
	//		//}
	//		//if(i >= 450 && i<600 &&  Pointer_CollisionAvoidance[i*4 + 2] != 16 && Pointer[i*4 + 3] == 0){
	//		//	ct4++; 
	//		//}
	//	}

	//	///*float group_agent_id = *Pointer;*/
	//	glUnmapBuffer(GL_ARRAY_BUFFER);

	//	/*glBindBuffer(GL_ARRAY_BUFFER, this->m_collisionAvoidanceFBO.getDensityWriteBuffer());
	//	if(Pointer_DensityField == NULL)
	//		Pointer_DensityField = new  int[64 * 64]; 

	//	int* Pointer_UI = (int*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 64 * 64 *sizeof(int), GL_MAP_READ_BIT);
	//	int count = 0; 
	//	for(int i = 0 ; i< 64 * 64; i++)
	//	{
	//		Pointer_DensityField[i] = Pointer_UI[i]; 
	//		count+=Pointer_DensityField[i] ;

	//		printf("%d: %d\n", i, Pointer_DensityField[i]);
	//	}

	//	glUnmapBuffer(GL_ARRAY_BUFFER);*/
	//	printf("end_RenderCollisionAvoidingPass\n"); 
	//}

	void ComputeCollisionDetectionPass()
	{
		m_agentMapFBO.BindForImageReading(0); 
		m_markerSamplingBufferFBO.BindForImageReading(1); //FULL RESOLUTOIN
		m_eularianMapFBO_FullResolution.BindForImageReading(3); //FULL RESOLUTION
		m_maskedNaviGuidIMO.BindForMaskedNaviImageArrayUnitReading(2); // 128 * 128
		m_collisionDetectionTech.Enable();

		m_AgentBufferGeometry->CollisionDetectionBindingBuffer();
		glDispatchCompute(20, 1, 1);
		m_AgentBufferGeometry->CollisionDetectionUnBindingBuffer(); 

		////output test
		//int *groupID = new int[600]; 
		//int *agentID = new int[600];
		//float *position = new float[2 * 600]; 
		//float *direction = new float[2 * 600]; 
		//float *speed = new float[600]; 
		//float *maxspeed = new float[600]; 
		//float *radians = new float[600]; 
		//float *turnrate = new float[600]; 
		//float *parkpos = new float[600 * 3]; 
		//float *originalpos = new float[600*2]; 

		/*glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->GidAidBuffer());
		int* Pointer = (int*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * 2 * sizeof(int), GL_MAP_READ_BIT);
		int groupid, agentid; 
		for(int i = 0 ; i< 600; i++)
		{
			groupid = Pointer[2 * i + 0]; 
			agentid = Pointer[2 * i + 1]; 	
			groupID[i] = groupid; 
			agentID[i] = agentid; 
			printf("groupID: %d, agentID: %d\n", groupid, agentid);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
*/
		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->PositionBuffer());
		//float* fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * 2 * sizeof(float), GL_MAP_READ_BIT);
		//float pos[2]; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	pos[0] = fPointer[2 * i + 0]; 
		//	pos[1] = fPointer[2 * i + 1]; 	
		//	position[2 * i] = pos[0]; 
		//	position[2 * i + 1] = pos[1]; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->DirectionBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * 2 * sizeof(float), GL_MAP_READ_BIT);
		//float _direction[2]; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	_direction[0] = fPointer[2 * i + 0]; 
		//	_direction[1] = fPointer[2 * i + 1]; 	
		//	direction[2 * i] = _direction[0]; 
		//	direction[2 * i + 1] = _direction[1]; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->SpeedBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * sizeof(float), GL_MAP_READ_BIT);
		//float fspeed; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	fspeed = fPointer[i];
		//	speed[i] = fspeed; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->MaxSpeedBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * sizeof(float), GL_MAP_READ_BIT);
		//float fmaxspeed; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	fmaxspeed = fPointer[i];
		//	maxspeed[i] = fmaxspeed; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->RadBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * sizeof(float), GL_MAP_READ_BIT);
		//float fradius; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	fradius = fPointer[i];
		//	radians[i] = fradius; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->TurnRateBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 * sizeof(float), GL_MAP_READ_BIT);
		//float fturnrate; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	fturnrate = fPointer[i];
		//	turnrate[i] = fturnrate; 
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->ParkPosBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 *3 * sizeof(float), GL_MAP_READ_BIT);
		//float parkposlive[3]; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	parkposlive[0] = fPointer[3 * i];
		//	parkposlive[1] = fPointer[3 * i+1];
		//	parkposlive[2] = fPointer[3 * i+2];

		//	parkpos[3 * i] = parkposlive[0];
		//	parkpos[3 * i + 1] = parkposlive[1];
		//	parkpos[3 * i + 2] = parkposlive[2];
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->OrigPosBuffer());
		//fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 *2 * sizeof(float), GL_MAP_READ_BIT);
		//float _originalpos[2]; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	_originalpos[0] = fPointer[2 * i];
		//	_originalpos[1] = fPointer[2 * i+1];

		//	originalpos[2 * i] = _originalpos[0] ;
		//	originalpos[2 * i + 1] = _originalpos[1] ;
		//}
		//glUnmapBuffer(GL_ARRAY_BUFFER);
		
		/*glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->DeltaPosBuffer());
		float* deltabufferPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 *4 * sizeof(float), GL_MAP_READ_BIT);
		float _deltapos[4]; 
		for(int i = 0 ; i< 600; i++)
		{
			_deltapos[0] = deltabufferPointer[4 * i];
			_deltapos[1] = deltabufferPointer[4 * i + 1];
			_deltapos[2] = deltabufferPointer[4 * i + 2];
			_deltapos[3] = deltabufferPointer[4 * i + 3];

			printf("delta0: %f, delta1: %f, status0: %f, status1: %f\n", _deltapos[0], _deltapos[1],
				_deltapos[2], _deltapos[3]);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);*/

		///////////*glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->DebugBuffer());
		//////////float* debugbufferPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 600 *4 * sizeof(float), GL_MAP_READ_BIT);
		//////////float _debug[4]; 
		//////////int collision_ct =0; 
		//////////int non_collision_ct = 0; 
		//////////float ratio; 
		//////////for(int i = 0 ; i< 600; i++)
		//////////{
		//////////	_debug[0] = debugbufferPointer[4 * i];
		//////////	_debug[1] = debugbufferPointer[4 * i + 1];
		//////////	_debug[2] = debugbufferPointer[4 * i + 2];
		//////////	_debug[3] = debugbufferPointer[4 * i + 3];

	 //////////       if(_debug[0] > 0.0f){
		//////////		printf("Debug: %f, %f, %f, %f\n", _debug[0], _debug[1],
		//////////		_debug[2], _debug[3]);
		//////////	}
		//////////}
		//////////glUnmapBuffer(GL_ARRAY_BUFFER);*/

		////delte translation
		//float dx, dy; 
		//for(int i = 0 ; i< 600; i++)
		//{
		//	if(i == 150 || i == 300 || i == 450)
		//	{
		//		i == i; 
		//	}
		//	dx = position[2 * i] - originalpos[2 * i]; 
		//	dy = position[2 * i + 1] - originalpos[2 * i + 1]; 
		//    printf("i: %d, dx: %f, dy: %f\n", i, position[2 * i], position[2 * i + 1]);
		//}



	}

	void DrawDiscomfort()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		
		m_eularianMapFBO_DownSampling.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(6.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	//void DrawAgent()
	//{
	//	printf("start_DrawAgent\n"); 
	//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	//glEnable(GL_PROGRAM_POINT_SIZE);
 //       //glPointSize(32.0f);
	//	glDisable(GL_PROGRAM_POINT_SIZE);

	//	m_drawAgentsTech.Enable(); 

	//	m_MotionComputingGeometry->RenderAgentSprites();

	//	glDisable(GL_PROGRAM_POINT_SIZE);

	//	m_drawBuildingsTech.Enable(); 

	//	m_BuildingGeometry->RenderSprites();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	printf("end_DrawAgent\n"); 
	//}

	void DrawAgentBuffer()
	{
		//printf("start_DrawAgent\n"); 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glEnable(GL_PROGRAM_POINT_SIZE);
        //glPointSize(32.0f);
		glDisable(GL_PROGRAM_POINT_SIZE);

		m_drawAgentsBufferTech.Enable(); 
		m_uncertaintyIMO.BindForImageUnitReading(0);
		m_AgentBufferGeometry->DrawAgentsBuffer(); 
		
		m_drawBuildingsTech.Enable(); 
		m_BuildingGeometry->RenderSprites();

		//m_drawNaviMaskTech.Enable(); 
		//m_NaviBuildBufferGeometry->RenderBoundingBoxBuffer();
		if(TUNNEL_ENABLE)
			DrawSceneNavigation();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//printf("end_DrawAgent\n"); 
	}

	void DrawBackground()
	{
		printf("start_DrawBackground\n"); 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		m_fullQuadTech.SetIntegerTextureFlag(5.0f); 
		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		printf("end_DrawBackground\n"); 
	}
	
	////in the Edit Layer GUI
	//void DrawSceneConfiguration()
	//{
	//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	//glEnable(GL_PROGRAM_POINT_SIZE);
 //       //glPointSize(32.0f);
	//	glDisable(GL_PROGRAM_POINT_SIZE);
	//	glDisable(GL_DEPTH_TEST);

	//	DrawSceneNavigation();

	//	m_drawBuildingsTech.Enable(); 

	//	m_BuildingGeometry->RenderSprites();

	//	m_drawAgentsTech.Enable(); 

	//	m_MotionComputingGeometry->RenderAgentSprites();

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//	glDisable(GL_PROGRAM_POINT_SIZE);
	//}

	//
	void DrawSceneConfigurationBuffer()
	{
		DrawAgentBuffer(); 
	}

	void DrawSceneNavigation()
	{
		m_drawNaviBuildingTech.Enable(); 
		m_NaviBuildBufferGeometry->RenderNavigationBuildingBuffer(); 
	}

	//in the Edit Layer GUI
	void DrawEularianField(int gid, int nid,  bool navi)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(0);

		int id = (navi == true)? nid: gid; 

		this->m_SpriteTex[id]->Draw(0,0,WINDOW_WIDTH,WINDOW_HEIGHT, navi);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void DrawDiscomfortField()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(0);

		this->m_SpriteTex_discomfort->Draw(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,false);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void DrawCostField()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(0);

		this->m_SpriteTex_cost->Draw(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,false);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void DrawBuildingfromFBO()
	{
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, m_eularianMapFBO.getFBO());
	 //   glReadBuffer(GL_COLOR_ATTACHMENT0);
		//glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
  //                        0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		
		m_eularianMapFBO_DownSampling.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(9.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void UpdateBuildingInfo()
	{
		static int drawValid = 1; 
		if(drawValid == 0)
			return; 
		m_AgentBufferGeometry->UpdateBuildingInfo(*m_BuildingManager); 
		drawValid = 0; 
	}
	void ComputeNaviMask()
	{
		static int drawValid = 1; 
		if(drawValid == 0)
			return; 

		glViewport(0, 0, 128, 128);
		m_naviMaskFBO.BindForWriting();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		
		m_drawNaviMaskTech.Enable(); 
		m_eularianMapFBO_DownSampling.BindForReading(GL_TEXTURE0);
		m_NaviBuildBufferGeometry->RenderBoundingBoxBuffer(); 

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1600, 900);
		
		drawValid = 0; 
	}
	void MaskNaviGuidField()
	{
		static int drawValid = 1; 
		if(drawValid == 0)
			return; 

		m_naviMaskFBO.BindForImageReading(0); 
		this->m_navigationArrayTex->BindImageArray(1); 
		this->m_guidanceArrayTex->BindImageArray(2); 
		m_maskedNaviGuidIMO.BindForShaderStorageBuffer(); 
		m_maskedNaviGuidIMO.BindForMaskedNaviImageArrayUnitWriting(3);
		m_maskedNaviGuidIMO.BindForMaskedGuidImageArrayUnitWriting(4);

		m_maskNaviGuidTech.Enable();
        glDispatchCompute(128/16, 128/16, 1); 

		//debug
		glBindBuffer(GL_ARRAY_BUFFER, m_maskedNaviGuidIMO.GUID23Buffer());
		float* Pointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 128 * 128 * 4 * sizeof(float), GL_MAP_READ_BIT);
		int offset; 
		
		for(int y = 0; y< 128; y++) // 
		{
			for(int x = 0; x< 128; x++)
			{
				offset = x + y * 128; 

				if(abs(Pointer[4 * offset+3])>0.0f || abs(Pointer[4 * offset+2])>0.0f)
					printf("Grid(%d, %d): %f, %f\n", x, y,  Pointer[4 * offset+2],Pointer[4 * offset+3]);
			}
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		drawValid = 0; 
	}
	void DrawNaviMask()
	{
				
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_fullQuadTech.Enable(); 
		
		m_naviMaskFBO.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(9.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_fullQuadTech.Enable(); 
		
		m_eularianMapFBO_DownSampling.BindForReading(GL_TEXTURE5);
		
		m_fullQuadTech.SetIntegerTextureFlag(9.0f); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_BLEND);
	}

	void DrawMaskedNavi(int groupID)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_drawMaskedNaviGuidTech.Enable(); 
		m_drawMaskedNaviGuidTech.SetGroupIDUniform(groupID);
		m_drawMaskedNaviGuidTech.SetDrawNaviUniform(true); 
		m_drawMaskedNaviGuidTech.SetMaskedNaviArrayTextureUnit(0); 
		m_drawMaskedNaviGuidTech.SetMaskedGuidArrayTextureUnit(1); 
		m_maskedNaviGuidIMO.BindForMaskedNaviTextureArrayReading(GL_TEXTURE0);
		m_maskedNaviGuidIMO.BindForMaskedGuidTextureArrayReading(GL_TEXTURE1);
		
		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DrawMaskedGuid(int groupID)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_drawMaskedNaviGuidTech.Enable(); 
		m_drawMaskedNaviGuidTech.SetGroupIDUniform(groupID);
		m_drawMaskedNaviGuidTech.SetDrawNaviUniform(false);
		m_drawMaskedNaviGuidTech.SetMaskedNaviArrayTextureUnit(0); 
		m_drawMaskedNaviGuidTech.SetMaskedGuidArrayTextureUnit(1); 
		m_maskedNaviGuidIMO.BindForMaskedNaviTextureArrayReading(GL_TEXTURE0);
		m_maskedNaviGuidIMO.BindForMaskedGuidTextureArrayReading(GL_TEXTURE1);
		m_FullScreenQuad->Render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void DrawBuildingToFBO()
	{
		if(this->m_eularian_dirty_draw == false){
			return; 
		}

		static int drawValid = 1; 
		if(drawValid == 0)
			return; 

		glViewport(0, 0, 128, 128);
		m_eularianMapFBO_DownSampling.BindForWriting();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		//glEnable(GL_PROGRAM_POINT_SIZE);
		
		m_drawBuildingsTech.Enable(); 
		m_BuildingGeometry->RenderSprites();
		
		if(TUNNEL_ENABLE)
			DrawSceneNavigation(); 

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glDisable(GL_PROGRAM_POINT_SIZE);
		m_eularian_dirty_draw = true; 
		glViewport(0, 0, 1600, 900);

		DrawBuildingToFBO_FULL();

		drawValid = 0; 
	}
	void DrawBuildingToFBO_FULL()
	{
		m_eularianMapFBO_FullResolution.BindForWriting();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		
		m_drawBuildingsTech.Enable(); 
		m_BuildingGeometry->RenderSprites();
		
		if(TUNNEL_ENABLE)
			DrawSceneNavigation(); 

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DrawUncertaintyToFBO(int iterationnum)
	{
		/*if(iterationnum == 0)
		{
			m_uncertaintySmootherTech.SetFirstTimeUniform(1);
		}
		else{
			m_uncertaintySmootherTech.SetFirstTimeUniform(0);
		}*/
		m_eularianMapFBO_DownSampling.BindForImageReading(0); 
		m_uncertaintyIMO.BindForImageUnitReadWrite(1); 
		m_uncertaintySmootherTech.Enable(); 
		glDispatchCompute(128/16, 128/16, 1); 
	}
	void DrawNaviField(int id)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//m_drawNaviTech.Enable(); 

		//m_drawNaviTech.SetNaviTexUnit(0);
  //     
		//m_navigationTex[id]->Bind(COLOR_TEXTURE_UNIT);

		m_drawNaviArrayTech.Enable(); 
		m_drawNaviArrayTech.SetNaviTexArrayUnit(0);
		m_drawNaviArrayTech.SetGroupNumFloat(id); 
		m_navigationArrayTex->Bind(COLOR_TEXTURE_UNIT);

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	//in the Edit-Stroke Layer
	void DrawStroke()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_PROGRAM_POINT_SIZE);
        //glPointSize(32.0f);

		//DRAW BUILDING
		m_drawBuildingsTech.Enable(); 
		m_BuildingGeometry->RenderSprites();
		//DRAW AGENTS BUFFER
		m_drawAgentsBufferTech.Enable(); 
		m_uncertaintyIMO.BindForImageUnitReading(0);
		m_AgentBufferGeometry->DrawAgentsBuffer(); 
		//obsolete
		//m_drawAgentsTech.Enable(); 
		//m_MotionComputingGeometry->RenderAgentSprites();
		//Draw Navigation Scene
		if(TUNNEL_ENABLE)
			DrawSceneNavigation(); 

		//Draw Stroke
		m_drawStrokeTech.Enable(); 
		m_drawStrokeTech.SetActiveGroupFloat(m_active_navi_group);
		m_StrokeGeometry->RenderSprites();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_PROGRAM_POINT_SIZE);
	}

	void NaviBuildingUpdate()
	{
		//fetch the info from the NaviBuilingBufferGeometry
		int num = this->m_NaviBuildBufferGeometry->m_center.size(); 
		float cellpx, cellpz; 
		float width = 1/64.f; 
		float height = 11/64.f; 
		for(int i =0; i< num; i++)
		{
			cellpx = this->m_NaviBuildBufferGeometry->m_center.at(i).x; 
			cellpz = this->m_NaviBuildBufferGeometry->m_center.at(i).y; 

			Obstacle *obstacle = new Obstacle(cellpx, cellpz, width * WINDOW_WIDTH * 0.499, height * WINDOW_HEIGHT * 0.499); 
			obstacle->computeAABB(cellpx * 1600, cellpz * 900, width * WINDOW_WIDTH * 0.499, height * WINDOW_HEIGHT * 0.499); 
			ObstaclePtrList.push_back(obstacle); 
		}
	}

	void ProceedSceneEditor(int mouseLx, int mouseLy, bool left)
	{
		
		if(this->m_edit == true && this->m_editFinish == false && left == false && m_edit_navi == false)
		{
			static int firstime = 1; 
			if(firstime)
			{
				NaviBuildingUpdate(); 
				firstime = 0; 
			}
			//float rad = WINDOW_WIDTH*0.02 + RandomFloat()*(WINDOW_WIDTH*0.1 - WINDOW_WIDTH*0.02); 
			int cell_min = 1; 
			int cell_max = 3; 

			int xunit, zunit; 
			float xspan, zspan; 
			float width, height; 

			int cellx, cellz; 
			float cellpx, cellpz; 

			UtilHelper::PositionToCell(mouseLx, mouseLy, cellx, cellz); 
			UtilHelper::CellToPosition(cellx, cellz, cellpx, cellpz); 

			xunit = round(cell_min + RandomFloat()*(cell_max - cell_min)); 
			xunit = 2 * xunit +1 ; //make sure is odd
			zunit = round(cell_min + RandomFloat()*(cell_max - cell_min)); 
			zunit = 2 * zunit +1 ; 
			xspan = 1/float(UtilHelper::GridDimensionX);
			zspan = 1/float(UtilHelper::GridDimensionZ);


			width = xunit * xspan; 
			height = zunit * zspan; 

			m_BuildingManager->AddBuilding(mouseLx, mouseLy, width, height, 4, 4); 
			Obstacle *obstacle = new Obstacle(cellpx, cellpz, width * WINDOW_WIDTH * 0.499, height * WINDOW_HEIGHT * 0.499); 
			obstacle->computeAABB(cellpx, cellpz, width * WINDOW_WIDTH * 0.499, height * WINDOW_HEIGHT * 0.499); 
			ObstaclePtrList.push_back(obstacle); 

		//	BuildingEntry entry(mouseLx, mouseLy, width, height, cellpx, cellpz);
			if(READING_PROFILE == 0)
			{
				m_buildingEntryVector.push_back(BuildingEntry(mouseLx, mouseLy, width, height, cellpx, cellpz)); 
			}

			CallBack_NewBuilding(); 
			m_eularian_dirty = true; 
		}
	}

    void ProceedStroke_pressed(int mouseLx, int mouseLy, bool left, int State)
	{
		if(this->m_edit == true && this->m_editFinish == false && left == false && m_edit_navi == true)
		{
			m_StrokeManager->AddStroke(mouseLx, mouseLy, m_active_navi_group); 

			CallBack_Stroke(mouseLx, mouseLy, State); 
			//CallBack_NewBuilding(); 

			//m_eularian_dirty = true; 
		}
	}
    void ProceedStroke_released(int mouseLx, int mouseLy, bool left, int State)
	{
		if(this->m_edit == true && this->m_editFinish == false && left == false && m_edit_navi == true)
		{
			m_StrokeManager->AddStroke(-1, -1, m_active_navi_group); 
			m_sketchyMode = State == GLUT_DOWN ? true : false ; 
		}
	}

	//right mouse
	void CallBack_NewBuilding()
	{
		std::vector<BuildingVS> combineBuildings; 
		m_BuildingManager->ClinchBuildings(combineBuildings);

		if(m_BuildingGeometry->Initialized())
			m_BuildingGeometry->Update(combineBuildings); 
		else
			m_BuildingGeometry->Initialize(combineBuildings); 
	
		m_discomfortField2D->update(ObstaclePtrList); 
		m_SpriteTex_discomfort->UpdateDiscomfort(m_discomfortField2D);

	    //m_costField2D->clearAll(); 
		m_costField2D->init(*m_discomfortField2D); 
		m_SpriteTex_cost->UpdateCost(m_costField2D); 
	}
    void CallBack_FinishGuidanceField()
	{
		int count = this->m_StrokeManager->m_strokecollection.size(); 
		int pixelx, pixely; 

		int cellx, cellz; 

		int groupid; 
		int groupnum = -1; 
		for(int c =0 ; c<count; c++)
		{
			pixelx= this->m_StrokeManager->m_strokecollection.at(c).px * WINDOW_WIDTH; 
			pixely= this->m_StrokeManager->m_strokecollection.at(c).py * WINDOW_HEIGHT;
			groupid = this->m_StrokeManager->m_strokecollection.at(c).groupid; 

			if(groupid> groupnum)
			{
				groupnum = groupid; 
			}

			if(pixelx == -1 && pixely == -1)
			{
				this->m_SketchyInterface[groupid]->m_SketchyVisualTex.push_back(SketchGrid2D(-1, -1));// 
	            this->m_SketchyInterface[groupid]->segment++; 


				continue; 
			}

			UtilHelper::PositionToCell(pixelx, pixely, cellx, cellz);
			SketchGrid2D temp; 
			temp.x = pixelx; 
			temp.y = pixely; 
			temp.cx = cellx;
			temp.cz = cellz; 

			this->m_SketchyInterface[groupid]->m_SketchyVisualTex.push_back(temp); 
		}
		//detection intersection
		int prev_cx, prev_cz, curr_cz, curr_cx; 
		int prev_g, curr_g; 
		float prev_px, prev_pz, curr_px, curr_pz; 

		//EACH GROUP PROCESSED ONE BY ONE
		for(int g_id = 0; g_id <= groupnum; g_id++)
		{
			int stroke_ct = this->m_SketchyInterface[g_id]->m_SketchyVisualTex.size(); 
			for(int i =1; i<stroke_ct; i++)
			{
				prev_cx = this->m_SketchyInterface[g_id]->m_SketchyVisualTex.at(i-1).cx;
				prev_cz = this->m_SketchyInterface[g_id]->m_SketchyVisualTex.at(i-1).cz;
				curr_cx = this->m_SketchyInterface[g_id]->m_SketchyVisualTex.at(i).cx;
				curr_cz = this->m_SketchyInterface[g_id]->m_SketchyVisualTex.at(i).cz;

				prev_px = this->m_SketchyInterface[g_id]->m_SketchyVisualTex.at(i-1).x; 
				prev_pz = this->m_SketchyInterface[g_id]->m_SketchyVisualTex.at(i-1).y; 
				curr_px = this->m_SketchyInterface[g_id]->m_SketchyVisualTex.at(i).x; 
				curr_pz = this->m_SketchyInterface[g_id]->m_SketchyVisualTex.at(i).y; 

				if(prev_px == -1 || prev_pz == -1 || curr_px == -1 || curr_pz == -1)
				{
					continue; 
				}

				float gradientx = curr_px - prev_px; 
				float gradientz = curr_pz - prev_pz; 
				float oldgradientx, oldgradientz; 

				if(prev_cx != curr_cx || prev_cz != curr_cz)
				{
					//update gradient on both cell
					oldgradientx = this->m_SketchyInterface[g_id]->readAggSketchyGradient(prev_cx, prev_cz).gradientx; 
					oldgradientz = this->m_SketchyInterface[g_id]->readAggSketchyGradient(prev_cx, prev_cz).gradientz; 

					this->m_SketchyInterface[g_id]->writeAggSketchyGradient(prev_cx, prev_cz, SketchyGradient(oldgradientx +gradientx, 
						oldgradientz + gradientz));

			
					oldgradientx = this->m_SketchyInterface[g_id]->readAggSketchyGradient(curr_cx, curr_cz).gradientx; 
					oldgradientz = this->m_SketchyInterface[g_id]->readAggSketchyGradient(curr_cx, curr_cz).gradientz; 

					this->m_SketchyInterface[g_id]->writeAggSketchyGradient(curr_cx, curr_cz, SketchyGradient(oldgradientx +gradientx, 
						oldgradientz + gradientz));
				}
				else
				{
					gradientx += this->m_SketchyInterface[g_id]->readAggSketchyGradient(prev_cx, prev_cz).gradientx; 
					gradientz += this->m_SketchyInterface[g_id]->readAggSketchyGradient(prev_cx, prev_cz).gradientz;

					this->m_SketchyInterface[g_id]->writeAggSketchyGradient(prev_cx, prev_cz, SketchyGradient(gradientx, 
						gradientz));
				}
			}
		}

		//propagate Gradient looped by group
		for(int g_id = 0; g_id <= groupnum; g_id++)
		{
			for(int i = 0 ; i< UtilHelper::GridDimensionX; i++)
			{
				for(int j = 0; j< UtilHelper::GridDimensionZ; j++){
					float mag, gx, gz ; 
					gx = this->m_SketchyInterface[g_id]->readAggSketchyGradient(i, j).gradientx; 
					gz = this->m_SketchyInterface[g_id]->readAggSketchyGradient(i, j).gradientz; 
					mag = sqrt(gx*gx + gz*gz); 
					if(mag > 0.00001)
					{
						gx= gx/mag;
						gz = gz/mag; 
						this->m_SketchyInterface[g_id]->writeSketchyGradient(i, j, SketchyGradient(gx * 0.98, gz * 0.98, 1)); 
					}
				}
			}
		}

		//propagate
		int cellex, cellez; 
		int cellnx, cellnz; 
		int cellwx, cellwz; 
		int cellsx, cellsz; 

		for(int g_id = 0; g_id <= groupnum; g_id++)
		{
			for(int i =0; i< UtilHelper::GridDimensionX; i++)
			{
				for(int j =0; j< UtilHelper::GridDimensionZ; j++)
				{
					//
					cellex = i+1; 
					cellez = j; 

					cellnx = i; 
					cellnz = j+1; 

					cellwx = i-1;
					cellwz = j; 

					cellsx = i; 
					cellsz = j-1; 

					float sumgrad_x = 0.0f; 
					float sumgrad_z = 0.0f; 

					bool neighboring = false; 

					if(this->m_SketchyInterface[g_id]->readSketchyGradient(i, j).flag!=0){
						continue; 
					}

					if(UtilHelper::CellOutofBoundary(cellex, cellez) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).flag==1)
					{
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientz;
						neighboring = true; 
					}

					if(UtilHelper::CellOutofBoundary(cellnx, cellnz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).flag==1)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellwx, cellwz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).flag==1)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellsx, cellsz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).flag==1)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientz;
					}
					if( neighboring == true)
					{
						float sumgradmag = sqrt(sumgrad_x * sumgrad_x + sumgrad_z * sumgrad_z);

						if(sumgradmag > 0)
						{
						sumgrad_x /=sumgradmag; 
						sumgrad_z /=sumgradmag; 

						float coeff = exp(-1.0 * 1.0f * 0.5 ); 
				
						sumgrad_x *= coeff; 
						sumgrad_z *= coeff; 

						}
						this->m_SketchyInterface[g_id]->writeSketchyGradient(i, j, SketchyGradient(sumgrad_x, sumgrad_z, 2));
					}
				}
			}
		}//end of propogate 
		//start of second-order propogate
		for(int g_id = 0; g_id <= groupnum; g_id++)
		{
			for(int i =0; i< UtilHelper::GridDimensionX; i++)
			{
				for(int j =0; j< UtilHelper::GridDimensionZ; j++)
				{
					//
					cellex = i+1; 
					cellez = j; 

					cellnx = i; 
					cellnz = j+1; 

					cellwx = i-1;
					cellwz = j; 

					cellsx = i; 
					cellsz = j-1; 

					float sumgrad_x = 0.0f; 
					float sumgrad_z = 0.0f; 

					bool neighboring = false; 

					if(this->m_SketchyInterface[g_id]->readSketchyGradient(i, j).flag!=0){
						continue; 
					}

					if(UtilHelper::CellOutofBoundary(cellex, cellez) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).flag==2)
					{
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientz;
						neighboring = true; 
					}

					if(UtilHelper::CellOutofBoundary(cellnx, cellnz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).flag==2)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellwx, cellwz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).flag==2)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellsx, cellsz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).flag==2)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientz;
					}
					if( neighboring == true)
					{
						float sumgradmag = sqrt(sumgrad_x * sumgrad_x + sumgrad_z * sumgrad_z);

						if(sumgradmag > 0)
						{
						sumgrad_x /=sumgradmag; 
						sumgrad_z /=sumgradmag; 

						float coeff = exp(-1.2 * 1.2f * 0.5 ); 
				
						sumgrad_x *= coeff; 
						sumgrad_z *= coeff; 

						}
						this->m_SketchyInterface[g_id]->writeSketchyGradient(i, j, SketchyGradient(sumgrad_x, sumgrad_z, 3));
					}
				}
			}
		}//end of propogate 
		//start of third-order propogate
		for(int g_id = 0; g_id <= groupnum; g_id++)
		{
			for(int i =0; i< UtilHelper::GridDimensionX; i++)
			{
				for(int j =0; j< UtilHelper::GridDimensionZ; j++)
				{
					//
					cellex = i+1; 
					cellez = j; 

					cellnx = i; 
					cellnz = j+1; 

					cellwx = i-1;
					cellwz = j; 

					cellsx = i; 
					cellsz = j-1; 

					float sumgrad_x = 0.0f; 
					float sumgrad_z = 0.0f; 

					bool neighboring = false; 

					if(this->m_SketchyInterface[g_id]->readSketchyGradient(i, j).flag!=0){
						continue; 
					}

					if(UtilHelper::CellOutofBoundary(cellex, cellez) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).flag==3)
					{
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientz;
						neighboring = true; 
					}

					if(UtilHelper::CellOutofBoundary(cellnx, cellnz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).flag==3)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellwx, cellwz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).flag==3)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellsx, cellsz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).flag==3)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientz;
					}
					if( neighboring == true)
					{
						float sumgradmag = sqrt(sumgrad_x * sumgrad_x + sumgrad_z * sumgrad_z);

						if(sumgradmag > 0)
						{
						sumgrad_x /=sumgradmag; 
						sumgrad_z /=sumgradmag; 

						float coeff = exp(-1.6 * 1.6f * 0.5 ); 
				
						sumgrad_x *= coeff; 
						sumgrad_z *= coeff; 

						}
						this->m_SketchyInterface[g_id]->writeSketchyGradient(i, j, SketchyGradient(sumgrad_x, sumgrad_z, 4));
					}
				}
			}
		}//end of propogate 
		//start of fourth-order propogate
		for(int g_id = 0; g_id <= groupnum; g_id++)
		{
			for(int i =0; i< UtilHelper::GridDimensionX; i++)
			{
				for(int j =0; j< UtilHelper::GridDimensionZ; j++)
				{
					//
					cellex = i+1; 
					cellez = j; 

					cellnx = i; 
					cellnz = j+1; 

					cellwx = i-1;
					cellwz = j; 

					cellsx = i; 
					cellsz = j-1; 

					float sumgrad_x = 0.0f; 
					float sumgrad_z = 0.0f; 

					bool neighboring = false; 

					if(this->m_SketchyInterface[g_id]->readSketchyGradient(i, j).flag!=0){
						continue; 
					}

					if(UtilHelper::CellOutofBoundary(cellex, cellez) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).flag==4)
					{
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientz;
						neighboring = true; 
					}

					if(UtilHelper::CellOutofBoundary(cellnx, cellnz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).flag==4)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellwx, cellwz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).flag==4)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellsx, cellsz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).flag==4)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientz;
					}
					if( neighboring == true)
					{
						float sumgradmag = sqrt(sumgrad_x * sumgrad_x + sumgrad_z * sumgrad_z);

						if(sumgradmag > 0)
						{
						sumgrad_x /=sumgradmag; 
						sumgrad_z /=sumgradmag; 

						float coeff = exp(-2.2 * 2.2f * 0.5 ); 
				
						sumgrad_x *= coeff; 
						sumgrad_z *= coeff; 

						}
						this->m_SketchyInterface[g_id]->writeSketchyGradient(i, j, SketchyGradient(sumgrad_x, sumgrad_z, 5));
					}
				}
			}
		}//end of propogate 
		//start of fifth-order propogate
		for(int g_id = 0; g_id <= groupnum; g_id++)
		{
			for(int i =0; i< UtilHelper::GridDimensionX; i++)
			{
				for(int j =0; j< UtilHelper::GridDimensionZ; j++)
				{
					//
					cellex = i+1; 
					cellez = j; 

					cellnx = i; 
					cellnz = j+1; 

					cellwx = i-1;
					cellwz = j; 

					cellsx = i; 
					cellsz = j-1; 

					float sumgrad_x = 0.0f; 
					float sumgrad_z = 0.0f; 

					bool neighboring = false; 

					if(this->m_SketchyInterface[g_id]->readSketchyGradient(i, j).flag!=0){
						continue; 
					}

					if(UtilHelper::CellOutofBoundary(cellex, cellez) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).flag==5)
					{
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientz;
						neighboring = true; 
					}

					if(UtilHelper::CellOutofBoundary(cellnx, cellnz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).flag==5)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellwx, cellwz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).flag==5)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellsx, cellsz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).flag==5)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientz;
					}
					if( neighboring == true)
					{
						float sumgradmag = sqrt(sumgrad_x * sumgrad_x + sumgrad_z * sumgrad_z);

						if(sumgradmag > 0)
						{
						sumgrad_x /=sumgradmag; 
						sumgrad_z /=sumgradmag; 

						float coeff = exp(-2.6 * 2.6f * 0.5 ); 
				
						sumgrad_x *= coeff; 
						sumgrad_z *= coeff; 

						}
						this->m_SketchyInterface[g_id]->writeSketchyGradient(i, j, SketchyGradient(sumgrad_x, sumgrad_z, 6));
					}
				}
			}
		}//end of propogate 
		//start of sixth-order propogate
		for(int g_id = 0; g_id <= groupnum; g_id++)
		{
			for(int i =0; i< UtilHelper::GridDimensionX; i++)
			{
				for(int j =0; j< UtilHelper::GridDimensionZ; j++)
				{
					//
					cellex = i+1; 
					cellez = j; 

					cellnx = i; 
					cellnz = j+1; 

					cellwx = i-1;
					cellwz = j; 

					cellsx = i; 
					cellsz = j-1; 

					float sumgrad_x = 0.0f; 
					float sumgrad_z = 0.0f; 

					bool neighboring = false; 

					if(this->m_SketchyInterface[g_id]->readSketchyGradient(i, j).flag!=0){
						continue; 
					}

					if(UtilHelper::CellOutofBoundary(cellex, cellez) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).flag==6)
					{
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientz;
						neighboring = true; 
					}

					if(UtilHelper::CellOutofBoundary(cellnx, cellnz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).flag==6)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellwx, cellwz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).flag==6)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellsx, cellsz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).flag==6)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientz;
					}
					if( neighboring == true)
					{
						float sumgradmag = sqrt(sumgrad_x * sumgrad_x + sumgrad_z * sumgrad_z);

						if(sumgradmag > 0)
						{
						sumgrad_x /=sumgradmag; 
						sumgrad_z /=sumgradmag; 

						float coeff = exp(-3.0 * 3.0f * 0.5 );  
				
						sumgrad_x *= coeff; 
						sumgrad_z *= coeff; 

						}
						this->m_SketchyInterface[g_id]->writeSketchyGradient(i, j, SketchyGradient(sumgrad_x, sumgrad_z, 7));
					}
				}
			}
		}//end of propogate 
		//start of seventh-order propogate
		for(int g_id = 0; g_id <= groupnum; g_id++)
		{
			for(int i =0; i< UtilHelper::GridDimensionX; i++)
			{
				for(int j =0; j< UtilHelper::GridDimensionZ; j++)
				{
					//
					cellex = i+1; 
					cellez = j; 

					cellnx = i; 
					cellnz = j+1; 

					cellwx = i-1;
					cellwz = j; 

					cellsx = i; 
					cellsz = j-1; 

					float sumgrad_x = 0.0f; 
					float sumgrad_z = 0.0f; 

					bool neighboring = false; 

					if(this->m_SketchyInterface[g_id]->readSketchyGradient(i, j).flag!=0){
						continue; 
					}

					if(UtilHelper::CellOutofBoundary(cellex, cellez) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).flag==7)
					{
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellex, cellez).gradientz;
						neighboring = true; 
					}

					if(UtilHelper::CellOutofBoundary(cellnx, cellnz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).flag==7)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellnx, cellnz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellwx, cellwz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).flag==7)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellwx, cellwz).gradientz;
					}

					if(UtilHelper::CellOutofBoundary(cellsx, cellsz) == false 
						&&this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).flag==7)
					{
						neighboring = true; 
						sumgrad_x += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientx;
						sumgrad_z += this->m_SketchyInterface[g_id]->readSketchyGradient(cellsx, cellsz).gradientz;
					}
					if( neighboring == true)
					{
						float sumgradmag = sqrt(sumgrad_x * sumgrad_x + sumgrad_z * sumgrad_z);

						if(sumgradmag > 0)
						{
						sumgrad_x /=sumgradmag; 
						sumgrad_z /=sumgradmag; 

						float coeff = exp(-4.0 * 4.0f * 0.5 );  
				
						sumgrad_x *= coeff; 
						sumgrad_z *= coeff; 

						}
						this->m_SketchyInterface[g_id]->writeSketchyGradient(i, j, SketchyGradient(sumgrad_x, sumgrad_z, 8));
					}
				}
			}
			this->m_SketchyInterface[g_id]->copy(); 
		}//end of propogate 
	}

	void Update_NavigationField()
	{
		for(int i = 0; i< m_edit_navi_group.size(); i++)
		{
			this->m_navigationField2D[i]->clearAll();
			this->m_navigationField2D[i]->init_version2(*m_costField2D, *m_SketchyInterface[i]); 
		}
	}
	void Bake_NavigationTex()
	{
		for(int i = 0; i< m_edit_navi_group.size(); i++)
		{
			this->m_navigationTex[i]->Load(); 
		}

		this->m_navigationArrayTex->LoadArray(); 
		this->m_guidanceArrayTex->LoadArray(); 

	}
	void Bake_GuidanceTex()
	{

	}

	void CallBack_Stroke(int mouse_x, int mouse_y, int State)
	{
	    m_sketchyMode = State == GLUT_DOWN ? true : false ; 

		std::vector<StrokeVS> combineStrokes; 
		m_StrokeManager->ClinchStrokes(combineStrokes);

		if(m_StrokeGeometry->Initialized())
			m_StrokeGeometry->Update(combineStrokes); 
		else
			m_StrokeGeometry->Initialize(combineStrokes); 
	    /*_mousex = nwx; _mousey = nwy;*/
	}

	void CallBack_StrokeMove(int mouse_x, int mouse_y)
	{
		if(m_sketchyMode == false)
			return; 

		m_StrokeManager->AddStroke(mouse_x, mouse_y, m_active_navi_group); 
		 
		std::vector<StrokeVS> combineStrokes; 
		m_StrokeManager->ClinchStrokes(combineStrokes);

		if(m_StrokeGeometry->Initialized())
			m_StrokeGeometry->Update(combineStrokes); 
		else
			m_StrokeGeometry->Initialize(combineStrokes); 
	    /*_mousex = nwx; _mousey = nwy;*/
	}
	//clinch agents, no more edit on agents
	void CallBack_ClinchAgents()
	{
		std::vector<point_struct> combine_agents; 
        //Agent Manager Update
		m_AgentManager->ClinchAgents(combine_agents); 
		//Voronoi Draw Geometry Update
		m_AgentMesh->ClinchMesh(combine_agents); 
		//Voronoi Buffer Draw Geometry Update
		m_AgentBufferGeometry->Initialize(*m_AgentManager);

		//Motion Shader Pass Geometry Update
		//m_MotionComputingGeometry->Initialize(m_AgentManager->m_agentcollection); 

		//for debug purpose 
		ptr = new float[4 * combine_agents.size()]; 

		//
		int group_num = m_AgentManager->m_agentcollection.size(); 
		int real_group_num = group_num-UNCERTAINTY_GROUP;
		for(int i =0; i<real_group_num; i++)
		{
			m_edit_navi_group.push_back(false); 
			m_edit_Show_Guidance.push_back(false); 
			m_edit_Show_Navigation.push_back(false);
			m_edit_Show_NavigationTex.push_back(false); 
		}

		m_navigationField2D = new NavigationField2D*[real_group_num]; 
		m_SketchyInterface = new SketchyInterface*[real_group_num]; 
		m_SpriteTex = new Sprite*[real_group_num]; 
		m_navigationTex = new TextureData*[real_group_num]; 
		m_navigationArrayTex = new TextureArrayData(GL_TEXTURE_2D_ARRAY, m_navigationField2D, real_group_num); 
		m_guidanceArrayTex = new GuidanceTextureArray(GL_TEXTURE_2D_ARRAY, m_SketchyInterface, real_group_num); 


		float destmin[2]; 
		float destmax[2]; 
		int destmincell[2];
		int destmaxcell[2];
		std::vector<int> cellIDx; 
		std::vector<int> cellIDz; 
		for(int i =0; i<real_group_num; i++)
		{
			cellIDx.clear();
			cellIDz.clear();

			m_navigationField2D[i] = 
			new NavigationField2D(UtilHelper::GridDimensionX, UtilHelper::GridDimensionZ);

			m_navigationTex[i] = new TextureData(GL_TEXTURE_2D, m_navigationField2D[i]);

			destmin[0] = m_GoalMin[i].x;
			destmin[1] = m_GoalMin[i].y;
			destmax[0] = m_GoalMax[i].x;
			destmax[1] = m_GoalMax[i].y;

			UtilHelper::PositionToCell(destmin[0] * WINDOW_WIDTH, destmin[1] * WINDOW_HEIGHT, destmincell[0], destmincell[1]);
			UtilHelper::PositionToCell(destmax[0] * WINDOW_WIDTH, destmax[1] * WINDOW_HEIGHT, destmaxcell[0], destmaxcell[1]);

			for(int cx = destmincell[0]; cx <= destmaxcell[0]; cx++)
			{
				for(int cz = destmincell[1]; cz <= destmaxcell[1]; cz++)
				{
					cellIDx.push_back(cx); 
					cellIDz.push_back(cz); 
				}
			}

			m_navigationField2D[i]->InitializeGoalCell(cellIDx, cellIDz);

			m_SketchyInterface[i] = 
			new SketchyInterface(UtilHelper::GridDimensionX, UtilHelper::GridDimensionZ);

			m_SpriteTex[i] = 
				new Sprite(m_SketchyInterface[i], m_navigationField2D[i], false); 
		}

		//_mouse_sketchy.resize(group_num); 
	}

	void UpdateAgentState()
	{
		printf("start_UpdateAgentState\n"); 
		//1. position
		//2. goal direction
		float agent_x, agent_y; 
		float offset_x, offset_y; 
		float dir_x, dir_y; 
		int agent_id; 

		//Pointer_CollisionAvoidance

		for (int i = 0; i < m_AgentManager->m_agentcollection.size(); ++i) {
			for(int j = 0 ; j < m_AgentManager->m_agentcollection[i].size(); j++){
				float gx, gy; 
				gx = m_AgentManager->m_agentcollection[i][j].gx; 
				gy = m_AgentManager->m_agentcollection[i][j].gy; 

				agent_x = m_AgentManager->m_agentcollection[i][j].px; 
				agent_y = m_AgentManager->m_agentcollection[i][j].py; 
				agent_id =  m_AgentManager->m_agentcollection[i][j].agentid; 

				//group id and agent id
				if(Pointer_CollisionAvoidance[agent_id*4] == i && Pointer_CollisionAvoidance[agent_id*4 + 1] == agent_id)
				{

					offset_x = Pointer_CollisionAvoidance[agent_id*4 + 2]; 
					offset_y = Pointer_CollisionAvoidance[agent_id*4 + 3]; 

					if(abs(offset_x) >= 0 && abs(offset_x) <= 1){
						agent_x += offset_x ; 
						agent_y += offset_y; 

						Vector2f newdir(gx - agent_x, gy- agent_y); 
						newdir.Normalize(); 
				
						m_AgentManager->m_agentcollection[i][j].px = agent_x; 
						m_AgentManager->m_agentcollection[i][j].py = agent_y; 
						m_AgentManager->m_agentcollection[i][j].dirx = newdir.x; 
						m_AgentManager->m_agentcollection[i][j].diry = newdir.y; 

						float minx = 0.001526;
						float dx = gx- agent_x; 
						float dy = gy- agent_y;
						if((dx * dx + dy * dy) < minx*minx){
							m_AgentManager->m_agentcollection[i][j].px = m_AgentManager->m_agentcollection[i][j].initialpx; 
							m_AgentManager->m_agentcollection[i][j].py = m_AgentManager->m_agentcollection[i][j].initialpy; 

							if(m_AgentManager->m_agentcollection[i][j].groupid == 0)
							{
								int agentid = m_AgentManager->m_agentcollection[i][j].agentid; 
								int row = agentid / 25;
								int col = agentid % 25; 

								int pixelx = col * 20 + 560; 
								int pixely = 2.5 + 10 * row; 

								m_AgentManager->m_agentcollection[i][j].px = float(pixelx)/float(1600); 
								m_AgentManager->m_agentcollection[i][j].py = float(pixely)/float(900); 

							}
							else if(m_AgentManager->m_agentcollection[i][j].groupid == 2)
							{
								int agentid = m_AgentManager->m_agentcollection[i][j].agentid - 300; 
								int row = agentid / 25;
								int col = agentid % 25; 

								int pixelx = col * 20 + 560; 
								int pixely = 2.5 + 10 * row; 

								pixely = 900 - pixely; 

								m_AgentManager->m_agentcollection[i][j].px = float(pixelx)/float(1600); 
								m_AgentManager->m_agentcollection[i][j].py = float(pixely)/float(900); 
							}
							else if(m_AgentManager->m_agentcollection[i][j].groupid == 1)
							{
								int agentid = m_AgentManager->m_agentcollection[i][j].agentid - 150; 
								int col = agentid / 25;
								int row = agentid % 25; 

								int pixely = row * 20 + 210; 
								int pixelx = 2.5 + 10 * col; 

								m_AgentManager->m_agentcollection[i][j].px = float(pixelx)/float(1600); 
								m_AgentManager->m_agentcollection[i][j].py = float(pixely)/float(900); 
							}
							else if(m_AgentManager->m_agentcollection[i][j].groupid == 3)
							{
								int agentid = m_AgentManager->m_agentcollection[i][j].agentid - 450; 
								int col = agentid / 25;
								int row = agentid % 25; 

								int pixely = row * 20 + 210; 
								int pixelx = 1.5 + 10 * col; 

								pixelx = 1600-pixelx;

								m_AgentManager->m_agentcollection[i][j].px = float(pixelx)/float(1600); 
								m_AgentManager->m_agentcollection[i][j].py = float(pixely)/float(900); 
							}

							m_AgentManager->m_agentcollection[i][j].smax *= 0.0;  
		/*					m_AgentManager->m_agentcollection[i][j].px = -1; 
							m_AgentManager->m_agentcollection[i][j].py = -1; */
						}
					}
				}
			}
		}
		std::vector<point_struct> combine_agents; 

		m_AgentManager->ClinchAgents(combine_agents); 
				
		m_AgentMesh->UpdateMesh(combine_agents);

		m_MotionComputingGeometry->Update(m_AgentManager->m_agentcollection); 

		printf("end_UpdateAgentState\n"); 
	}

	void TransferAgentPosFromGPU()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_AgentBufferGeometry->PositionBuffer());
		float* fPointer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, AGENT_NUM * 2 * sizeof(float), GL_MAP_READ_BIT);
		float _position[2]; 
		int groupID; 
		int agentID; 
		for(int i = 0 ; i< AGENT_NUM; i++)
		{
			groupID = (i/150); 
			agentID = i - groupID * 150; 
			_position[0] = fPointer[2 * i + 0]; 
			_position[1] = fPointer[2 * i + 1]; 	
			m_AgentManager->m_agentcollection[groupID][agentID].px = _position[0];
			m_AgentManager->m_agentcollection[groupID][agentID].py = _position[1];
		
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		std::vector<point_struct> combine_agents; 

		m_AgentManager->ClinchAgents(combine_agents); 
				
		m_AgentMesh->UpdateMesh(combine_agents);
	}

	void RenderSampledMarkers()
	{
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, m_markerSamplingFBO.getFBO());
		//
	 //   glReadBuffer(GL_COLOR_ATTACHMENT0);
		//glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
  //                        0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		RenderMarkerSamplingDifference();
	}

	void RenderMarkerSamplingDifference()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		m_textureSubTech.Enable(); 
		
		m_markerSamplingBufferFBO.BindForReading(GL_TEXTURE0);

		m_markerSamplingBufferFBO.BindForReading(GL_TEXTURE1); 

		m_FullScreenQuad->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//void RenderPass3()
	//{
	//			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_motionComputingFBO.getFBO());
	//    glReadBuffer(GL_COLOR_ATTACHMENT0);
	//	  glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
 //                         0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	//	  glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}

	//void RenderPass4()
	//{
	//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_collisionAvoidanceFBO.getFBO());
	//    glReadBuffer(GL_COLOR_ATTACHMENT0);
	//	  glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
 //                         0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	//	   glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//}
	
	//void DrawCircle(int px, int py, int rad, int segment)
	//{
	//	//draw to buffer
	//	glBegin( GL_TRIANGLE_FAN );
	//	glMultiTexCoord2f(GL_TEXTURE0, x, y);
	//	glVertex2f(x, y);
 //       for( int n = 0; n <= segments; ++n ) {
 //           float const t = 2*M_PI*(float)n/(float)segments;
	//		glMultiTexCoord2f(GL_TEXTURE0, x + sin(t)*r, y + cos(t)*r);
 //           glVertex2f(x + sin(t)*r, y + cos(t)*r);
 //       }
	//	glEnd();
	//}
    
  //  LightingTechnique m_LightingTech;
   // SilhouetteTechnique m_silhouetteTech;
	FullQuadTechnique m_fullQuadTech; 
   // MarkerSamplingTechnique m_markersamplingTech; 
	//MarkerSamplingDebugTechnique m_markersamplingDebugTech; 
	MarkerSamplingBufferTechnique m_markersamplingBufferTech; 
	//MotionComputingTechnique m_motionComputingTech; 
	//CollisionAvoidanceTechnique m_collsionAvoidingTech; 
	//DrawAgentsSpriteTechnique m_drawAgentsTech; 
	DrawBuildingSpriteTechnique m_drawBuildingsTech; 
	DrawStrokeSpriteTechnique m_drawStrokeTech; 
	DrawNaviFullScreenTechnique m_drawNaviTech; 
	DrawNaviFullScreenArrayTechnique m_drawNaviArrayTech; 
	DensityVelocitySplatterTechnique m_splatterDensityVelocityTech; 
    FutureDiscomfortSplatterTechnique m_splatterFutureDiscomfortTech; 
	AverageVelocityCalculatorTechnique m_averageVelocityCalcTech; 
	SpeedCalculatorTechnique m_speedCalculatorTech; 
	CostCalculatorTechnique m_costCalculatorTech; 
	PotentialInitTechnique m_potentialInitTech; 
    PotentialCalcTechnique m_potentialCalculatorTech; 
	GradientCalcTechnique m_gradientCalculatorTech; 
	AgentAdvectionTechnique m_agentAdvectionTech; 
	DrawAgentsBufferTechnique m_drawAgentsBufferTech; 
	DrawMarkerBufferTechnique m_drawMarkersBufferTech; 
	CollisionDetectionTechnique m_collisionDetectionTech; 
	TextureSubTechnique m_textureSubTech; 
	UncertaintySmootherTechnique m_uncertaintySmootherTech; 
	DrawNaviBuildingTechnique m_drawNaviBuildingTech; 
	DrawNaviMaskTechnique m_drawNaviMaskTech;
	MaskNaviGuidTechnique m_maskNaviGuidTech; 
	DrawMaskedNaviGuidTechnique m_drawMaskedNaviGuidTech; 


	//Buffer Geometry
	//VoronoiTessallationTechnique m_voronoiTessallationTech; 


    Camera* m_pGameCamera;
    float m_scale;
   //DirectionalLight m_directionalLight;
   //Vector3f m_boxPos;
   //Mesh m_mesh;

    struct {
        bool IsPressed;
        int x;
        int y;
    } m_leftMouseButton;

	struct {
        bool IsPressed;
        int x;
        int y;
    } m_rightMouseButton;

	struct {
        bool IsPressed;
        int x;
        int y;
    } m_midMouseButton;

    PersProjInfo m_persProjInfo;


    int m_glutTime;
    long long m_startTime;
    int m_frameCount;
    float m_fps;    

	int m_mousex, m_mousey; 
	bool m_wasMouseOverMenu;
	bool m_mouseOverMenu; 
	int m_mouseScroll;
	unsigned char m_mouseBut;
	bool m_showMenu; 
	int m_propScroll;
	float weight1, weight2, weight3;
	bool m_showTools; 

	//
	bool m_drawUncertainty; 
	bool m_drawFutureDiscomfort; 
	bool m_drawCost; 
	bool m_drawMask; 
	bool m_drawMaskedNavi; 
	bool m_drawMaskedGuid; 

	bool m_randomMarker;
	bool m_randomMarkerFinished; 
	bool m_randomAgent;
	bool m_randomAgentFinished; 
	bool m_clinchAgent; 
	bool m_clinchAgentFinished; 
	bool m_renderVoronoiGS; 
	bool m_drawAgent; 
	bool m_showVoronoiAgent; 
	bool m_showEularianMap; 
	bool m_showGuidanceMap; 
	bool m_showNaviMap; 
	bool m_showGuidanceMapSub[4];
	bool m_showNaviMapSub[4]; 
	//per timestep
	bool m_launchSimulation; 

	bool m_drawMarkers; 
	bool m_drawMarkerSampling; 
	//bool m_drawMotionVector; 
	//bool m_drawFinalMotionVector; 

	//edit
	bool m_edit;
	bool m_editFinish;
	bool m_edit_Show;
	bool m_edit_navi; 
	bool m_edit_navi_Finish; 
	bool m_edit_ShowEularian; 
	std::vector<bool> m_edit_Show_Guidance; 
	int  m_active_showguidance_group; 
	std::vector<bool> m_edit_Show_Navigation; 
	int  m_active_shownavigation_group; 
	std::vector<bool> m_edit_Show_NavigationTex; 
	int  m_active_shownavigation_groupTex; 
	bool m_edit_ShowDiscomfort; 
	bool m_edit_ShowCostField; 


	std::vector<bool> m_edit_navi_group; 
	int  m_active_navi_group; 
	std::vector<std::vector<int>> m_mouse_sketchy;  //always reflect the current stroking 

	bool m_sketchyMode; //
	bool m_edit_Show_Crowd; 
    



	bool m_main; 

	MarkerMesh* m_MarkerMesh;
	AgentMesh* m_AgentMesh; 
	MarkerGeometry* m_MarkerGeometry; 
	MotionComputingGeometry* m_MotionComputingGeometry;
	BuildingGeometry* m_BuildingGeometry; 
	StrokeGeometry* m_StrokeGeometry; 
	AgentBufferGeometry* m_AgentBufferGeometry; 
	MarkerBufferGeometry* m_MarkerBufferGeometry; 
	FullScreenQuad* m_FullScreenQuad; 
	NavigationBuildingBufferGeometry* m_NaviBuildBufferGeometry; 

	float* ptr; 
	int m_groupnum; 
	bool m_eularian_dirty; 
	bool m_eularian_dirty_draw; 

	//Framebuffer for Composite Voronoi Map
	MarkerMapFBO m_markerMapFBO;
    //Framebuffer for Agent Voronoi Map
	AgentMapFBO m_agentMapFBO; 

	
	//Framebuffer for Sampling the MarkerMap
	//MarkerSamplingFBO m_markerSamplingFBO; 
	//MarkerSamplingFBO m_markerSamplingDebugFBO; 
	MarkerSamplingBufferFBO m_markerSamplingBufferFBO;


	//Input Geometry: Agent Point Set
	//MotionComputingFBO m_motionComputingFBO;
	//Input Geometry: Agent Point Set
	//CollisionAvoidanceFBO m_collisionAvoidanceFBO; 
	//Environment Setting
	EularianMapFBO m_eularianMapFBO_FullResolution; 
	EularianMapFBO m_eularianMapFBO_DownSampling; 
	//density & velocity Splatter
	DensityVelocityFBO m_densityVelocityFBO; 
	//speed image buffer
	SpeedImageObject m_speedIMO; 
	CostImageObject m_costIMO; 
	PotentialImageObject m_potentialIMO; 
	GradientImageObject m_gradientIMO12;
	GradientImageObject m_gradientIMO34;
	UncertaintyImageObject m_uncertaintyIMO; 
	NaviMaskFBO m_naviMaskFBO; 
	MaskedNaviGuidImageObject m_maskedNaviGuidIMO; 



	AgentManager* m_AgentManager; 
	BuildingManager* m_BuildingManager; 
	StrokeManager* m_StrokeManager; 
	NavigationField* m_NavigationField; 

	//Horde3d Old class
	std::vector<ObstaclePtr> ObstaclePtrList; 
	DiscomfortNavigationField2D* m_discomfortField2D; 
	CostNavigationField2D* m_costField2D; 
	NavigationField2D** m_navigationField2D; 
	SketchyInterface** m_SketchyInterface; 
	Sprite** m_SpriteTex; 
	Sprite* m_SpriteTex_discomfort; 
	Sprite* m_SpriteTex_cost; 
	//
	TextureData** m_navigationTex; 
	TextureArrayData* m_navigationArrayTex; 
	GuidanceTextureArray* m_guidanceArrayTex; 


	std::vector<Vector2f> m_RegionMin; 
	std::vector<Vector2f> m_RegionMax; 

	std::vector<Vector2f> m_GoalMin; 
	std::vector<Vector2f> m_GoalMax; 

	std::vector<float> m_test1; 
	std::vector<float> m_test2; 
	bool GPUCopy; 
	bool PrintMotionVector; 


	//local copy
	//MarkerSampling Pass
	float* Pointer_MarkerSampling;
	MarkerArray *MarkerAgentStats; 
	float* Pointer_MarkerSamplingDebug; 
	MarkerArray *MarkerAgentStatsDebug; 
	float* Pointer_MotionSampling; 
	float* Pointer_CollisionAvoidance; 
	float* Pointer_NavigationField[4]; 
	int* Pointer_DensityField; 

	//sphere
	Texture* m_pTexture;

	//building entry
	std::vector<BuildingEntry> m_buildingEntryVector; 
	bool m_LoadFromFile; 

};


int main(int argc, char** argv)
{
    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Tutorial 39")) {
        return 1;
    }
    
	if (!imguiRenderGLInit("DroidSans.ttf"))
	{
		//printf("Could not init recast GUI renderer.\n");
		return -1;
	}

    SRANDOM;
    
    Tutorial39* pApp = new Tutorial39();

    if (!pApp->Init()) {
        return 1;
    }
    
    pApp->Run();

    delete pApp;
 
    return 0;
}