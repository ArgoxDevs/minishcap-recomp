# minishcap-recomp

*The Legend of Zelda: The Minish Cap* (USA, SHA-1 `b4bd50e4131b027c334547b4524e2dbbd4227130`, 16 MiB, guardado EEPROM) recompilado con el motor `gba-recomp` (repositorio hermano `..\gba-recomp`). Este repositorio es el **juego**: perfil, C propio y scripts. Repositorios hermanos de referencia: `..\snes-recomp` y `..\alttp-recomp` (misma estructura: motor general + perfil `games\<id>.toml` + `games\<id>\*.c`).

## Estado y referencias

- Revisado hasta el Festival Minish y la entrega del escudo; comprobados guardado, recarga de la EEPROM, opciones Sí/No y rebobinado. No se ha jugado hasta el final.
- Traducción: 2705 mensajes en español de 3699 entradas del banco. Las restantes incluyen entradas vacías, nombres propios, depuración y habla Minish; no equivalen a diálogos pendientes.
- Perfil operativo, con 79 raíces dinámicas. El motor ya lee el perfil y recompila el juego; no es un borrador sin soporte.
- Estado detallado y pendientes: `plan.md`. Criterios de traducción y cobertura visual: `docs/traduccion.md`. Construcción, pruebas y paquetes: `docs/DESARROLLO.md`. Formato del texto: `docs/texto.md`.
- Estos dos archivos, `CLAUDE.md` y `AGENTS.md`, deben mantenerse idénticos. Al modificar las instrucciones, actualizar ambos.

## Estructura

- `roms\minishcap_usa.gba`: la ROM del usuario (en `.gitignore`).
- `games/minishcap_usa.toml`: identificación de la ROM, overlays, raíces, punto de estados guardados y aspecto del lanzador. Solo direcciones, nombres y tamaños.
- `games/dynamic_roots.txt`: destinos de llamadas indirectas descubiertos ejecutando el juego. Añadir las direcciones que reporte el runtime y repetir el tramo que falló.
- `games/minishcap_usa/text.c`: reconstruye el banco de mensajes en memoria con `lang/es.txt`. `--language dump` vuelca el inglés de la ROM del usuario a `lang/en.dump.txt`, ignorado por Git.
- `games/minishcap_usa/gfx.c`: rótulos gráficos en español, dibujados con una fuente propia y comprobación de OAM/CRC. Quedan otros gráficos internos por traducir, como ITEMS, QUEST STATUS, SAVE y SLEEP.
- `games/minishcap_usa/options.c`: texto rápido y transiciones rápidas, en la categoría Extras del menú F1. Se compila con `-DRT_GAME -DRT_GAME_OPTS`.
- `scripts/check.ps1` y `scripts/check.sh`: escaneo, pruebas del motor, pruebas C de BIOS/estados si hay compilador y construcción real si hay ROM.
- `scripts/check_text.ps1`: códigos de control, longitud de líneas y número de líneas de la introducción; devuelve código 1 ante avisos. Requiere el volcado local del inglés.
- `scripts/review_translation.ps1`: revisión visual con entradas por fotograma y capturas de la PPU, sin depender del teclado físico.
- `scripts/release.ps1` y `scripts/release.sh`: paquetes de Windows y Linux. `scripts/make_icon.py`: icono y emblema propios.
- `docs/`: documentación. `out/`: ejecutables de prueba, ROM de trabajo, estados y capturas, siempre fuera de Git.

## Comprobaciones y revisión visual

```powershell
powershell -File scripts/check.ps1
powershell -File scripts/check_text.ps1
powershell -File scripts/review_translation.ps1
```

En Linux: `scripts/check.sh` (o `--no-build` para omitir la construcción desde la ROM).

- Para construir hace falta el motor compilado (`cargo build --release` en `../gba-recomp`), TinyCC y SDL2. Las rutas y dependencias están en `docs/DESARROLLO.md`.
- La revisión visual usa TinyCC (`-Tcc` o `GBA_PORT_CC`), Python y Pillow. Acepta `-Press`, `-InputScript`, `-ShotAt`, `-SaveState 300:nombre`, `-LoadState 1:nombre` y `-GameOpt fast_text=2`.
- Cada `-Session` mantiene una EEPROM de prueba y estados en `out/translation-visual/<Session>`. Usar sesiones distintas para ejecuciones simultáneas; compilan su propio `run.exe`.
- Los fotogramas de entrada cuentan desde el arranque del proceso, incluso al cargar un estado. No reutilizar estados tomados a mitad de un diálogo después de editar la traducción: pueden cambiar los desplazamientos del banco. Cargar uno anterior al diálogo o arrancar desde la EEPROM.
- Las partidas originales no se usan como destino de las pruebas. Comprobar los textos en imágenes: el límite de caracteres no garantiza el ancho real ni la posición del cursor.
- Actualizar `plan.md` y `docs/traduccion.md` con el tramo probado, las correcciones y los límites de la revisión.

## Traducción

- Usar siempre **Minish** en español, también **Festival Minish** y **Hoja Minish**, por preferencia del dueño. Seguir el glosario de `docs/traduccion.md`.
- Conservar los códigos de control y sus parámetros, incluidos los que parecen letras o signos. Los acentos, la ñ y los signos de apertura se escriben en UTF-8.
- La introducción (grupo 15, mensajes 1–6) debe conservar el número de líneas del original. `rewrap.ps1` excluye las pantallas propias de los grupos 1, 3 y 15.
- El texto es propio; no copiar traducciones ajenas ni incorporar el volcado en inglés al repositorio.

## Reglas

- Nunca la ROM ni nada derivado: ni C generado, volcados de memoria/texto, capturas con gráficos del juego ni partidas. `roms/`, `out/`, `saves/`, `*.gba`, `*.srm`, `*.sav`, `*.sst` y `games/*/lang/*.dump.txt` están en `.gitignore`. Antes de cada push ejecutar `python ../gba-recomp/tools/scan.py --history`; debe terminar en «limpio».
- El C es original (clean-room). La decompilación `zeldaret/tmc` y los proyectos que la usan (`999sian/tmc`, `mstan/MinishCapRecomp`) solo son referencia; de la decompilación, como mucho nombres de símbolos, nunca su C.
- Lo del traductor, la PPU, el sonido o el runtime va en `gba-recomp`; aquí solo lo específico de este juego.
- Documentación y mensajes de commit en español.
