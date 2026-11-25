# minishcap-recomp

*The Legend of Zelda: The Minish Cap* (GBA, USA) running natively on your PC, built from **your own ROM** with the
[`gba-recomp`](../gba-recomp) engine. This repository is the game: its profile, its launcher look and its scripts. It
contains no ROM and nothing derived from one. [Versión en español](README.es.md).

**Status: early.** The game boots, plays through the intro, the Minish Festival and the castle ceremony, saves to EEPROM and has sound and save
states; it has not been played to the end. See `plan.md`.

## Playing (Windows)

1. Unzip the package anywhere.
2. Double-click `gba-port.exe`, choose your `minishcap_usa.gba` once and press **Play**. The first time the game is
   prepared on your machine (a few seconds); after that it starts at once. Only the path of the ROM is remembered.
3. Saves go to `saves\` next to the program.

Dropping the ROM on `gba-port.exe` works too, and from a terminal: `gba-port.exe <rom.gba> [--no-launch]`.
The ROM must be the USA release (SHA-1 `b4bd50e4131b027c334547b4524e2dbbd4227130`).

Keys: arrows, `X` = A, `Z` = B, `A` = L, `S` = R, `Enter` = Start, `Backspace` = Select, `Tab` (held) = fast forward,
`F11` = full screen, `F5` / `F8` = save / load state (`1`-`9` pick the slot), `F12` = screenshot, `F1` / `Esc` = menu (language, video, sound, key and pad rebinding, save-state slots, rewind and the game's extras: fast text, fast transitions), `R` (held) = rewind.
Game controllers work too.

## Building the package

`powershell -File scripts\release.ps1` (needs the engine built: `cargo build --release` in `..\gba-recomp`). The
launcher icon is drawn by `scripts\make_icon.py` (original art, nothing from the game).

## Linux

Download `minishcap-recomp-<version>-linux-x86_64.tar.gz`, extract it, and run:

```
./gba-port /path/to/your/rom.gba
```

The first run translates and compiles the game from your ROM (a couple of seconds) and caches it in `~/.cache/gba-port`; saves go to `saves/` next to `games/`. You need the C library development files to link it (`build-essential` or `libc6-dev`). There is no ROM-picker window on Linux yet. The ROM is never included in the package.

See [docs/DESARROLLO.md](docs/DESARROLLO.md) (Spanish) for building from source, testing and packaging, and [CONTRIBUTING.md](CONTRIBUTING.md).
