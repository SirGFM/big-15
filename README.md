# JJAT (production/repository name: big-15)

JJAT is a precision platformer with a few inspiration from metroidvania games (except it turned out really short...). You play as two characters exploring a facility, looking for a way to defeat a monster.

It was originally intented as a co-op game, but it was played during most part of its development as a single player game.

The game was made as a follow up to my GGJ#15 entry, taking the idea of controlling to character at the same time and the ability to teleport from one player to the other. It was used to check what should be added to its framework ([GFraMe](https://github.com/SirGFM/GFraMe)).

Also, it was developed for the [BIG festival](http://www.bigfestival.com.br/lang/en?r=/).

# Build instructions

Be sure to have SDL2 installed. You can get it from your package manager. In Debian/Ubuntu:

```
$ sudo apt-get install libsdl2-dev
```

After cloning it, you'll have to clone the framework, that is set as a submodule. To do so, run the following command on the cloned directory:

```
$ git submodule update --init
```

Afterward, simply compile it:

```
$ make
```

Or, for the release version:

```
$ make RELEASE=yes
```

The main difference between both version are some debug features (like, being able to move to the mouse cursor) and the assets folder location. The release version requires the assets folder to be on the same directory as the binary, while the debug version requires it to be on the binary's parent folder.

So, if you compile the release version, run:

```
$ mv ./bin/game .
```
from the cloned directory.

After building the game, it'll be missing the songs. You must render every song before being able to run the game. To do so, go into big-15/assets/vmml and compile every song using a mml player/renderer. The one used was [VMML by Benjamin Soule] (http://benjaminsoule.fr/tools/vmml/).

The relation between .mml and .wav are as follow:

|MML file           | WAVE file       |
|-------------------|-----------------|
|boss-battle.mml    |bossBattle.wav   |
|intro.mml          |intro.wav        |
|menu.mml           |menu.wav         |
|movin-on.mml       |movinOn.wav      |
|tension-goes-up.mml|TensionGoesUp.wav|
|victory.mml        |victory.wav      |
