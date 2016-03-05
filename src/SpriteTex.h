#ifndef SPRITETEX_H
#define SPRITETEX_H

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "SketchyInterface.h"
#include "NavigationField2D.h"
#include "DiscomfortNavigationField2D.h"
#include "CostNavigationField2D.h"

// Basically encapsulates a texture and makes it easy to draw it. Not
// highly efficient to do it this way, but for this game it works.
class Sprite
{
public:
  // The texture type will be deducted from the file type. If the type
  // is grayscale and pure_alpha is true the texture will be an alpha
  // texture instead of a grayscale texture.
  Sprite(SketchyInterface* sketchy, bool pure_texture); 
  Sprite(SketchyInterface* sketchy, NavigationField2D* navigationfield2D, bool pure_texture); 

  Sprite(DiscomfortNavigationField2D* discomfort, bool pure_texture); 
  Sprite(CostNavigationField2D* cost, bool pure_texture); 
  ~Sprite();

  void UpdateDiscomfort(DiscomfortNavigationField2D* discomfortfield); 
  void UpdateCost(CostNavigationField2D* cost); 
  void Draw(float x, float y, float w, float h, bool navi);
  void DrawSpeed(float x, float y, float w, float h); //four arrow, direction is solid, scale to represent size 
  void DrawArrow(float w, float h, float sourcex, float sourcey, float destx, float desty, float scalex, float scalez, float scale, float color[4]); 
  
private:
  // The OpenGL texture object id.
  unsigned int _id;
  unsigned char *Data;
  bool _pure_texture; 
  bool _guidancefield; 
  bool _navigationfield; 

  SketchyInterface *_sketchyInterface; 
  NavigationField2D* _navigationfield2D;
  DiscomfortNavigationField2D *_discomfortfield2D; 
};

#endif //SPRITETEX_H
