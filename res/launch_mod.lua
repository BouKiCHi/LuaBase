-- launch_mod.lua

local mod = do_module("mod_base.lmod")

local lview = "listview.lmod"

function mod.init()
	mod.doList()
end

function mod.doList()

	mod.list = {
		"test_gui.lmod",
		"clock.lmod",
		"iconlist.lmod",
		"diag.lmod" ,
		"filer.lmod",
		"keyboard.lmod",
		"custom.lmod"
	}

	mod.submod = do_module(lview)
	mod.submod.start()
	mod.submod.setTextBody( mod.list )
	mod.submod.setHeader ( " -- MODULE TEST -- " )
end

function mod.checkNotify()
	if mod.submod.isDone()
	then
		if mod.submod.moduleFile == lview
		then
			local result = mod.submod.getResult()
			if result < 0 then
				mod.doFinish()
			else
				mod.submod = do_module ( mod.list [ result + 1 ] )
				mod.submod.start()
			end
		else
			mod.doList()
		end
	end
end

return mod
