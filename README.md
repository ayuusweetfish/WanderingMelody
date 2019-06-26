# Wandering Melody

A collaborative musical game for 1-4 players.

## Building

Current version is built with Cocos2d-x 3.17.1.

Before building, copy the entire Cocos2d-x folder into the project and rename
it as `cocos2d`. Or perhaps create a symlink, whichever is more convenient.

You may need to re-run CMake after each change to `Resources/`.

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
* **Arrows**: navigation
* **Enter**: select

Pre-game:
* **Tab**: options panel
  - **1234**: toggle autoplay
  - **Q**: mode selection
* **Enter**: start
* **Esc**: back

Key mappings:
* P1: **ZXCV**
* P2: **KL;'**
* P3: **QWER**
* P4: **7890**
