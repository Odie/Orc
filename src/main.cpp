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

GLuint loadShader(GLenum shaderType, const char* filePath)
{
	GLuint shaderID = glCreateShader(shaderType);

	// read the entire shader file
	std::ifstream shaderFile(filePath);
	if(!shaderFile.is_open()){
		logError("[Shader] Couldn't open file:  %s\n", filePath);
		return 0;
	}

	std::stringstream buffer;
	buffer << shaderFile.rdbuf();

	const char* shaderContents = buffer.str().c_str();


	// compile the shader
	logMessage("[Shader] Compiling:  %s\n", filePath);
	glShaderSource(shaderID, 1, &shaderContents , NULL);
	glCompileShader(shaderID);

	// report any shader compiliation errors
	GLint result = GL_FALSE;
	int infoLogLength;

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0){
		std::vector<char> errorMessage(infoLogLength + 1);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, &errorMessage[0]);
		logError("[Shader] Compile error: %s", &errorMessage[0]);
		return 0;
	}

	// return the id of the compiled shader
	return shaderID;
}

GLuint loadShaderProgram(const char * vertexFilePath,const char * fragmentFilePath){

	// load the shaders
	GLuint vertShaderID = loadShader(GL_VERTEX_SHADER, vertexFilePath);
	GLuint fragShaderID = loadShader(GL_FRAGMENT_SHADER, fragmentFilePath);
	if(vertShaderID == 0 || fragShaderID == 0)
		return 0;

	// Link the program
	logMessage("[Shader] linking program\n");

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertShaderID);
	glAttachShader(programID, fragShaderID);
	glLinkProgram(programID);

	// report any program link errors
	GLint result = GL_FALSE;
	int infoLogLength;

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0){
		std::vector<char> errorMessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errorMessage[0]);
		logError("[Shader] Link error: %s", &errorMessage[0]);
	}

	glDeleteShader(vertShaderID);
	glDeleteShader(fragShaderID);

	return programID;
}

void handleKeystrokes()
{
	SDL_Event event;

	/* Poll for events */
	while(SDL_PollEvent(&event)){

		switch(event.type){
			/* Keyboard event */
			/* Pass the event data onto PrintKeyInfo() */
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Load the shader
	GLuint programID = loadShaderProgram("assets/basic.vertexshader", "assets/basic.fragmentshader");

	// Setup the model to display
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glUseProgram(programID);

	static const GLfloat vertexData[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glBindVertexArray(0);

	while(true){
		handleKeystrokes();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Swap buffers
		SDL_GL_SwapWindow(globals.window);
	}

	// TODO!!!
	// Cleanup programID and vertexArrayID
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
