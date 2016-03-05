/*
        Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARKERSAMPLINGDEBUG_TECHNIQUE_H
#define	MARKERSAMPLINGDEBUG_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class MarkerSamplingDebugTechnique : public Technique {
public:

    MarkerSamplingDebugTechnique();

    virtual bool Init();

	void SetMarkersSamplingTextureUnit(unsigned int TextureUnit); 
	void SetWriteMarkerIDTextureUnit(unsigned int TextureUnit);

private:

	GLuint m_MarkersSamplingTextureUnitLocation; 
	GLuint m_WriteMarkerIDUnitLocation;  

	GLuint SamplerName[1]; // 3 TEXTURE NEED TO SAMPLE
};


#endif