#pragma once

#ifndef __OpenGL_H__
#define __OpenGL_H__

#include <GL/glew.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include "glext.h"
#include <fltk/gl.h>
//#include "wglext.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Utilities/Data.h>

namespace SnowSim {
	namespace Utilities {
		namespace OpenGL {

			class Shaders {

				/* Some more global variables for convenience. */
				static double t0;
				static int frames;
				static char titlestring[200];

				static GLuint permTextureID;
				static GLuint gradTextureID;
				static GLuint sphereList;
				static GLboolean updateTime;
				static GLboolean animateObject;

				static GLhandleARB programObj;
				static GLhandleARB vertexShader;
				static GLhandleARB fragmentShader;
				static GLint      location_permTexture;
				static GLint      location_gradTexture;
				static GLint      location_time;

				static const char *vertexShaderStrings[1];
				static const char *fragmentShaderStrings[1];
				static GLint vertexCompiled;
				static GLint fragmentCompiled;
				static GLint shadersLinked;
				static char str[4096]; // For error messages from the GLSL compiler and linker

				static int perm[256];

				/* These are Ken Perlin's proposed gradients for 3D noise. I kept them for
				   better consistency with the reference implementation, but there is really
				   no need to pad this to 16 gradients for this particular implementation.
				   If only the "proper" first 12 gradients are used, they can be extracted
				   from the grad4[][] array: grad3[i][j] == grad4[i*2][j], 0<=i<=11, j=0,1,2
				*/
				static int grad3[16][3];

				/* These are my own proposed gradients for 4D noise. They are the coordinates
				   of the midpoints of each of the 32 edges of a tesseract, just like the 3D
				   noise gradients are the midpoints of the 12 edges of a cube.
				*/
				static int grad4[32][4];

				/*
				 * readShaderFile(filename) - read a shader source string from a file
				 */
				static unsigned char* readShaderFile(const char *filename);


				/*
				 * createShaders() - create, load, compile and link the GLSL shader objects.
				 */
				static void createShaders();

			};


			bool extInit();

			 /* printError() - Signal an error. MessageBox() is a Windows-specific function,
			 * rewrite this to print the error message to the console to make it portable!
			 */
			static void printError(const char *errtype, const char *errmsg) {
			  // fprintf(stderr, "%s: %s\n", errtype, errmsg);
			  //MessageBoxA(NULL, errmsg, errtype, MB_OK|MB_ICONEXCLAMATION);
				std::cout << "[Error] " << errmsg << "(Type: " << errtype << ")" << std::endl;
			}
		}
	}
}

#endif
