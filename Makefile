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
endif
ifneq (, $(findstring win, $(MAKECMDGOALS)))
    ifdef $(OS)
        ifeq ($(OS), linux)
            $(error More than a single OS target was specified)
        endif
    endif
    OS := win
    EXT := .exe
    # ICON is lazily resolved
    ICON = $(WINICON)
endif
ifneq (, $(findstring 32, $(MAKECMDGOALS)))
    ARCH := 32
endif
ifneq (, $(findstring 64, $(MAKECMDGOALS)))
    ifdef $(ARCH)
        ifneq ($(ARCH), 32)
            $(error More than a single target architecture was specified)
        endif
    endif
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
else
    myCFLAGS := $(myCFLAGS) -O1
endif

myLDFLAGS := -L$(LIBDIR) $(LDFLAGS)
myLDFLAGS := $(myLDFLAGS) -Wl,-Bstatic -lGFraMe -Wl,-Bdynamic -lm
ifeq ($(OS), win)
    myLDFLAGS := $(myLDFLAGS) -mwindows -lmingw32
else
    myCFLAGS := $(myCFLAGS) -fPIC
endif
myLDFLAGS := $(myLDFLAGS)  -lSDL2main -lSDL2

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
    win32_debug win64_debug clean reallyclean LIB

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
	@ echo "  clean"

linux32: bin/linux32_release/$(TARGET)
linux32_debug: bin/linux32_debug/$(TARGET)
linux64: bin/linux64_release/$(TARGET)
linux64_debug: bin/linux64_debug/$(TARGET)
win32: bin/win32_release/$(TARGET).exe
win32_debug: bin/win32_debug/$(TARGET).exe
win64: bin/win64_release/$(TARGET).exe
win64_debug: bin/win64_debug/$(TARGET).exe

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
