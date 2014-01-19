package.path = "./lib/?.lua;./lib/?/init.lua;" .. package.path

-- string.split
-- Splits a string using the given deliminater
function string.split(text, delimiter)
	local list = {}
	local pos = 1
	if string.find("", delimiter, 1) then -- this would result in endless loops
		error("delimiter matches empty string!")
	end
	while 1 do
		local first, last = string.find(text, delimiter, pos)
		if first then -- found?
			table.insert(list, string.sub(text, pos, first-1))
			pos = last+1
		else
			table.insert(list, string.sub(text, pos))
			break
		end
	end
	return list
end

-- ffi.load override
-- The default ffi.load loads from a specific path.
-- We'd like it to search through a list of configurable locations.
local ffi = require("ffi")

ffi.searchPath = "./lib;"
local ffiLoad = ffi.load
ffi.load = function(libName)
	local paths = string.split(ffi.searchPath, ';')

	-- Iterate through all configured paths
	for _,path in ipairs(paths) do

		-- Try building the full path to the dynamic library using these patterns
		local patterns = {
			"%s/%s",
			"%s/lib%s.dylib"
		}

		-- Try each of the patterns at each fo the configured paths
		for _,pattern in ipairs(patterns) do
			local libPath = string.format(pattern, path, libName)
			local status, err = pcall(ffiLoad, libPath)

			-- If we've succeeded, return the loaded lib
			if status then
				local lib = err
				return lib
			end

			-- Otherwise, try again
		end
	end
end

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

local bmp = sdl.loadBMP("assets/smiley.bmp")
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
