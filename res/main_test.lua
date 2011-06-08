-- main.lua

local mod = dofile("mod_base.lua")

-- routines

function mod.init()

	diag     = dofile("diag.lua")
	listview = dofile("listview.lua")

	mod.w = mod.sw
	mod.h = mod.sh
	
	mod.win = gui.createWindow( 0 , 16 , mod.w , mod.h - 16 )

	local box = gui.createBox ( 0 , 0 , mod.w , mod.h - 16)

	local button

	for i = 0 , 5
	do
		button = gui.createButton ( 30 + ( i * 40 ) , 30 , 32 , 32 , tostring(i) )
		gui.appendChild ( box , button )
	end
	
	gui.setFocus  ( mod.win , button )
	gui.viewFocus ( mod.win , true )

	gui.append ( mod.win , box )
	

	mod.call = nil
end

function mod.checkKey()

	if key.Left()
	then
		gui.prevFocus( mod.win )
	end
	
	if key.Right()
	then
		gui.nextFocus( mod.win )
	end
	
	if key.A()
	then
		local obj = gui.getFocusObj ( mod.win )
		gui.setClick( obj , true )
		gui.update ( mod.win )
		
		diag.start()
		diag.setMessage( "Hello,World!!" )
	
		mod.call = diag					
	end
	
	if key.B()
	then
		listview.start()
		listview.setTextBody ( { "Apple","Banana" } )
		mod.call = listview
	end
	
	if key.Select()
	then
		mod.quit = true
	end

	if key.Start() then
		system.CallLua("keyboard.lua")
	end
end

function mod.checkNotify()

	if mod.call and mod.call.isDone()
	then
		local obj = gui.getFocusObj ( mod.win )
		gui.setClick( obj , false )
		gui.update ( mod.win )
		
		mod.call.reset()
		
		mod.call = nil
	end

	if system.BackNotify()
	then
		if system.ResultNotify() 
		then
			str = system.GetResultString()
			print("str = ",str)
		end
		system.ClearNotify()
		mod.ignoreKey()
	end
end

return mod
