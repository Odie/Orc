#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include <OpenGL/gl3.h>

#include <glm/glm.hpp>
using namespace glm;

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

typedef struct{
	SDL_Window* window;
	SDL_GLContext glContext;
} Globals;

Globals globals;

void logError(const char* format, ...)
{
	va_list argList;

	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);
}

void logMessage(const char* format, ...)
{
	va_list argList;

	va_start(argList, format);
	vfprintf(stdout, format, argList);
	va_end(argList);
}

void handleKeystrokes()
{
	SDL_Event event;

	/* Poll for events */
	while(SDL_PollEvent(&event)){

		switch(event.type){
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE)
					exit(0);
				break;

			/* SDL_QUIT event (window close) */
			case SDL_QUIT:
				exit(0);
				break;

			default:
				break;
		}
	}
}

void engineLoop()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	while(true){
		handleKeystrokes();

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );
	}
}

void shutdown()
{
	logMessage("Exiting...\n");

	//-----------------------------------------------------------------------------
	// Shutdown everything
	//-----------------------------------------------------------------------------
	SDL_GL_DeleteContext(globals.glContext);
	SDL_DestroyWindow(globals.window);
	SDL_Quit();
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


	SDL_Window* window = globals.window = SDL_CreateWindow("Orc", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if(window == NULL){
		logError("[SDL] create window failed:  %s\n", SDL_GetError());
		return 1;
 	}

	SDL_GLContext glContext = globals.glContext = SDL_GL_CreateContext(window);
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
	printf("%s\n", version);


	//-----------------------------------------------------------------------------
	// Setup app exit handler
	//-----------------------------------------------------------------------------
	atexit(shutdown);

	//-----------------------------------------------------------------------------
	// Run the orc engine and do interesting stuff!
	//-----------------------------------------------------------------------------
	engineLoop();

	exit(0);
}
