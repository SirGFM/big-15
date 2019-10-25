#=========================================================================
# This Makefile have the following targets:
#   - linux32
#   - linux64
#   - win32
#   - win64
#   - *_debug
#   - web

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
# Set specific CFLAGS, LDFLAGS and LDLIBS based on the target
WEB := false

myCFLAGS := -I ./lib/GFraMe/include/ $(CFLAGS) -Wall
myLDFLAGS := -L ./lib/GFraMe/bin/$(MAKECMDGOALS) $(LDFLAGS)
myLDLIBS := $(myLDLIBS) -Wl,-Bstatic -lGFraMe -Wl,-Bdynamic -lm

ifneq (, $(findstring win, $(MAKECMDGOALS)))
    # ICON is generated from a resource file and a .ico
    ICON := obj/$(MAKECMDGOALS)/assets_icon.o
    myLDLIBS := $(myLDLIBS) -mwindows -lmingw32
else ifneq (, $(findstring linux, $(MAKECMDGOALS)))
    myCFLAGS := $(myCFLAGS) -fPIC
    STRIP := strip
else ifneq (, $(findstring web, $(MAKECMDGOALS)))
    # The web build is slightly different...
    #   - It only compiles in release mode
    #   - LDLIBS and LDFLAGS are ignored
    #   - The game library is built from this Makefile
    # So this pretty much overrides everything
    WEB := true

    # Ignore anything set on the command line and use emcc
    override CC := emcc
    myCFLAGS := $(myCFLAGS) -DEMCC -s USE_SDL=2 -s WASM=1

    # List for every source in the game library
    GFRAME_SRC := $(call rwildcard, lib/GFraMe/src/, *.c)

    # List every resource (to be embedded) preceded by a --preload-file
    WEB_RES := $(call rwildcard, $(ASSET_PREFIX)/assets/, *)
    WEB_RES := $(foreach res,$(WEB_RES),--preload-file $(res)@$(res:$(ASSET_PREFIX)/%=%))
endif

myLDLIBS := $(myLDLIBS)  -lSDL2main -lSDL2 $(LDLIBS)

# Since CFLAGS are modified here once again (to add optimization), some flags
# are overriden for the web build (since that's somewhat special)
ifeq ($(WEB), true)
    myCFLAGS := $(myCFLAGS) -O2
    undefine myLDLIBS
    undefine myLDFLAGS
else ifneq (, $(findstring _debug, $(MAKECMDGOALS)))
    myCFLAGS := $(myCFLAGS) -O0 -g -DDEBUG
else
    myCFLAGS := $(myCFLAGS) -O1
endif

ifneq (, $(findstring 64, $(MAKECMDGOALS)))
    myCFLAGS := $(myCFLAGS) -m64
else
    myCFLAGS := $(myCFLAGS) -m32
endif

#=========================================================================
# Paths and objects
SRC := $(call rwildcard, src/, *.c)
OBJS := $(SRC:src/%.c=obj/$(MAKECMDGOALS)/%.o)

#=========================================================================
# Helper build targets
.PHONY: help linux32_release linux64_release linux32_debug linux64_debug \
    win32_release win64_release win32_debug win64_debug web package_web \
    clean reallyclean LIB

help:
	@ echo "Build targets:"
	@ echo "  linux32_release"
	@ echo "  linux64_release"
	@ echo "  linux32_debug"
	@ echo "  linux64_debug"
	@ echo "  win32_release"
	@ echo "  win64_release"
	@ echo "  win32_debug"
	@ echo "  win64_debug"
	@ echo "  web"
	@ echo "  package_web"
	@ echo "  clean"
	@ echo "  reallyclean"

linux32_release: bin/linux32_release/game.bin
linux32_debug: bin/linux32_debug/game.bin
linux64_release: bin/linux64_release/game.bin
linux64_debug: bin/linux64_debug/game.bin
win32_release: bin/win32_release/game.exe
win32_debug: bin/win32_debug/game.exe
win64_release: bin/win64_release/game.exe
win64_debug: bin/win64_debug/game.exe
web: bin/web/game.html

#=========================================================================
# Build targets
%.exe: $(OBJS) $(ICON) | %.mkdir LIB
	@ echo "[ CC] $@"
	@ $(CC) $(myCFLAGS) -o $@ $^ $(myLDFLAGS) $(myLDLIBS)

%.bin: $(OBJS) $(ICON) | %.mkdir LIB
	@ echo "[ CC] $@"
	@ $(CC) $(myCFLAGS) -o $@ $^ $(myLDFLAGS) $(myLDLIBS)
	@ if [ "$(MODE)" == "release" ]; then echo "[STP] $@"; fi
	@ if [ "$(MODE)" == "release" ]; then strip $@; fi

obj/$(MAKECMDGOALS)/%.o: src/%.c | obj/$(MAKECMDGOALS)/%.mkdir
	@ echo "[ CC] $< -> $@"
	$(CC) $(myCFLAGS) -o $@ -c $<

obj/$(MAKECMDGOALS)/assets_icon.o: assets/icons/icon.rc | obj/$(MAKECMDGOALS)/assets_icon.mkdir
	@ echo "[ICN] $@"
	@ $(WINDRES) $< $@

LIB:
	@ echo "[LIB] Building dependencies..."
	@ make $(subst _release,,$(MAKECMDGOALS)) --directory=./lib/GFraMe/

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
package_web: bin/web/game.html misc/index.html
	@ echo "[WEB] Packaging..."
	@ mkdir -p jjat_web
	@ cp misc/index.html jjat_web/
	@ cp bin/web/game.data jjat_web/
	@ cp bin/web/game.js jjat_web/
	@ cp bin/web/game.wasm jjat_web/

bin/web/game.html: bin/web/game.bc bin/web/libGFraMe.bc | $(ASSET_PREFIX)/assets/ bin/web/game.mkdir
	@ echo "[EMC] $@"
	$(CC) -s TOTAL_MEMORY=134217728 $(myCFLAGS) -o $@ $^ $(WEB_RES)

$(ASSET_PREFIX)/assets/:
	@ echo 'Could not find the pre-compiled assets!'
	@ echo 'Be sure to set $$ASSET_PREFIX to point to a valid directory'
	@ false

bin/web/game.bc: $(OBJS) | bin/web/game.mkdir
	@ echo "[ CC] $@"
	$(CC) $(myCFLAGS) $^ -o $@

bin/web/libGFraMe.bc: | bin/web/libGFraMe.mkdir
	@ echo "[ CC] $@"
	$(CC) $(myCFLAGS) -o $@ $(GFRAME_SRC)
