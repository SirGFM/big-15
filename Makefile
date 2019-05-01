#=========================================================================
# This Makefile have the following targets:
#   - linux32
#   - linux64
#   - win32
#   - win64
#   - *_debug

#=========================================================================
# Set the target and the lib's version
TARGET := game

#=========================================================================
# Run make using bash, so syntax for conditionals works as expected
SHELL := /bin/bash

#=========================================================================
# Set the default CC (may be overriden into something like mingw)
CC ?= gcc

# To find every .c file in src:
#   FILES := $(call rwildcard, src/, *.c)
# To find all the .c and .h files in src:
#   FILES := $(call rwildcard, src/, *.c *.h)
rwildcard=$(foreach d,$(wildcard $1*), \
    $(call rwildcard,$d/,$2) \
    $(filter $(subst *,%,$2),$d))

#=========================================================================
# Parse the configuration from the target goal
ifneq (, $(findstring _debug, $(MAKECMDGOALS)))
    MODE := debug
else
    MODE := release
endif
ifneq (, $(findstring linux, $(MAKECMDGOALS)))
    OS := linux
    STRIP := strip
else ifneq (, $(findstring win, $(MAKECMDGOALS)))
    ifdef $(OS)
        ifeq ($(OS), linux)
            $(error More than a single OS target was specified)
        endif
    endif
    OS := win
    EXT := .exe
    # ICON is lazily resolved
    ICON = $(WINICON)
else ifneq (, $(findstring web, $(MAKECMDGOALS)))
    CC := emcc
    OS := web
    ARCH := 32
    # Since the game's .bc is the main intermediate before a .js, use that
    #EXT := .bc

    GFRAME_SRC := $(call rwildcard, lib/GFraMe/src/, *.c)

    WEB_RES := $(call rwildcard, $(ASSET_PREFIX)/assets/, *)
    WEB_RES := $(foreach res,$(WEB_RES),--preload-file $(res)@$(res:$(ASSET_PREFIX)/%=%))
endif
ifneq (, $(findstring 32, $(MAKECMDGOALS)))
    ARCH := 32
else ifneq (, $(findstring 64, $(MAKECMDGOALS)))
    ARCH := 64
endif

#=========================================================================
# Setup path to the lib
TGTDIR := $(OS)$(ARCH)_$(MODE)
LIBDIR := ./lib/GFraMe/bin/$(OS)$(ARCH)_$(MODE)

#=========================================================================
# Setup CFLAGS and LDFLAGS (NOTE: the submodule lib is placed early in the
# search path to force its use)
myCFLAGS := -I"./lib/GFraMe/include/" $(CFLAGS) -Wall
ifeq ($(ARCH), 64)
    myCFLAGS := $(myCFLAGS) -m64
else
    myCFLAGS := $(myCFLAGS) -m32
endif

ifeq ($(MODE), debug)
    myCFLAGS := $(myCFLAGS) -O0 -g -DDEBUG
else ifeq ($(OS), web)
    myCFLAGS := $(myCFLAGS) -O2
else
    myCFLAGS := $(myCFLAGS) -O1
endif

ifeq ($(OS), web)
    myCFLAGS := $(myCFLAGS) -DEMCC -s USE_SDL=2 -s WASM=1
else
    myLDFLAGS := -L$(LIBDIR) $(LDFLAGS)
    myLDFLAGS := $(myLDFLAGS) -Wl,-Bstatic -lGFraMe -Wl,-Bdynamic -lm
    ifeq ($(OS), win)
        myLDFLAGS := $(myLDFLAGS) -mwindows -lmingw32
    else
        myCFLAGS := $(myCFLAGS) -fPIC
    endif
    myLDFLAGS := $(myLDFLAGS)  -lSDL2main -lSDL2
endif

#=========================================================================
# Paths and objects
VPATH := src
OBJDIR := obj/$(TGTDIR)
BINDIR := bin/$(TGTDIR)

OBJS := $(OBJDIR)/audio.o $(OBJDIR)/bullet.o $(OBJDIR)/camera.o \
    $(OBJDIR)/collision.o $(OBJDIR)/commonEvent.o $(OBJDIR)/controller.o \
    $(OBJDIR)/credits.o $(OBJDIR)/demo.o $(OBJDIR)/event.o \
    $(OBJDIR)/global.o $(OBJDIR)/globalVar.o $(OBJDIR)/main.o $(OBJDIR)/map.o \
    $(OBJDIR)/menustate.o $(OBJDIR)/mob.o $(OBJDIR)/object.o \
    $(OBJDIR)/options.o $(OBJDIR)/parser.o $(OBJDIR)/player.o \
    $(OBJDIR)/playstate.o $(OBJDIR)/registry.o $(OBJDIR)/signal.o \
    $(OBJDIR)/textwindow.o $(OBJDIR)/timer.o $(OBJDIR)/transition.o \
    $(OBJDIR)/types.o $(OBJDIR)/ui.o $(OBJDIR)/quadtree/qthitbox.o \
    $(OBJDIR)/quadtree/qtnode.o $(OBJDIR)/quadtree/qtstatic.o \
    $(OBJDIR)/quadtree/quadtree.o $(OBJDIR)/state.o $(OBJDIR)/errorstate.o \
    $(OBJDIR)/save.o

WINICON := obj/$(TGTDIR)/assets_icon.o

#=========================================================================
# Helper build targets
.PHONY: help linux32 linux64 linux32_debug linux64_debug win32 win64 \
    win32_debug win64_debug web package_web clean reallyclean LIB

help:
	@ echo "Build targets:"
	@ echo "  linux32"
	@ echo "  linux64"
	@ echo "  linux32_debug"
	@ echo "  linux64_debug"
	@ echo "  win32"
	@ echo "  win64"
	@ echo "  win32_debug"
	@ echo "  win64_debug"
	@ echo "  web"
	@ echo "  package_web"
	@ echo "  clean"

linux32: bin/linux32_release/$(TARGET)
linux32_debug: bin/linux32_debug/$(TARGET)
linux64: bin/linux64_release/$(TARGET)
linux64_debug: bin/linux64_debug/$(TARGET)
win32: bin/win32_release/$(TARGET).exe
win32_debug: bin/win32_debug/$(TARGET).exe
win64: bin/win64_release/$(TARGET).exe
win64_debug: bin/win64_debug/$(TARGET).exe
web: bin/web32_release/$(TARGET).html

#=========================================================================
# Build targets
bin/$(TGTDIR)/$(TARGET)$(EXT): $(OBJS) $(ICON) | bin/$(TGTDIR)/$(TARGET).mkdir
	@ echo "[ CC] $@"
	@ $(CC) $(myCFLAGS) -o $@ $^ $(myLDFLAGS)
	@ if [ "$(MODE)" == "release" ]; then echo "[STP] $@"; fi
	@ if [ "$(MODE)" == "release" ]; then $(STRIP) $@; fi

obj/$(TGTDIR)/%.o: %.c | obj/$(TGTDIR)/%.mkdir
	@ echo "[ CC] $< -> $@"
	@ $(CC) $(myCFLAGS) -o $@ -c $<

obj/$(TGTDIR)/assets_%.o: assets/icons/%.rc | obj/$(TGTDIR)/assets_%.mkdir
	@ echo "[ICN] $@"
	@ $(WINDRES) $< $@

#=========================================================================
# Helper build targets (for dependencies and directories)
bin/$(TGTDIR)/$(TARGET)$(EXT): | LIB

LIB:
	@ echo "[LIB] Building dependencies..."
	@ make $(MAKECMDGOALS) --directory=./lib/GFraMe/

clean:
	@ echo "[ RM] ./*"
	@ rm -rf obj/
	@ rm -rf bin/

reallyclean:
	@ echo "[ RM] ./* ./lib/*"
	@ rm -rf obj/
	@ rm -rf bin/
	@ make clean --directory=./lib/GFraMe/

#=========================================================================
# Automatically generate directories
%.mkdir:
	@ mkdir -p $(@D)
	@ touch $@

#=========================================================================
# web target
package_web: bin/web32_release/$(TARGET).html misc/index.html
	@ echo "[WEB] Packaging..."
	@ mkdir -p jjat_web
	@ cp misc/index.html jjat_web/
	@ cp bin/$(TGTDIR)/$(TARGET).data jjat_web/
	@ cp bin/$(TGTDIR)/$(TARGET).js jjat_web/
	@ cp bin/$(TGTDIR)/$(TARGET).wasm jjat_web/

bin/$(TGTDIR)/$(TARGET).html: bin/$(TGTDIR)/$(TARGET).bc bin/$(TGTDIR)/libGFraMe.bc | $(ASSET_PREFIX)/assets/ bin/$(TGTDIR)/$(TARGET).mkdir
	@ echo "[EMC] $@"
	@ $(CC) -s TOTAL_MEMORY=134217728 $(myCFLAGS) -o $@ $^ $(WEB_RES)

$(ASSET_PREFIX)/assets/:
	@ echo 'Could not find the pre-compiled assets!'
	@ echo 'Be sure to set $$ASSET_PREFIX to point to a valid directory'
	@ false

bin/$(TGTDIR)/$(TARGET).bc: $(OBJS) | bin/$(TGTDIR)/$(TARGET).mkdir
	@ echo "[ CC] $@"
	@ $(CC) $(myCFLAGS) $^ -o $@

bin/$(TGTDIR)/libGFraMe.bc: | bin/$(TGTDIR)/libGFraMe.mkdir
	@ echo "[ CC] $@"
	$(CC) $(myCFLAGS) -o $@ $(GFRAME_SRC)
