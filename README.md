# Wandering Melody

A collaborative musical game for 1-4 players.

## Building

Current version is built with Cocos2d-x 3.17.1.

Before building, copy the entire Cocos2d-x folder into the project and rename
it as `cocos2d`. Or perhaps create a symlink, whichever is more convenient.

### Linux/macOS

```sh
cmake . -DCMAKE_BUILD_TYPE=Release
make
```

### Windows

```sh
cmake . -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -A Win32
# Change the version number accordingly if using a different version of VS
```

Open the generated file `WanderingMelody.sln` and build in Visual Studio.

## Controls

Song selection:
* _Arrows_: navigation
* _Enter_: select

Pre-game:
* _Tab_: mod selection
  - _1234_: toggle autoplay
  - _Q_: speed mode
* _Enter_: start
* _Esc_: back

Key mappings:
* P1: _ZXCV_
* P2: _KL;'_
* P3: _QWER_
* P4: _7890_
