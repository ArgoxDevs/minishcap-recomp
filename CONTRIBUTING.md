# Contribuir

Este repositorio es el juego (*The Legend of Zelda: The Minish Cap*, USA): su perfil, su C propio y los scripts de empaquetado. El traductor de ARM/Thumb a C, el runtime (bus, PPU, sonido, BIOS), el lanzador (`gba-port`) y la app de Android están en [`gba-recomp`](https://github.com/ArgoxDevs/gba-recomp). Un fallo del traductor, de la PPU o del sonido va allí.

## Reglas que no se negocian

- **Nunca la ROM ni nada derivado de ella**: ni en commits, ni en issues, ni en PR. Eso incluye volcados de memoria, capturas con gráficos del juego, el C generado, el texto en inglés volcado de la ROM y bytes de tablas copiados de ella. `.gitignore` excluye `*.gba`, `*.sav`, `*.srm`, `roms/`, `out/`, `saves/` y `games/*/lang/*.dump.txt`. Antes de abrir un PR:

  ```
  python ../gba-recomp/tools/scan.py --history
  ```

  Debe terminar en «limpio». El hook `pre-push` lo ejecuta solo si lo instalas (ver `docs/DESARROLLO.md`).
- **Código propio (clean-room).** El C que añadas es original. La decompilación `zeldaret/tmc` y los proyectos que la usan (`999sian/tmc`, `mstan/gbarecomp`) no declaran una licencia clara: sirven de pista para encontrar una dirección o un nombre, pero no se copia su código, y la dirección se comprueba a mano contra la ROM real.
- **Traducciones propias.** El texto en español lo escribes tú; nunca se copia el volcado en inglés de la ROM ni una traducción ajena.

## Qué se puede aportar

- **Opciones del juego** (`games/minishcap_usa/options.c`): se activan con la ROM original, están apagadas por defecto y no cambian la dificultad sin avisar. El mecanismo está en `gba-recomp/docs/opciones-del-juego.md`.
- **Raíces dinámicas** nuevas en `games/dynamic_roots.txt`, halladas jugando (`tools/complete.sh` del motor): una dirección hexadecimal por línea, impar si es Thumb.
- **Traducción al español** en `games/minishcap_usa/lang/es.txt` (formato en `docs/texto.md`).
- **Documentación** de cómo funciona una parte del juego y de las direcciones que usan las opciones.

## Probar

```
powershell -File scripts\check.ps1          # Windows
scripts/check.sh                            # Linux
```

Ejecutan, de menos a más costoso: el escaneo de la ROM, las pruebas del motor y una construcción real del juego desde `roms/minishcap_usa.gba` (se omite si no hay ROM). En GitHub, `ci.yml` hace lo que no necesita la ROM. Si añades una opción, comprueba el efecto en una partida nueva y anótalo en `plan.md` (fotograma, valor antes y después).

## Commits y PR

- Mensajes en español, en imperativo, con el qué y el porqué en la primera línea.
- Describe en el PR qué dirección tocas, cómo la comprobaste y con qué ROM (sin adjuntar nada de ella).
