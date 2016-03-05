#include <stdio.h>
#include "SpriteTex.h"
#include <fstream>
#include <sstream>
#include "Windows.h"
#include <GL/glew.h>
#include <GL/freeglut.h>


Sprite::Sprite(SketchyInterface* sketchy, bool pure_texture)
: _id(0), _pure_texture(pure_texture), _guidancefield(true), _navigationfield(false)
{
	  _sketchyInterface = sketchy; 
	 
	// Make a texture object and bind it.
	  glGenTextures(1, &_id);
	  glBindTexture(GL_TEXTURE_2D, _id);

	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
	  unsigned int width, height;
  
	  width = height = (int)sqrt(float(sketchy->m_SketchyFlagTex.size()));

	  this->Data = new unsigned char[width * height * 4]; 
  
	  int value; 

	  int gridx, gridz; 

	  char debugstring[256]; 
	  for(int i = 0 ; i< width * height; i++)
	  {
		  gridx = i/width; 

		  gridz = i-gridx*width; 

		  //gridx = width -1 -gridx; 

		  float gradx, gradz; 

		  gradx = sketchy->readSketchyGradient(gridx, gridz).gradientx; 
		  gradz = sketchy->readSketchyGradient(gridx, gridz).gradientz; 

		  float gradmag = sqrt(gradx * gradx + gradz * gradz); 

		  this->Data[4*i + 0]  = int(gradmag *255);
		//  this->Data[4*i + 0] = 255;
		  this->Data[4*i + 1] = 0; 
		  this->Data[4*i + 2] = 0;
		  this->Data[4*i + 3] = 255; 

		 // if(sketchy->readSketchyGradient(gridx, gridz).gradientx !=0 || sketchy->readSketchyGradient(gridx, gridz).gradientz!=0){
			// sprintf( debugstring, 
			//	"gridx: %d, gridz:%d, gradientx: %f, gradientz: %f\n",gridx, gridz, 
			//	sketchy->readSketchyGradient(gridx, gridz).gradientx, 
			//	sketchy->readSketchyGradient(gridx, gridz).gradientz);

			//OutputDebugString(debugstring);
		 // }
	  }
	  // Upload the pixel data.
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
				   0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(Data));
}


Sprite::Sprite(DiscomfortNavigationField2D* discomfortfield, bool pure_texture):
  _id(0), _pure_texture(pure_texture), _guidancefield(false),
	_navigationfield(false)
  {
	// Make a texture object and bind it.
	  glGenTextures(1, &_id);
	  glBindTexture(GL_TEXTURE_2D, _id);

	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
	  unsigned int width, height;
  
	  width = height = (int)sqrt(float(discomfortfield->m_discomfortNavigationField2D.size()));

	  this->Data = new unsigned char[width * height * 4]; 
  
	//  int value; 

	//  int gridx, gridz; 

	//  //char debugstring[256]; 

	//  discomfortfield->Normalize(); 

	//  for(int i = 0 ; i< width * height; i++)
	//  {
	//	  gridx = i/width; 

	//	  gridz = i-gridx*width; 

	//	  //gridx = width -1 -gridx; 

	//	  float discomfort;  
	//	  discomfort = discomfortfield->readDiscomfortNavigationField2D(gridx, gridz);
	//	  if(discomfort == FLT_MAX){
	//		  discomfort = 1.0f; 
	//	  }

	//	  this->Data[4*i + 0] = int(discomfort *255);
	//	  this->Data[4*i + 1] = int(discomfort *255); 
	//	  this->Data[4*i + 2] = int(discomfort *255);
	//	  this->Data[4*i + 3] = 255; 

	///*	  if(sketchy->readSketchyGradient(gridx, gridz).gradientx !=0 || sketchy->readSketchyGradient(gridx, gridz).gradientz!=0){
	//		sprintf( debugstring, 
	//			"gridx: %d, gridz:%d, gradientx: %f, gradientz: %f\n",gridx, gridz, 
	//			sketchy->readSketchyGradient(gridx, gridz).gradientx, 
	//			sketchy->readSketchyGradient(gridx, gridz).gradientz);

	//		OutputDebugString(debugstring);
	//	  }*/
	//  }

	//  discomfortfield->DeNormalize(); 

	//  // Upload the pixel data.
	//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
	//			   0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(Data));

}

Sprite::Sprite(CostNavigationField2D* cost, bool pure_texture):
  _id(0), _pure_texture(pure_texture), _guidancefield(false),
	_navigationfield(false)
  {
	// Make a texture object and bind it.
	  glGenTextures(1, &_id);
	  glBindTexture(GL_TEXTURE_2D, _id);

	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
	  unsigned int width, height;
  
	  width = height = (int)sqrt(float(cost->m_costNavigationField.size()));

	  this->Data = new unsigned char[width * height * 4];
}
Sprite::Sprite(SketchyInterface* sketchy, NavigationField2D* navigationfield2D, bool pure_texture): _id(0), _pure_texture(pure_texture), _guidancefield(false),
	_navigationfield(true)
{
	_sketchyInterface = sketchy; 
	_navigationfield2D  = navigationfield2D;
	// Make a texture object and bind it.
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
	unsigned int width, height;
  
	width = height = (int)sqrt(float(navigationfield2D->m_navigationfield2D.size()));

	this->Data = new unsigned char[width * height * 4]; 
  
	int value; 

	int gridx, gridz; 

	char debugstring[256]; 
	for(int i = 0 ; i< width * height; i++)
	{
		gridx = i/width; 

		gridz = i-gridx*width; 

		//gridx = width -1 -gridx; 

		float gradx, gradz; 
		int rgba[4] = {255, 255, 255, 255}; 

		gradx = navigationfield2D->readPathCost(gridx, gridz).naviX; 
		gradz = navigationfield2D->readPathCost(gridx, gridz).naviZ; 

		if(gradx > 0 && gradz > 0)
		{
			rgba[0] = 255; 
			rgba[1] = 0; 
			rgba[2] = 0; 
			rgba[3] = 255; 
		}
		if(gradx < 0 && gradz >0)
		{
			rgba[0] = 0; 
			rgba[1] = 255; 
			rgba[2] = 0; 
			rgba[3] = 255; 
		}
		if(gradx < 0 && gradz <0)
		{
			rgba[0] = 0; 
			rgba[1] = 0; 
			rgba[2] = 255; 
			rgba[3] = 255; 
		}
		if(gradx > 0 && gradz <0)
		{
			rgba[0] = 255; 
			rgba[1] = 255; 
			rgba[2] = 0; 
			rgba[3] = 255; 
		}

	

		this->Data[4*i + 0]  = rgba[0];
	//  this->Data[4*i + 0] = 255;
		this->Data[4*i + 1] =rgba[1];
		this->Data[4*i + 2] = rgba[2];
		this->Data[4*i + 3] = rgba[3];  
	}


	// Upload the pixel data.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
				0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(Data));
}

Sprite::~Sprite()
{
  // Release the texture object.
   delete[] Data; 
   glDeleteTextures(1, &_id);
}


void Sprite::UpdateDiscomfort(DiscomfortNavigationField2D* discomfortfield)
{
	  glBindTexture(GL_TEXTURE_2D, _id);

	  unsigned int width, height;
  
	  width = height = (int)sqrt(float(discomfortfield->m_discomfortNavigationField2D.size()));
  
	  int value; 

	  int gridx, gridz; 

	  //discomfortfield->Normalize(); 

	  for(int i = 0 ; i< width * height; i++)
	  {
		  gridx = i/width; 

		  gridz = i-gridx*width; 

		  //gridx = width -1 -gridx; 

		  float discomfort;  
		  discomfort = discomfortfield->readDiscomfortNavigationField2D(gridz, gridx);
		  if(discomfort == FLT_MAX){
			  discomfort = 1.0f; 
		  }

		  this->Data[4*i + 0] = int(discomfort *255);
		  this->Data[4*i + 1] = int(discomfort *255); 
		  this->Data[4*i + 2] = int(discomfort *255);
		  this->Data[4*i + 3] = 255; 

	/*	  if(sketchy->readSketchyGradient(gridx, gridz).gradientx !=0 || sketchy->readSketchyGradient(gridx, gridz).gradientz!=0){
			sprintf( debugstring, 
				"gridx: %d, gridz:%d, gradientx: %f, gradientz: %f\n",gridx, gridz, 
				sketchy->readSketchyGradient(gridx, gridz).gradientx, 
				sketchy->readSketchyGradient(gridx, gridz).gradientz);

			OutputDebugString(debugstring);
		  }*/
	  }

	  //discomfortfield->DeNormalize(); 

	  // Upload the pixel data.
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
				   0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(Data));

}
void Sprite::UpdateCost(CostNavigationField2D* costfield)
{
	  glBindTexture(GL_TEXTURE_2D, _id);

	  unsigned int width, height;
  
	  width = height = (int)sqrt(float(costfield->m_costNavigationField.size()));
  
	  int value; 

	  int gridx, gridz; 

	  //discomfortfield->Normalize(); 

	  for(int i = 0 ; i< width * height; i++)
	  {
		  gridx = i/width; 

		  gridz = i-gridx*width; 

		  //gridx = width -1 -gridx; 

		  float cost;  
		  cost = costfield->readCostMac(gridz, gridx).at(0);
		  //if(discomfort == FLT_MAX){
			 // discomfort = 1.0f; 
		  //}

		  this->Data[4*i + 0] = int(cost *255);
		  this->Data[4*i + 1] = int(cost *255); 
		  this->Data[4*i + 2] = int(cost *255);
		  this->Data[4*i + 3] = 255; 

	/*	  if(sketchy->readSketchyGradient(gridx, gridz).gradientx !=0 || sketchy->readSketchyGradient(gridx, gridz).gradientz!=0){
			sprintf( debugstring, 
				"gridx: %d, gridz:%d, gradientx: %f, gradientz: %f\n",gridx, gridz, 
				sketchy->readSketchyGradient(gridx, gridz).gradientx, 
				sketchy->readSketchyGradient(gridx, gridz).gradientz);

			OutputDebugString(debugstring);
		  }*/
	  }

	  //discomfortfield->DeNormalize(); 

	  // Upload the pixel data.
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
				   0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(Data));

}
void Sprite::Draw(float x, float y, float w, float h, bool navi)
{
	if(this->_pure_texture == true)
	{
		  // Set_Texturing(1);
		  // Bind the texture of this sprite and draw a quad with it.
		  glEnable(GL_TEXTURE_2D);
		  glBindTexture(GL_TEXTURE_2D, _id);
		  glBegin(GL_QUADS);
		  glTexCoord2f(0, 0);
		  glVertex2f(x/w, y/h);
		  glTexCoord2f(1, 0);
		  glVertex2f((x + w)/w, y/h);
		  glTexCoord2f(1, 1);
		  glVertex2f((x + w)/w, (y + h)/h);
		  glTexCoord2f(0, 1);
		  glVertex2f(x/h, (y + h)/h);
		  glEnd();
		  glDisable(GL_TEXTURE_2D);
	}
	else
	{
		float posx, posz; 
		float zpadding = w/UtilHelper::GridDimensionX; 
		float xpadding = h/UtilHelper::GridDimensionZ; 

	    //draw the grid
		glLineWidth(1.f);
		for(int i = 0 ; i< UtilHelper::GridDimensionX; i++)
		{
			for(int j = 0; j< UtilHelper::GridDimensionZ; j++)
			{
				glBegin(GL_LINE_LOOP);
				//left bottom 
				posx = xpadding * i; 
				posx/=h;
				posz = zpadding * j; 
				posz/=w;
			    glVertex2f(posz, posx);
				glColor4f(1,1,1,1);

				glVertex2f(posz + zpadding, posx);
				glColor4f(1,1,1,1);

				glVertex2f(posz + zpadding, posx + xpadding);
				glColor4f(1,1,1,1);

				glVertex2f(posz,posx + xpadding);
				glColor4f(1,1,1,1);

				glEnd();
			}
		}

		_guidancefield = navi == true? false:true; 
		if(_guidancefield){

			int ct = _sketchyInterface->m_SketchyVisualTex.size();
			SketchGrid2D temp; 

			float scalez = h/UtilHelper::GraphicsDimensionZ; 
			float scalex = w/UtilHelper::GraphicsDimensionX; 

			glLineWidth(5.f);

			int currenti = 0; 
			int numberofsegement = _sketchyInterface->segment; 
			
			while(numberofsegement>0){

				glBegin(GL_LINE_STRIP);

				for(int i = currenti; i<ct; i++)
				{ 
					if(_sketchyInterface->m_SketchyVisualTex[i].x == -1 
						&&_sketchyInterface->m_SketchyVisualTex[i].y == -1)
					{
						glEnd();

						float colorBig[4] = {1, 0, 0, 1};
						DrawArrow(w, h, _sketchyInterface->m_SketchyVisualTex[i - 10].x, 
						_sketchyInterface->m_SketchyVisualTex[i - 10].y,  _sketchyInterface->m_SketchyVisualTex[i - 1].x, 
						_sketchyInterface->m_SketchyVisualTex[i - 1].y, scalex, scalez, 10.0f, colorBig); 
						//glLineWidth(2.f);
						numberofsegement--; 

						currenti = i+1; 
						break;
					}

						temp = _sketchyInterface->m_SketchyVisualTex[i]; 
						//temp.x += UtilHelper::GraphicsDimensionX * 0.5; 
						//temp.y += UtilHelper::GraphicsDimensionZ * 0.5; 

						temp.x *= scalex; 
						temp.y *= scalez; 

						glVertex2f(temp.x/w, temp.y/h);
						glColor4f(1,0,0,1);

					/*	temp = _sketchyInterface->m_SketchyVisualTex[2*i + 1]; 
						temp.x += UtilHelper::GraphicsDimensionX * 0.5; 
						temp.y += UtilHelper::GraphicsDimensionZ * 0.5; 

						temp.x *= scalex; 
						temp.y *= scalez; 

						glVertex2f(temp.y, temp.x);
						glColor4f(1,0,0,1);*/

				}
				glEnd();
			}

			glLineWidth(1.3f);
			for(int i = 0 ; i< UtilHelper::GridDimensionX; i++)
			{
				for(int j = 0; j< UtilHelper::GridDimensionZ; j++)
				{
					float cx, cz; 
					UtilHelper::CellToPosition(i,j,cx,cz); 


					float gx = _sketchyInterface->readSketchyGradientCopy(i, j).gradientx; 
					float gz = _sketchyInterface->readSketchyGradientCopy(i, j).gradientz; 

					float gmag = sqrt(gx * gx + gz * gz); 

					if(gmag == 0.0f){

						continue; 
					}

					float scale = gmag * 10.f; 
					gx = gx/gmag; 
					gz = gz/gmag; 

					float x1, z1, x2, z2;
					x1 = cx + gx * scale;
					z1 = cz + gz * scale; 
					x2 = cx - gx * scale; 
					z2 = cz - gz * scale; 

					glBegin(GL_LINES);

				//	x1 += UtilHelper::GraphicsDimensionX * 0.5; 
				//	z1 += UtilHelper::GraphicsDimensionZ * 0.5; 

					x1 *= scalex; 
					z1 *= scalez; 

					glVertex2f(x1/w, z1/h);
					glColor4f(1,1,0,1);

					//
				//	x2 += UtilHelper::GraphicsDimensionX * 0.5; 
				//	z2 += UtilHelper::GraphicsDimensionZ * 0.5; 

					x2 *= scalex; 
					z2 *= scalez; 

					glVertex2f(x2/w, z2/h);
					glColor4f(1,1,0,1);

					glEnd();

					//draw arrow
					float colorsmall[4] = {1, 1, 0,1};
					DrawArrow(w, h, cx, cz, cx + gx * scale, cz + gz * scale, scalex, scalez, 0.5 * scale, colorsmall); 

				}
			}
		}
		else if(_navigationfield){

			int ct = _sketchyInterface->m_SketchyVisualTex.size();
			SketchGrid2D temp; 
			

			float scalez = h/UtilHelper::GraphicsDimensionZ; 
			float scalex = w/UtilHelper::GraphicsDimensionX; 

			glLineWidth(5.f);
			int currenti = 0; 
			int numberofsegement = _sketchyInterface->segment; 
			
			while(numberofsegement>0){

				glBegin(GL_LINE_STRIP);

				for(int i = currenti; i<ct; i++)
				{ 
					if(_sketchyInterface->m_SketchyVisualTex[i].x == -1 
						&&_sketchyInterface->m_SketchyVisualTex[i].y == -1)
					{
						glEnd();

						float colorBig[4] = {1, 0, 0, 1};
						DrawArrow(w, h, _sketchyInterface->m_SketchyVisualTex[i - 10].x, 
						_sketchyInterface->m_SketchyVisualTex[i - 10].y,  _sketchyInterface->m_SketchyVisualTex[i - 1].x, 
						_sketchyInterface->m_SketchyVisualTex[i - 1].y, scalex, scalez, 10.0f, colorBig); 
						glLineWidth(5.f);
						numberofsegement--; 

						currenti = i+1; 
						break;
					}
					glLineWidth(2.f);
						temp = _sketchyInterface->m_SketchyVisualTex[i]; 
						//temp.x += UtilHelper::GraphicsDimensionX * 0.5; 
						//temp.y += UtilHelper::GraphicsDimensionZ * 0.5; 

						temp.x *= scalex; 
						temp.y *= scalez; 

						glVertex2f(temp.x/w, temp.y/h);
						glColor4f(1,0,0,1);

					/*	temp = _sketchyInterface->m_SketchyVisualTex[2*i + 1]; 
						temp.x += UtilHelper::GraphicsDimensionX * 0.5; 
						temp.y += UtilHelper::GraphicsDimensionZ * 0.5; 

						temp.x *= scalex; 
						temp.y *= scalez; 

						glVertex2f(temp.y, temp.x);
						glColor4f(1,0,0,1);*/

				
				}


				glEnd();
			}

			//////WRITE TO FILE
			//std::ofstream outf;
			//outf.open("Navigation.txt", std::ios::out );
			//if( !outf.good() )
			//{
			//	return;
			//}

			//float gradientx, gradientz; 
			//for(int i = 0; i< UtilHelper::GridDimensionX; i++)
			//{
			//	for(int j = 0; j< UtilHelper::GridDimensionZ; j++)
			//	{
			//		float gradientx = _navigationfield2D->readState(i, j).navix; 
			//		float gradientz = _navigationfield2D->readState(i, j).naviz;

			//		outf << gradientx << "\n";
			//		outf << gradientz << "\n";
			//	}
			//}


			//outf.close();


			glLineWidth(1.3f);
			for(int i = 0 ; i< UtilHelper::GridDimensionX; i++)
			{
				for(int j = 0; j< UtilHelper::GridDimensionZ; j++)
				{
					float cx, cz; 
					UtilHelper::CellToPosition(i,j,cx,cz); 


					//float gx = _navigationfield2D->readPathCost(i, j).naviX; 
					//float gz = _navigationfield2D->readPathCost(i, j).naviZ; 


					float gx = _navigationfield2D->readState(i, j).navix; 
					float gz = _navigationfield2D->readState(i, j).naviz;

					float gmag = sqrt(gx * gx + gz * gz); 

					if(gmag == 0.0f){
						continue; 
					}

					float scale = gmag * 10.f; 
					gx = gx/gmag; 
					gz = gz/gmag; 

					float x1, z1, x2, z2;
					x1 = cx + gx * scale;
					z1 = cz + gz * scale; 
					x2 = cx - gx * scale; 
					z2 = cz - gz * scale; 

					glBegin(GL_LINES);

					//x1 += UtilHelper::GraphicsDimensionX * 0.5; 
					//z1 += UtilHelper::GraphicsDimensionZ * 0.5; 

					x1 *= scalex; 
					z1 *= scalez; 

					glVertex2f(x1/w, z1/h);
					glColor4f(1,1,0,1);

					//
					//x2 += UtilHelper::GraphicsDimensionX * 0.5; 
					//z2 += UtilHelper::GraphicsDimensionZ * 0.5; 

					x2 *= scalex; 
					z2 *= scalez; 

					glVertex2f(x2/w, z2/h);
					glColor4f(1,1,0,1);

					glEnd();

					//draw arrow
					float colorsmall[4] = {1, 1, 0,1};
					DrawArrow(w, h, cx, cz, cx + gx * scale, cz + gz * scale, scalex, scalez, 2.5f, colorsmall); 
				}
			}
		}
	}
}
void Sprite::DrawArrow(float w, float h, float sourcex, float sourcey, float destx, float desty, float scalex, float scalez, float scale, float color[4])
{
	//Draw Arrow
	float x1,x2,y1, y2;

	x1 = destx; 
	y1 = desty; 

	x2 = sourcex; 
	y2 = sourcey; 

	float vectorx1, vectory1; 
	float vectorx2, vectory2; //orthonormal
			
	vectorx1 = x1 - x2; 
	vectory1 = y1 - y2; 

	vectorx2 = -vectory1;
	vectory2 = vectorx1; 

	float vectormag = sqrt(vectorx1 * vectorx1 + vectory1 * vectory1);

	vectorx1/=vectormag; 
	vectory1/=vectormag; 

	vectorx2/=vectormag; 
	vectory2/=vectormag; 

	float arrowstartx = x1 - 2 * scale * vectorx1;
	float arrowstarty = y1 - 2 * scale * vectory1;

	float arrowx1 = arrowstartx + vectorx2 * scale; 
	float arrowy1 = arrowstarty + vectory2 * scale; 
	float arrowx2 = arrowstartx - vectorx2 * scale; 
	float arrowy2 = arrowstarty - vectory2 * scale; 

	glBegin(GL_LINES);

	//destx += UtilHelper::GraphicsDimensionX * 0.5; 
	//desty += UtilHelper::GraphicsDimensionZ * 0.5; 

	destx *= scalex; 
	desty *= scalez; 

	glVertex2f(destx/w, desty/h);
	glColor4f(color[0],color[1],color[2],color[3]);

	//arrowx1 += UtilHelper::GraphicsDimensionX * 0.5; 
	//arrowy1 += UtilHelper::GraphicsDimensionZ * 0.5; 
	arrowx1*= scalex; 
	arrowy1*= scalez; 

	glVertex2f(arrowx1/w, arrowy1/h);
	glColor4f(color[0],color[1],color[2],color[3]);
	glEnd(); 

	glBegin(GL_LINES);

	glVertex2f(destx/w, desty/h);
	glColor4f(color[0],color[1],color[2],color[3]);


	//arrowx2+= UtilHelper::GraphicsDimensionX * 0.5; 
	//arrowy2 += UtilHelper::GraphicsDimensionZ * 0.5; 
	arrowx2*= scalex; 
	arrowy2*= scalez; 

	glVertex2f(arrowx2/w, arrowy2/h);

	glColor4f(color[0],color[1],color[2],color[3]);
	glEnd(); 
}