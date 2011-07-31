-- global.lua

-- File stuff

local sysdir = system.GetExecDir()
local pathdiv = system.PathDiv()
local conf_dir = sysdir .. pathdiv .. "conf"

local resource_dir = sysdir .. pathdiv .. "res"
local lexe_dir = sysdir .. pathdiv .. "lexe"
local lmod_dir = sysdir .. pathdiv .. "lmod"
local lsub_dir = sysdir .. pathdiv .. "lsub"


function do_luafile( path , name )
	local module = dofile ( path )
	
	if name == nil then
		name = file_name ( path )	
	end
	
	if type(module) == "table" then
		module.moduleFile = name
	end
	return module
end

function do_execute( name )
	return do_luafile ( file_lexefile( name ) , name )
end


function do_module( name )
	return do_luafile ( file_lmodfile( name ) , name )
end

function do_submod( name )
	return do_luafile ( file_lsubfile( name ) , name )
end



-- get full path of file

function file_lexefile( name )
	return lexe_dir .. pathdiv .. name
end

function file_lmodfile( name )
	return lmod_dir .. pathdiv .. name
end

function file_lsubfile( name )
	return lsub_dir .. pathdiv .. name
end

function file_resfile( name )
	return resource_dir .. pathdiv .. name
end

function file_respath( path )
	return file_resfile( path )
end


-- get file part of the path

function file_name ( path )
	pattern = string.format( ".*%s(.*)" , pathdiv )
	return string.match ( path , pattern )
end

-- get file part of the path

function file_dir ( path )
	pattern = string.format( "(.*)%s.*" , pathdiv )
	return string.match ( path , pattern )
end

-- get base part of the filename
function file_basename ( name )
	return string.match( name , "([^\.]*)..*" )
end

-- Get extension of the filename
function file_ext ( name )
	return  string.match( name ,"[^\.]*\.(.*)")
end


-- Object handling

function copy_object ( srcobj )
	destobj = {}
	for k,v in ipairs( srcobj ) 
	do
		destobj[k] = v
	end
	
	return destobj
end


function disp_object ( srcobj )
	print("disp_object",srcobj)

	for k,v in pairs( srcobj ) 
	do
		print(":",k,v)
	end
end

-- Layer stuff

layer = {}
layer.table = {}

function layer.register(t)
	table.insert( layer.table , t )	
	t.init()
end

function layer.update()
	flag = false
	for k,v in pairs(layer.table) 
	do 
		if v.update() then flag = true end
	end
	return flag
end


function layer.updateForce()
	for k,v in pairs(layer.table) do v.updateForce() end
end


function layer.free()
	for k,v in pairs(layer.table) do v.free() end
end

-- Font stuff

font = {}

function font.load( size )

	local fontfile = "VLGothic" .. system.PathDiv() .."VL-Gothic-Regular.ttf"
	local ttf = graph.OpenFont( file_resfile(fontfile) , size )
	if not ttf then
		error("TTF couldn't be loaded!!")
	end
	return(ttf)
end

function font.close( ttf )
	if ttf then
		graph.CloseFont( ttf )
	end
end

-- Key Repeater Stuff
keyRepeat  = {}

function keyRepeat.new()
	local obj = {}
	obj.count = system.GetTick()
	obj.pastRead = key.Read()
	
	obj.repeat1 = 300
	obj.repeat2 = 25
	obj.step = 0
	
	keyRepeat.reset( obj )
	
	return obj
end

function keyRepeat.reset( obj )
	obj.count = system.GetTick() + obj.repeat1	
	obj.pastRead = key.Read()
end

function keyRepeat.isUpdate( obj )
	local tick = system.GetTick()
	local read = key.Read()
	
	if read ~= obj.pastRead 
	then
		obj.pastRead = read
		obj.count    = tick
	end

	if read ~= 0
	then
		if obj.count <= tick 
		then
			if obj.step == 0 then
				obj.count = tick + obj.repeat1
				obj.step = 1
			elseif obj.step == 1 then
				obj.count = tick + obj.repeat2
			end
			return true
		end
	else
		obj.step = 0
		obj.count = tick
	end
	return false
end

function keyRepeat.pressAnyKey ( obj )
	obj.read = key.Read()
	
	if obj.read == 0
	then
		return false
	end
	
	return true
end

-- Config stuff

config = {}

function config.getConfigPath ( name )
	if not system.IsExist( conf_dir ) then
		system.MkDir( conf_dir )
	end
	
	return conf_dir .. pathdiv .. name
end

function config.indent( level )
	local i
	local str = ""

	for i = 1,level 
	do
		str = str .. "  "
	end
	
	return str
end

function config.tableToString ( table , lv )

    local str = ""
	local vstr = ""
	

    for k,v in pairs ( table )
    do

		if str ~= "" 
		then 
			str = str .. ",\n" .. config.indent(lv)
		else 
			str = config.indent(lv)
		end

	
		if type(v) == "table"
		then 

			vstr = "\{\n" .. config.tableToString ( v , lv+1 ) .. "\n\}"
			
		elseif type(v) == "number" then
			vstr = string.format( "%d" , v )
		else
			vstr = string.format( "%q" , v )
		end

		if type(k) == "number"
		then
			str = str .. string.format( "[%d] = %s" , k , vstr )
		else
			str = str .. string.format( "%s = %s" , k , vstr ) 
		end
 
    end
	
	return str
end

function config.save( file , table )
	local str = config.tableToString( table , 0 )

	fp = io.open( file , "w" )
	
	if not fp then
		return
	end
	
	fp:write( str )
	fp:write( "\n" )
	io.close( fp )
end

function config.load( file )
	fp = io.open( file , "r" )
	
	if not fp then
		return {}
	end

	local str = ""
	
	for line in fp:lines()
	do
		str = str .. line
	end
	
	io.close( fp )
	
	return loadstring("return \{" .. str .. "\}")()
end

function config.test ()

	local t = {
		"x",
		"y",
		128,
		{
			2,4,8,16,32,64,128
		},
		this = 15,
		who = "ami"
	}

	print("table")
	print(config.tableToString( t , 0 ))


	config.save( "test.conf" , t )
	local newtbl  = config.load( "test.conf" )
	
	   
	print("from file")
	print(config.tableToString( newtbl , 0 ))

end


-- GUI stuff

gui = {}
gui.draw = {}

-- Initialize
function gui.init()

	if not gui.font then
		gui.font = {}
		gui.font.small  = font.load ( 10 )
		gui.font.normal = font.load ( 15 )
		gui.font.large  = font.load ( 20 )
	end

	if not gui.color then
		gui.color = {}
		gui.color.disable = graph.Color( 0x30,0x30,0x30 )
		
		gui.color.white = graph.Color( 0xff,0xff,0xff )
		gui.color.red   = graph.Color( 0xff,0x00,0x00 )
		gui.color.blue  = graph.Color( 0x00,0x00,0xff )
		gui.color.green = graph.Color( 0x00,0xff,0x00 )
		gui.color.black = graph.Color( 0x00,0x00,0x00 )

		
		gui.color.back = { 
			graph.Color( 0xcc,0xcc,0xcc ),
			graph.Color( 0xaa,0xaa,0xaa ),
			graph.Color( 0x55,0x55,0x55 )
		}
		
		gui.color.focus = {
			graph.Color( 0xff,0xff,0xff ),
			graph.Color( 0xf0,0xf0,0x80 ),
			graph.Color( 0x80,0x80,0x60 )
		}
		
		gui.color.clicked = {
			graph.Color( 0xff,0xff,0xff ),
			graph.Color( 0x40,0x40,0x20 ),
			graph.Color( 0x00,0x00,0x00 )
		}
				
		gui.color.box     = graph.Color( 0x00,0x00,0x00 )
		gui.color.text    = graph.Color( 0x00,0x00,0x00 )

		gui.color.bright = graph.Color( 0x80,0x80,0x80 )
		gui.color.dark   = graph.Color( 0x20,0x20,0x20 )
		
		gui.color.scroll = {}
		gui.color.scroll.bright = graph.Color( 0xff,0xff,0xff )
		gui.color.scroll.dark   = graph.Color( 0xa0,0xa0,0xa0 )
		gui.color.scroll.back   = graph.Color( 0x11,0x11,0x11 )
		gui.color.scroll.bar    = graph.Color( 0x88,0x88,0x88 )
	
	end
	
	if not gui.assign then
		gui.assign = {}
		gui.assign.left    = "left"
		gui.assign.center  = "center"
		gui.assign.right   = "right"
		gui.assign.top     = "top"
		gui.assign.bottom  = "bottom"
	end
end

function gui.free ()
	for k,v in pairs( gui.font )
	do
		font.close( v )
	end

end

function gui.getFont ( size )
	if gui.font then
		return gui.font[ size ]
	end
end

function gui.getLines ( obj )
	local lines = {}
	local s = obj.string
	local w
	
	if not s 
	then
		return lines
	end
	
	if obj.lineLim and obj.lineLim > 0
	then
		if string.len( s ) > obj.lineLim
		then
			s = string.sub( s , 1 , obj.lineLim ) .. ".."
		end
	end

	for w in string.gmatch( s  , "([^\n]*)[\n]?")
	do
		if w ~= ""
		then
			table.insert ( lines , w )
		end
	end
	return lines
end

-- draw routines

function gui.getColor ( obj , name )
	return obj.color[name] or gui.color[name]
end


function gui.draw.view ( win , bx , by , obj )
end

function gui.draw.image ( win , bx , by , obj )
	
	if obj.hide then
		return
	end
	
	local x = bx + obj.size.x
	local y = by + obj.size.y
	
	graph.Blit( obj.image , x , y  )
	
	if obj.border then
		gui.draw.box ( win , bx , by , obj )
	end
end

function gui.draw.text ( win , bx , by , obj )

	if obj.hide then
		return
	end

	local line

	local color = gui.getColor( obj , "text" )
	local lines = gui.getLines ( obj )	
	
	local x = bx + obj.size.x
	local y = by + obj.size.y

    -- if not updated any text stuff, then use cache
	if obj.cache.text == obj.string and obj.cache.textColor == color
	then
		for k,v in pairs(obj.cache.textsurf)
		do
			graph.Blit( v.text , v.x + x , v.y + y )
		end
		return
	end
	
	local textsurf = {}
	
    local tx = x
    local ty = y

	for line = 1,#lines
	do
		local text = graph.Text ( obj.font , lines[line] , color )
		
		local tw = graph.SurfWidth( text )
		local th = graph.SurfHeight( text )
		
		if obj.size.h == 0 and obj.size.h == 0 then
			obj.size.w = tw
			obj.size.h = th * #lines
		end
		
		local w = obj.size.w
		local h = obj.size.h
		
		local tmh = th * #lines


		if obj.assign.horizon == gui.assign.center 
		then
			tx = tx + (w/2) - (tw/2)
		elseif obj.assign.horizon == gui.assign.right
		then
			tx = tx + w - tw
		end
		
		if obj.assign.vertical == gui.assign.center
		then
			ty = ty + (h/2) - (tmh / 2) + (th * (line-1))
		elseif obj.assign.vertical == gui.assign.bottom
		then
			ty = ty + h - tmh + (th * (line-1))
		end
		
		graph.Blit( text , tx , ty )
		
		table.insert ( textsurf, { text = text , x = tx - x , y = ty - y } )
		
	end
	
	obj.cache.textsurf = textsurf
	obj.cache.text = obj.string
	obj.cache.textColor = color
	
end

function gui.draw.borderPrimitive ( x , y , w , h , c1 , c2 )
	graph.Line ( x , y , x + w , y , c1 )
	graph.Line ( x , y , x , y + h , c1 )
	graph.Line ( x + w , y , x + w , y + h , c2 )
	graph.Line ( x , y + h , x + w , y + h , c2 )
end

function gui.draw.border ( win , bx , by , obj )

	if obj.hide then
		return
	end

	local size = obj.size
	
	graph.Lock()
	
	local color_bright = gui.getColor ( obj , "bright" )
	local color_dark   = gui.getColor ( obj , "dark" )
	
	
	gui.draw.borderPrimitive ( bx + size.x , by + size.y , 
		size.w , size.h , color_bright , color_dark )

	graph.Unlock()
end

function gui.draw.box ( win , bx , by , obj )

	if obj.hide or not obj.drawBox then
		return
	end

	local size = obj.size
	
	local x = bx + size.x
	local y = by + size.y
	
	local fw = size.w
	local hs = (size.h/2)
	local hse = size.h-hs
	
	local color = gui.getColor ( obj , "back" )
	
	if win.focusView and obj.focused then
		color = gui.getColor ( obj , "focus" )
	end
	
	if obj.clicked  then
		color = gui.getColor ( obj , "clicked" )
	end
	
	if obj.disable then
		color = gui.getColor ( obj , "disable" )
	end
	
	if obj.cache.color ~= color or obj.cache.w ~= size.w or obj.cache.h ~= size.h 
	then
		graph.Lock()

		if type(color) == "table" then
			graph.BoxGrad( x , y , fw , hs , color[1] , color[2] )
			graph.BoxGrad( x , y + hs , fw , hse , color[2] , color[3] )
		else
			graph.BoxFill( x , y , fw , size.h , color )
		end
	
		graph.Unlock()
		gui.draw.border ( win , bx , by , obj )

		obj.cache.surf = graph.CreateSurf( size.w + 1, size.h + 1 ) 
		graph.CopySurf( graph.MainSurf() , obj.cache.surf , x , y , size.w + 1, size.h + 1 )
		
		obj.cache.color = color
		obj.cache.w = size.w
		obj.cache.h = size.h		
		
	else
		graph.Blit( obj.cache.surf , x , y )
	end
end


function gui.draw.scrollBar ( win , bx , by , obj )

	if obj.hide then
		return
	end

	local size = obj.size
	
	local x = bx + size.x
	local y = by + size.y
	
	local w = size.w
	local h = size.h

	local color = gui.getColor ( obj , "scroll" )
	
	local bar_h   = (obj.lines / (obj.max - obj.min)) * h 
	local bar_pos =  obj.pos   / (obj.max - obj.min)  * h
	
	graph.BoxFill( x , y , w , h , color.back )
	graph.BoxFill( x , y + bar_pos , w , bar_h , color.bar )

	gui.draw.border ( win , bx , by , obj )
	
	graph.Lock()

	gui.draw.borderPrimitive ( x + 1 , y + bar_pos + 1 , w - 2 , bar_h - 2 , color.bright , color.dark )
	
	graph.Unlock()
	
end

function gui.draw.button ( win , bx , by , obj )

	if obj.hide then
		return
	end
	
	gui.draw.box  ( win , bx , by , obj )
	gui.draw.text ( win , bx , by , obj )
	
end

function gui.createView ( x , y , w , h )
	local newobj = {}

	newobj.focused = false
	newobj.size = {}
	newobj.size.x = x
	newobj.size.y = y
	newobj.size.w = w
	newobj.size.h = h
	
	newobj.hide = false
	
	newobj.font = gui.font.normal
	newobj.draw = gui.draw.view
	newobj.color = {}
	newobj.cache = {}
	
	return newobj
end

function gui.createImage ( x , y , image )

	local iw = graph.SurfWidth( image )
	local ih = graph.SurfHeight( image )
	
	local newobj = gui.createView( x , y , iw , ih )

	newobj.image = image
	newobj.draw = gui.draw.image
	
	return newobj
end

function gui.createText ( x , y , string )
	
	local newobj = gui.createView( x , y , 0 , 0 )

	newobj.assign = { horizon = gui.assign.left , vertical = gui.assign.top }
	newobj.string = string
	newobj.draw = gui.draw.text
		
	return newobj
end

function gui.createTextBox ( x , y , w, h , string )
	
	local newobj = gui.createText ( x , y , string )
	
	newobj.size.w = w
	newobj.size.h = h
	newobj.drawBox = true

	return newobj
end

function gui.createScrollBar ( x , y , w , h )

	local newobj = gui.createView ( x , y , w , h )
	
	newobj.max = 10
	newobj.min = 0
	newobj.pos = 0
	newobj.lines = 5
	
	newobj.draw = gui.draw.scrollBar
	
	return newobj
end

function gui.createBox ( x , y , w , h )
	
	local newobj = gui.createView ( x , y , w , h )
	
	newobj.draw = gui.draw.box
	newobj.drawBox = true
	
	return newobj
end

function gui.createButton ( x , y , w , h , string )
	
	local newobj = gui.createTextBox( x , y , w , h , string )
	
	newobj.assign = { horizon = gui.assign.center , vertical = gui.assign.center }
	newobj.draw = gui.draw.button
	
	return newobj
end


function gui.createTextEdit ( x , y , w , h , string )
	
	local newobj = gui.createButton( x , y , w , h , string )
	
	newobj.color.back = gui.color.white
	
	return newobj
end


--  Focus
function gui.resetFocus ( win )
	if win.focusObj then
		win.focusObj.focused = false
	end
	win.focusObj = nil
end

function gui.getFocus ( win )
	return win.focusObj
end

function gui.setFocus ( win , obj )
	gui.resetFocus ( win )
	if obj
	then
		obj.focused = true
		win.focusObj = obj
	end
end

function gui.nextFocus ( win )
	if not win.focusObj or not win.focusObj.next
	then
		return
	end
	
	win.focusObj.next.focused = true
	win.focusObj.focused = false
	
	win.focusObj = win.focusObj.next
end

function gui.prevFocus ( win )
	if not win.focusObj or not win.focusObj.prev
	then
		return
	end

	win.focusObj.prev.focused = true
	win.focusObj.focused = false
	
	win.focusObj = win.focusObj.prev
end

function gui.connectObject ( prev , obj )
	if prev and obj then
		obj.prev  = prev
		prev.next = obj
	end
end

-- child

function gui.removeObjectFromTable ( objs , obj )
	local i
	for i = 1,#objs do
		if objs[i] == obj then
			table.remove( objs , i )
			gui.connectObject ( obj.prev , obj.next )
			return true
		end
		if objs[i].child then
			if gui.removeObjectFromTable ( objs[i].child , obj ) then
				return true
			end
		end
	end
end

function gui.removeObject ( win , obj )
	gui.removeObjectFromTable ( win.objects , obj )
end

function gui.append( win , newobj )
	table.insert( win.objects , newobj )
	
	if not newobj.child then
		if win.prevObj then
			gui.connectObject ( win.prevObj , newobj )
		end
		win.prevObj = newobj
	else
		if win.prevObj then
			gui.connectObject ( win.prevObj , newobj.child[1] )
		end
		win.prevObj = newobj.child[#newobj.child]
	end
end

function gui.appendChild( obj , newobj )
	if not obj.child 
	then
		obj.child = {}
	end
	
	table.insert( obj.child , newobj )
	
	if obj.prevObj then
		gui.connectObject( obj.prevObj , newobj )
	end
	obj.prevObj = newobj
end

function gui.drawObjects ( win , bx , by , objs )
	for k,v in pairs ( objs )
	do
		if v then
				v.draw( win , bx , by , v )
		else
			print("null found")
		end
		if v.child then
			gui.drawObjects( win , bx + v.size.x , by + v.size.y , v.child )
		end
	end
end

function gui.createWindow( x , y , w , h )
	local win = {}
	win.x = x
	win.y = y
	win.w = w
	win.h = h
	win.objects = {}
	win.objtable = {}
	win.focusView = false
	win.update = true
	
	return win
end

function gui.getFocusObj( win )
	return win.focusObj
end


function gui.getText( obj )
	return obj.string
end

function gui.setDrawer ( obj , draw )
	obj.draw = draw
end

function gui.setText( obj , text )
	obj.string = text
end

function gui.setImage ( obj , image )
	local iw = graph.SurfWidth( image )
	local ih = graph.SurfHeight( image )
	
	obj.size.w = iw
	obj.size.h = ih
	
	obj.image = image
	
end

function gui.setFont( obj , fontname )
	if gui.font and gui.font[fontname] then
		obj.font = gui.font[fontname]
	end
end

function gui.getColorMap( obj )
	return obj.color
end

function gui.setColorMap( obj , color )
	obj.color = color
end

function gui.setTextColor ( obj , color )
	obj.color.text = color
end

function gui.setColor ( obj , name , color )
	obj.color[name] = color
end

function gui.removeColor ( obj , name , color )
	obj.color[name] = nil
end



function gui.getId( obj )
	return obj.id
end

function gui.setId( obj , id )
	obj.id = id
end

function gui.setScrollBar( obj , max , min , lines )
	obj.max = max
	obj.min = min
	obj.lines = lines
end

function gui.setScrollPos( obj , pos )
	obj.pos = pos
end

function gui.setClick ( obj , flag )
	
	if flag then
		obj.clicked = true
	else
		obj.clicked = false
	end
end

function gui.resizeByText ( obj )

	local color = gui.getColor(obj , "text")
	local lines = gui.getLines ( obj )

	local line
	local w = 0
	local h = 0
	
	for line = 1,#lines
	do
		local text = graph.Text ( obj.font , lines[line] , color )
		
		local tw = graph.SurfWidth( text )
		local th = graph.SurfHeight( text )
				
		if w < tw then
			w = tw
		end

		h = h + th
		
		graph.FreeSurf ( text )
	end

	obj.size.w = w
	obj.size.h = h
	
end


function gui.setAssign ( obj , h , v )
	if not obj.assign then
		obj.assign = {}
	end
	obj.assign.horizon = h 
	obj.assign.vertical = v 
end

function gui.moveObject( obj , x , y )
	obj.size.x = x
	obj.size.y = y
end


function gui.getSize( obj )
	return obj.size.w , obj.size.h
end

function gui.setSize( obj , w , h )
	obj.size.w = w 
	obj.size.h = h
end


function gui.getPosition( obj )
	return obj.size.x , obj.size.y
end

function gui.setPosition( obj , x , y )
	obj.size.x = x
	obj.size.y = y
end


function gui.setDrawBox( obj , flag )
	obj.drawBox = flag
end


function gui.setLineLim( obj , lineLim )
	obj.lineLim = lineLim
end


function gui.setHide( obj , flag )
	obj.hide = flag
end

function gui.viewFocus ( win , flag )
	if flag then
		win.focusView = true
	else
		win.focusView = false
	end
end

function gui.setUpdate ( win )
	win.update = true
end


function gui.isUpdate ( win )
	return win.update
end

function gui.update( win )
	if win.update then
		gui.drawObjects ( win , win.x , win.y , win.objects )
		graph.Update()
		win.update = false
	end
end


