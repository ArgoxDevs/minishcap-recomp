# Desarrollo

Volver al [README](../README.md). Aquí está lo que necesita quien quiere compilar, probar o empaquetar el proyecto; para jugar basta con las releases (todavía no hay ninguna publicada).

## Qué contiene

- `games/minishcap_usa.toml`: el perfil. Identifica la ROM (CRC32 `ABCEBBB1`), da los bloques que el arranque copia a RAM (`[[overlay]]`), el fichero de raíces dinámicas (`[roots]`), el punto donde se toman los estados guardados (`[resume]`) y el aspecto del lanzador (`[launcher]`). Solo lleva direcciones, nombres y tamaños; ningún byte del juego.
- `games/dynamic_roots.txt`: direcciones de funciones a las que solo se llega por un salto indirecto (punteros en RAM, tablas indexadas). Salen de jugar con `tools/complete.sh` del motor: la ejecución pide cada una que falta.
- `games/minishcap_usa/options.c`: opciones del juego (menú F1, categoría Extras): texto rápido y transiciones rápidas. Se compila junto al C generado con `-DRT_GAME -DRT_GAME_OPTS`.
- `games/minishcap_usa/text.c`: mecanismo de traducción. Reconstruye el banco de mensajes en memoria con `lang/es.txt` (el texto en español está escrito en `lang/es.txt`: 2707 de los 3699 mensajes; el resto son nombres propios, texto de depuración y mensajes vacíos que se dejan como están); `--language dump` vuelca el inglés de la ROM del usuario a `lang/en.dump.txt` (ignorado por git). Ver [texto.md](texto.md).
- `games/minishcap_usa/icon.ico` y `emblem.bmp`: arte propio (`scripts/make_icon.py`).
- `scripts/check.ps1` y `scripts/check.sh`: comprobaciones. `scripts/release.ps1` y `scripts/release.sh`: paquetes de Windows y de Linux.
- `.github/workflows/`: `ci.yml` (lo que no necesita la ROM) y `linux-package.yml` (construye el paquete de Linux en Ubuntu 22.04; manual, no publica).

La ROM va en `roms/minishcap_usa.gba` (USA, SHA-1 `b4bd50e4131b027c334547b4524e2dbbd4227130`); `roms/`, `out/` y `saves/` están en `.gitignore`.

## Construir y ejecutar desde el código

Hace falta el motor, en la carpeta hermana `../gba-recomp`, y Rust.

```
git clone https://github.com/ArgoxDevs/gba-recomp ../gba-recomp
(cd ../gba-recomp && cargo build --release)
../gba-recomp/target/release/gba-port roms/minishcap_usa.gba --games games
../gba-recomp/target/release/gba-port roms/minishcap_usa.gba --games games --no-launch   # solo construye
```

`gba-port` traduce la ROM a C con `gba-recomp gen`, lo compila con TinyCC junto al runtime y SDL2, y lo deja en una caché por juego, CRC32 y versión (`%LOCALAPPDATA%\gba-port` o `~/.cache/gba-port`; `--cache DIR` la cambia). Si un cambio no se nota, borra esa carpeta.

### Windows

Hace falta TinyCC y SDL2 (con `include\` y `lib\x64\SDL2.dll`): `gba-port` los busca en `toolchain\tcc\` y `sdl2\` del paquete o de una carpeta hermana (por ejemplo `..\alttp-recomp`), o en `GBA_PORT_CC` y `GBA_PORT_SDL2`.

### Linux

```
sudo apt install build-essential pkg-config libsdl2-dev python3   # compilador de C (para enlazar), SDL2 y Python (scan.py)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh     # Rust, si no lo tienes
git clone https://github.com/TinyCC/tinycc ../tinycc && git -C ../tinycc checkout e23dabed
(cd ../tinycc && ./configure --prefix=$HOME/tcc && make && make install)
GBA_PORT_CC=$HOME/tcc/bin/tcc ../gba-recomp/target/release/gba-port roms/minishcap_usa.gba --games games
```

TinyCC enlaza con la libc del sistema (`crt1.o`, `libc.so`), de ahí `build-essential`. Con las cabeceras de `libsdl2-dev` en `/usr/include/SDL2` no hace falta `GBA_PORT_SDL2`.

## Probar

```
powershell -File scripts\check.ps1 [-NoBuild]     # Windows
scripts/check.sh [--no-build]                      # Linux
```

1. Nada de la ROM ni derivados en git ni en su historial (`tools/scan.py` del motor).
2. El perfil es TOML válido y el C propio compila contra el runtime (`check.sh`; en Windows lo hace la construcción real).
3. Las pruebas del motor (`cargo test`).
4. Pruebas C del runtime: BIOS y estados truncados, sin ROM. Windows busca TinyCC/GCC (`-Cc` permite elegirlo); Linux usa GCC. Se omiten si no hay compilador. Los archivos de prueba quedan en `out/runtime-check/`.
5. Construcción real desde `roms/minishcap_usa.gba` en una caché nueva (se omite sin ROM).

Para jugar a mano y hallar raíces dinámicas o fallos de la PPU, el motor trae `tools/build_play.sh` y `tools/complete.sh`; añade las direcciones nuevas a `games/dynamic_roots.txt`.

### Hook de `pre-push`

El `install_hooks.sh` del motor busca `tools/scan.py` dentro del repositorio, y aquí no existe. Crea `.git/hooks/pre-push` con este contenido y hazlo ejecutable (`chmod +x`); usa el `scan.py` del motor hermano:

```
#!/bin/sh
exec python -I "$(git rev-parse --show-toplevel)/../gba-recomp/tools/scan.py" --history
```

Los hooks no se versionan: hay que instalarlo en cada clon.

## Empaquetar

- **Windows:** `powershell -File scripts\release.ps1 [-Version 0.1.0]` → `out\release\minishcap-recomp-<v>-win64.zip` y `SHA256SUMS`. Compila el motor con el icono del juego y mete `gba-port.exe`, `gba-recomp.exe`, el runtime, SDL2, TinyCC y `games\`.
- **Linux:** `scripts/release.sh [0.1.0]` → `out/release/minishcap-recomp-<v>-linux-x86_64.tar.gz`. Para que abra en más distribuciones, constrúyelo en Ubuntu 22.04: lo hace el workflow `linux-package.yml` (`gh workflow run linux-package.yml -f version=0.1.0`), que deja el paquete como artefacto de la ejecución.
- **Android:** la app vive en el motor (`../gba-recomp/android/`, ver su README); usa el perfil de este repositorio (`gbaPort.profile`).

Los scripts fallan y no crean el paquete si encuentran una ROM, una partida, un volcado o C generado. Las releases llevan `SHA256SUMS`, que el aviso de versión nueva comprueba antes de instalar. Publicar una release es decisión del dueño del repositorio.

A diferencia de `alttp-recomp`, aquí no se sube el motor compilado a la raíz del repositorio: el paquete se construye con el motor de `../gba-recomp` en cada release, y por eso no hace falta un `update-engine.ps1`.
