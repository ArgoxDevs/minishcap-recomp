# minishcap-recomp

*The Legend of Zelda: The Minish Cap* (GBA, USA) ejecutándose de forma nativa en tu PC, construido a partir de **tu
propia ROM** con el motor [`gba-recomp`](../gba-recomp). Este repositorio es el juego: su perfil, el aspecto de su
lanzador y sus scripts. No contiene ninguna ROM ni nada derivado de una. [English version](README.md).

**Estado: temprano.** El juego arranca, pasa la intro, el Festival Minish y la ceremonia del castillo, guarda en la EEPROM y tiene sonido y
estados guardados; no se ha jugado hasta el final. Ver `plan.md`.

## Jugar (Windows)

1. Descomprime el paquete donde quieras.
2. Haz doble clic en `gba-port.exe`, elige una vez tu `minishcap_usa.gba` y pulsa **Jugar**. La primera vez el juego se
   prepara en tu equipo (unos segundos); después arranca al instante. Solo se recuerda la ruta de la ROM.
3. Las partidas van a `saves\`, junto al programa.

También sirve arrastrar la ROM sobre `gba-port.exe`, y desde una terminal: `gba-port.exe <rom.gba> [--no-launch]`.
La ROM debe ser la versión USA (SHA-1 `b4bd50e4131b027c334547b4524e2dbbd4227130`).

Teclas: flechas, `X` = A, `Z` = B, `A` = L, `S` = R, `Enter` = Start, `Retroceso` = Select, `Tab` (mantenida) = avance
rápido, `F11` = pantalla completa, `F5` / `F8` = guardar / cargar estado (`1`-`9` eligen la ranura), `F12` = captura,
`F1` / `Esc` = menú (idioma, vídeo, sonido, cambiar teclas y mando, ranuras de estado, rebobinado y los extras del juego: texto rápido, transiciones rápidas), `R` (mantenida) = rebobinar. También funciona un mando.

## Armar el paquete

`powershell -File scripts\release.ps1` (hace falta el motor compilado: `cargo build --release` en `..\gba-recomp`). El
icono del lanzador lo dibuja `scripts\make_icon.py` (arte propio, nada del juego).

## Linux

Descarga `minishcap-recomp-<versión>-linux-x86_64.tar.gz`, descomprímelo y ejecuta:

```
./gba-port /ruta/a/tu/rom.gba
```

La primera vez traduce y compila el juego desde tu ROM (un par de segundos) y lo guarda en `~/.cache/gba-port`; las partidas van a `saves/`, junto a `games/`. Para enlazarlo hacen falta los ficheros de desarrollo de la libc (`build-essential` o `libc6-dev`). Todavía no hay ventana para elegir la ROM en Linux. El paquete nunca lleva la ROM.

Consulta [docs/DESARROLLO.md](docs/DESARROLLO.md) para construir desde el código, probar y empaquetar, y [CONTRIBUTING.md](CONTRIBUTING.md) para contribuir.
