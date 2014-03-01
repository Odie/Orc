#include <stdio.h>
#include "SDL.h"
#include <OpenGL/gl3.h>

void logError(const char* format, ...)
{
	va_list argList;

	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);
}

int main()
{

	//-----------------------------------------------------------------------------
	// Initialize SDL
	//-----------------------------------------------------------------------------

	// Don't initialize the audio for now.
	// on OSX, SDL reports "no available audio device" for some reason.
	int result = SDL_Init( SDL_INIT_EVERYTHING & ~SDL_INIT_AUDIO );

	if(result != 0){
		logError("[SDL] initialization failed:  %s\n", SDL_GetError());
		return 1;
	}


	//-----------------------------------------------------------------------------
	// Initialize GL
	//-----------------------------------------------------------------------------
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


	SDL_Window* window = SDL_CreateWindow("Orc", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if(window == NULL){
		logError("[SDL] create window failed:  %s\n", SDL_GetError());
		return 1;
 	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if(glContext == NULL){
		logError("[SDL] could not create opengl context:  %s\n", SDL_GetError());
		return 1;
	}


	//-----------------------------------------------------------------------------
	// Report openGL version
	//-----------------------------------------------------------------------------
	int majorVersion;
	int minorVersion;
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
	printf("Using openGL %i.%i\n", majorVersion, minorVersion);

	char* version = (char*)glGetString(GL_VERSION);
	printf("%s", version);


	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
