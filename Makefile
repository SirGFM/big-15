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
# Set the default CC (may be overriden into something like mingw)
CC ?= gcc

#=========================================================================
# Parse the configuration from the target goal
ifneq (, $(findstring _debug, $(MAKECMDGOALS)))
    MODE := debug
    STRIP := touch
else
    MODE := release
endif
ifneq (, $(findstring linux, $(MAKECMDGOALS)))
    OS := linux
    ifndef $(DEBUG)
        STRIP := strip
    endif
endif
ifneq (, $(findstring win, $(MAKECMDGOALS)))
    ifdef $(OS)
        ifeq ($(OS), linux)
            $(error More than a single OS target was specified)
        endif
    endif
    OS := win
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
# Setup CFLAGS and LDFLAGS
CFLAGS := $(CFLAGS) -Wall -I"./lib/GFraMe/include/"
ifeq ($(ARCH), 64)
    CFLAGS := $(CFLAGS) -m64
else
    CFLAGS := $(CFLAGS) -m32
endif

ifeq ($(MODE), debug)
    CFLAGS := $(CFLAGS) -O0 -g -DDEBUG
else
    CFLAGS := $(CFLAGS) -O1
endif

LDFLAGS := $(LDFLAGS) -L$(LIBDIR)
LDFLAGS := $(LDFLAGS) -Wl,-Bstatic -lGFraMe -Wl,-Bdynamic -lm
ifeq ($(OS), win)
    LDFLAGS := $(LDFLAGS) -mwindows -lmingw32
else
    CFLAGS := $(CFLAGS) -fPIC
endif
LDFLAGS := $(LDFLAGS)  -lSDL2main -lSDL2

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
    $(OBJDIR)/quadtree/quadtree.o

WINICON := assets/icons/icon.o

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
bin/$(TGTDIR)/$(TARGET): $(OBJS) $(ICON)
	@ echo "[ CC] $@"
	@ $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@ echo "[STP] $@"
	@ $(STRIP) $@

obj/$(TGTDIR)/%.o: %.c
	@ echo "[ CC] $< -> $@"
	@ $(CC) $(CFLAGS) -o $@ -c $<

%.o: %.rc
	@ echo "[ICN] $@"
	@ $(WINDRES) $< $@

#=========================================================================
# Helper build targets (for dependencies and directories)
bin/$(TGTDIR)/$(TARGET): | LIB

LIB:
	@ echo "[LIB] Building dependencies..."
	@ make $(MAKECMDGOALS) --directory=./lib/GFraMe/

$(OBJS): | obj/$(TGTDIR) obj/$(TGTDIR)/quadtree bin/$(TGTDIR)

obj/$(TGTDIR):
	@ echo "[MKD] $@"
	@ mkdir -p obj/$(TGTDIR)

obj/$(TGTDIR)/quadtree:
	@ echo "[MKD] $@"
	@ mkdir -p obj/$(TGTDIR)/quadtree

bin/$(TGTDIR):
	@ echo "[MKD] $@"
	@ mkdir -p bin/$(TGTDIR)

clean:
	@ echo "[ RM] ./*"
	@ rm -rf obj/
	@ rm -rf bin/

reallyclean:
	@ echo "[ RM] ./* ./lib/*"
	@ rm -rf obj/
	@ rm -rf bin/
	@ make clean --directory=./lib/GFraMe/
