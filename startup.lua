--- startup.lua

-- エラー発生時に呼び出される
function do_error(s)
	print(debug.traceback())
	return s
end

-- 初期化
function init()
	
	dofile(system.GetExecDir() .. system.PathDiv() .. "lmod" .. system.PathDiv() .. "global.lua")

	gui.init()
	do_module("info.layer")

	main = do_execute( "filer.lexe" )
	main.start()
end

-- 更新
function update()
	if main.isDone()
	then
		system.Quit()
	end
	
	system.Delay(10)
end

-- 開放
function free()
	layer.free()
	gui.free()
end
