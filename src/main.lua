require 'bootstrap'
local gl = require 'graphics.OpenGL'
local sdl = require 'sdl2'
local ffi = require 'ffi'

if sdl == nil then
	print("[SDL] ffi could not be loaded")
	return
end

local initResult = sdl.init(sdl.INIT_EVERYTHING)
if initResult == -1 then
	print("[SDL] initialization failed")
	return
end

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

-- now you can make GL calls.
gl.glClearColor(255, 255, 255,1)
gl.glClear(gl.GL_COLOR_BUFFER_BIT)
sdl.gL_SwapWindow(win)

-- Once finished with OpenGL functions, the SDL_GLContext can be deleted.
sdl.gL_DeleteContext(glContext)
sdl.destroyWindow(win);
sdl.quit();
