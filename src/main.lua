require 'bootstrap'
local gl = require 'graphics.OpenGL'
local sdl = require 'sdl2'
local ffi = require 'ffi'

-------------------------------------------------------------------------------
-- Initialize GL
-------------------------------------------------------------------------------
if sdl == nil then
	print("[SDL] ffi could not be loaded")
	return
end

local initResult = sdl.init(sdl.INIT_EVERYTHING)
if initResult == -1 then
	print("[SDL] initialization failed")
	return
end

-- Ask for an openGL 3.2 context
sdl.gL_SetAttribute(sdl.GL_CONTEXT_MAJOR_VERSION, 3);
sdl.gL_SetAttribute(sdl.GL_CONTEXT_MINOR_VERSION, 2);
sdl.gL_SetAttribute(sdl.GL_CONTEXT_PROFILE_MASK, sdl.GL_CONTEXT_PROFILE_CORE);

sdl.gL_SetAttribute(sdl.GL_DOUBLEBUFFER, 1);
sdl.gL_SetAttribute(sdl.GL_DEPTH_SIZE, 24);

local win = sdl.createWindow("Hello World!", 100, 100, 640, 480, sdl.WINDOW_SHOWN);
if win == nil then
	print("[SDL] create window failed")
	return
end

glContext = sdl.gL_CreateContext(win)
if glContext == nil then
	print("[SDL] could not create opengl context");
	return
end

-------------------------------------------------------------------------------
-- Report openGL version
-------------------------------------------------------------------------------
majorVersion = ffi.new("GLint[1]")
minorVersion = ffi.new("GLint[1]")
gl.glGetIntegerv(gl.GL_MAJOR_VERSION, majorVersion);
gl.glGetIntegerv(gl.GL_MINOR_VERSION, minorVersion);
print(string.format("Using openGL %i.%i", majorVersion[0], minorVersion[0]))

version = gl.glGetString(gl.GL_VERSION)
version = ffi.string(version)
print(version)

-------------------------------------------------------------------------------
-- Do some stuff
-------------------------------------------------------------------------------
gl.glClearColor(255, 255, 255,1)
gl.glClear(gl.GL_COLOR_BUFFER_BIT)
sdl.gL_SwapWindow(win)

sdl.delay(3*1000);

-------------------------------------------------------------------------------
-- Cleanup
-------------------------------------------------------------------------------
sdl.gL_DeleteContext(glContext)
sdl.destroyWindow(win);
sdl.quit();
