--- startup.lua

function do_error(s)
	print(debug.traceback())
	return s
end

function init()
	
	dofile(system.GetExecDir() .. system.PathDiv() .. "res" .. system.PathDiv() .. "global.lua")

	gui.init()
	do_module("info.lmod")

	main = do_module( "launch_mod.lua" )
	main.start()
end

function update()
	if main.isDone()
	then
		system.Quit()
	end
	
	system.Delay(10)
end

function free()
	layer.free()
	gui.free()
end
