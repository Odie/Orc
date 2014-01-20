require 'bootstrap'

local sdl = require("sdl2")

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

local ren = sdl.createRenderer(win, -1, sdl.RENDERER_ACCELERATED + sdl.RENDERER_PRESENTVSYNC);
if ren == nil then
	print("[SDL] could not create renderer")
	return
end

local bmp = sdl.loadBMP(assetDir .. "smiley.bmp")
if bmp == nil then
	print("[SDL] could not load bitmap")
	return
end

local tex = sdl.createTextureFromSurface(ren, bmp);
sdl.freeSurface(bmp);
if tex == nil then
	print("[SDL] could not create texture from surface")
	return
end

sdl.renderClear(ren);
sdl.renderCopy(ren, tex, nil, nil);
sdl.renderPresent(ren);

sdl.delay(3*1000);

sdl.destroyTexture(tex);
sdl.destroyRenderer(ren);
sdl.destroyWindow(win);
sdl.quit();
