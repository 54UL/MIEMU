# MINIEMU
Minimalist emulator implementations (IN-PROGRESS)

## Features
- Most minimal impementation (simple asf)
- Minimal dependency count
    - SDL_2
- Emulator UI
    - Shell
        - Emulation control (start, stop, reset)
        - File explorer 
- Portable code
    - Linux (ubuntu/debian/fedora)
- Emulation engine api
    - multi-emulator system
- Implementations
    - Chip 8
    - Game boy (IN PROGRESS)

Check the [road map](DOCS/emu_roadmap.md) for development notes uwu

### Setup

1) Install dependencies
```bash
sudo apt-get update
sudo apt-get install libsdl2-dev
```

2) Compile
```bash
mkdir build
cd build && cmake ..
cmake --build .
```
2) Create `ROMS` folder
4) Controls
    * Chip 8 keypad map()
        ```
        1234 
        QWER 
        ASDF
        ZXCV
        ```
    * Shell control
        ```
        O -> UP 
        L -> DOWN
        K -> BACK
        ; -> ACCEPT
        ```
3) Enjoy

## CC8 (C CHIP-8 EMULATOR) Overview 

# Initial state
![alt](images/initial-state.png)
# Running state
![alt](images/running-state.png)

- Check [Chip 8 gallery](images/chip8-gallery.md)
## Resources

Chip 8 test suite
* https://github.com/Timendus/chip8-test-suite

Chipo 8 roms
* https://github.com/kripod/chip8-roms

## Game boy WIP ()
- CPU Almost fully implemented
- Test rendering without LCD Controller and PPU
- Emulation API + app implemented

![alt](images/gb-app-wip-1.png)


## Authors
* TODO!!!
