#
# Makefile for LuaBase
#

TARGET = luabase
TARGET_RELEASE := luabase


CC = gcc
RM = rm
CFLAGS =

DATE_CMD = `date "+%y%m%d"`
README = README
MAKEFILE = Makefile
DOCS_DIR = docs
LICENSE_DIR = license 
RELEASE_DIR = release
STARTUP = startup.lua
RESOURCE = res

EXE_PACK = $(TARGET)_$(DATE_CMD).zip
SRC_PACK = $(TARGET)_$(DATE_CMD)_src.zip

ifndef RELEASE
CFLAGS += -g
else
CFLAGS += -O3
STRIP_DO = 1
endif

ifdef A320
OBJDIR = a320_objs
DEVPFX = mipsel-linux-uclibc
CC = $(DEVPFX)-gcc
STRIP = $(DEVPFX)-strip
CFLAGS += -D__A320__
RELEASE_DIR = a320_release

TARGET := $(TARGET).a320
BINDIR = $(shell dirname `which $(CC)`)
SDLCONF = $(BINDIR)/sdl-config

else

OBJDIR = objs
SDLCONF = sdl-config
STRIP = strip

endif


SDL_CFLAGS = `$(SDLCONF) --cflags`
LIBS = `$(SDLCONF) --libs`
LIBS += -lSDL_ttf 
LIBS += -lSDL_image
LIBS += -lSDL_mixer

# CFLAGS += `lua-config --include`
# LIBS += `lua-config --libs`
# CFLAGS += -I/usr/include/lua5.1 
# LIBS   += -llua5.1

LIBS += -llua



# LIBS += `taglib-config --libs`
# LIBS +=  -ltag_c -lsqlite3
# CFLAGS += `taglib-config --cflags`

#
# 
#
OBJFILES = main.o mini_gui.o key.o system.o graph.o mixer.o

API_SRC = key.c system.c graph.c mixer.c
SRCFILES = $(API_SRC) debug_draw.c main.c mini_gui.c 
SRCFILES += graph.h key.h mini_gui.h mixer.h system.h autogen.py

ifdef DEBUG
CFLAGS += -g
endif

OBJS = $(addprefix $(OBJDIR)/,$(OBJFILES))


all : $(OBJDIR) $(TARGET)

release : all
	rm -rf $(RELEASE_DIR)
	mkdir  $(RELEASE_DIR)
ifneq ($(TARGET),$(TARGET_RELEASE))
	cp $(TARGET) $(RELEASE_DIR)/$(TARGET_RELEASE)
else
	cp $(TARGET) $(RELEASE_DIR)
endif
	cp -r $(LICENSE_DIR) $(RELEASE_DIR)
	cp $(README) $(RELEASE_DIR)

	cp $(STARTUP) $(RELEASE_DIR)
	cp -r $(RESOURCE) $(RELEASE_DIR)
	

$(OBJDIR) :
	mkdir $(OBJDIR)

$(TARGET) : $(OBJS)
	@echo "Linking ... $@"
	$(CC) -o $@ $(OBJS) $(LIBS)
ifdef STRIP_DO
	@echo "Stripping ... $@"
	$(STRIP) $@
endif

$(OBJDIR)/%.o : %.c
	$(CC) -c $(SDL_CFLAGS) $(CFLAGS) -o $@ $<
	
zipsrc : 
	zip $(SRC_PACK) $(README)
	zip $(SRC_PACK) $(MAKEFILE)
	zip $(SRC_PACK) $(STARTUP)
	zip $(SRC_PACK) $(SRCFILES)
	zip -r $(SRC_PACK) $(LICENSE_DIR)
	zip -r $(SRC_PACK) $(RESOURCE)
	
zip : release
	cd $(RELEASE_DIR) ; zip -r ../$(EXE_PACK) *

clean :
	$(RM) -f $(OBJS) $(TARGET)
	$(RM) -rf $(RELEASE_DIR)
	
docs :
	python docgen.py $(API_SRC)

ifndef A320
a320 :
	make A320=1 RELEASE=1
	
a320_zip :
	make A320=1 RELEASE=1 zip

a320_clean :
	make A320=1 clean
	
a320_release :
	make A320=1 RELEASE=1 release
endif
