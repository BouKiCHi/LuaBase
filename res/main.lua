-- main.lua

local mod = do_module("mod_base.lmod")

-- routines

function mod.init()

	mod.w = mod.sw
	mod.h = mod.sh - 16
	
	mod.win   = gui.createWindow( 0 , 16 , mod.w , mod.h )
	local box = gui.createBox   ( 0 , 0  , mod.w , mod.h )
	
	gui.viewFocus ( mod.win , true )
	
	gui.append ( mod.win , box )	

	mod.call = nil
	
	icon_path = file_respath( "icon" ) .. system.PathDiv() 
	
	mod.readDir( icon_path );
	
	local icon_image = {}
	local icon_name = {}
	
	for i = 1,#mod.files do
		local fi = mod.files[i]
		
		if not fi.isdir then
			-- print(fi.name)
			local ext = string.match(fi.name,".*\.(png)")
			
			if ext == "png" or ext == "bmp" or ext == "jpg"
			then
				local icon = graph.ImgLoad( icon_path .. fi.name )
				if icon then
					table.insert( icon_image , icon )
					table.insert( icon_name , string.match(fi.name,"(.*)\.png"))
				end
			end				
		end
	end
		
	mod.icon = {}
	mod.icon.image = icon_image
	mod.icon.name  = icon_name
	
	mod.inum = 1	
	mod.iobjs = {}
	mod.tobjs = {}
	mod.tobjpos = {}

	mod.col = 4
	mod.row = 4
	
	
	local iw = 32
	local col = mod.col
	local xspace = mod.w - (iw * col)
	
	local ih = 32
	local row = mod.row
	local yspace = mod.h - (ih * row)

	local wsx = xspace / (col + 1)
	local wsy = yspace / (row + 1)
	
	local ix = 0
	local iy = wsy
	
	local inum = 1
	
	for j = 1,row do
		ix = wsx
		for i = 1,col do

			local img = gui.createImage ( ix , iy , icon_image[ inum ] )
			gui.appendChild( box , img )

			local text = gui.createButton ( ix , iy + ih , 0 , 0 , icon_name[ inum ] )
			gui.setDrawBox( text , false )
			gui.setLineLim( text , 10 )

			gui.setAssign ( text , gui.assign.center , gui.assign.top )
			gui.setFont ( text , "small" )
			gui.resizeByText ( text )

			local w,h = gui.getSize ( text )			
			gui.setPosition ( text , ix + (iw / 2) - (w/2) , iy + ih )			
			gui.appendChild( box , text )
			

			table.insert ( mod.tobjpos, { ix , iy } )
			
			ix = ix + iw + wsx
			inum = inum + 1
			
			table.insert ( mod.iobjs , img )
			table.insert ( mod.tobjs , text )
			
		end
		iy = iy + ih + wsy
	end	
	
	mod.iw = iw
	mod.ih = ih

end

function mod.redrawIcon()

	local inum = mod.inum
	
	for i = 1,#mod.iobjs
	do
		local icon = mod.iobjs[ i ]
		local text = mod.tobjs[ i ]

		gui.setImage ( icon , mod.icon.image[inum] )
		gui.setText  ( text , mod.icon.name[inum] )

		gui.resizeByText ( text )
		local x,y = unpack( mod.tobjpos[i] )
		local w,h = gui.getSize ( text )
		
		gui.setPosition ( text , x + (mod.iw / 2) - ( w / 2 ) , y + mod.ih )
		
		inum = inum + 1
	end
	
	gui.setUpdate( mod.win )
end

function mod.checkKey()
	if key.Up()
	then
		if mod.inum - mod.col >= 1 then
			mod.inum = mod.inum - mod.col
			mod.redrawIcon()
		end
	end
	if key.Down()
	then
		if mod.inum + #mod.iobjs < #mod.icon.image then
			mod.inum = mod.inum + mod.col
			mod.redrawIcon()
		end
	end
	if key.Select()
	then
		mod.doFinish()
	end
end


function mod.readDir( path )
	local list = {}
	local dir = system.OpenDir(path)
	local ent = system.ReadDir(dir)
	
	mod.files = {}
	
	while ent do
		local fi = {}
		local line
		fi.name  = system.EntName( ent )
		fi.size  = system.Filesize ( fi.name )
		fi.isdir = system.IsDir( fi.name )
		
		if fi.isdir then
			line = "<" .. fi.name .. ">"
		else
			line = fi.name
		end
				
		table.insert ( mod.files , fi )
		table.insert ( list , line )
		
		ent = system.ReadDir( dir )
	end
	system.CloseDir( dir )
	return list
end

return mod
