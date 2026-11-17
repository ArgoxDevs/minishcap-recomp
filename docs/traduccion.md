# Traducción al español: criterios y glosario

Texto propio, escrito a mano a partir de lo que dice el juego (nunca copiado de otra traducción ni del volcado). Se traduce el **sentido** y el tono, no palabra por palabra: si una frase literal suena rara, se reescribe como lo diría alguien en español. Tuteo siempre. Los nombres propios no se traducen; los apodos o nombres descriptivos sí.

## Reglas técnicas

- Los códigos entre llaves (`{06}{00}` nombre del jugador, `{02}{01}` color, `{05}{03}..` cursor de opciones, etc.) se copian tal cual y en el mismo orden relativo; el byte que sigue a un código puede parecer una letra (`!`, `%`): es un parámetro, no se toca.
- Los saltos `\n` se rehacen para que cada línea quepa (el inglés llega a unos 36 caracteres por línea; en español se intenta no pasar de 34) y se conserva el número de líneas del cuadro cuando se puede.
- Letras con acento, `ñ`, `¿` y `¡` van directas en UTF-8.
- En la introducción (grupo 15, mensajes 1–6) se conserva el número de líneas del original: las vidrieras tienen espacio fijo.
  `check_text.ps1` lo comprueba y devuelve código 1 ante avisos; `rewrap.ps1` excluye este grupo del reajuste automático.

## Glosario

| Inglés | Español |
|---|---|
| Minish / Picori | Minish (siempre, por preferencia del dueño; Festival Minish, Hoja Minish) |
| Kinstone / Kinstone Piece / Kinstone Bag | piedra de fusión / fragmento de fusión / bolsa de fusión |
| Heart Container / Piece of Heart | Contenedor de Corazón / Pieza de Corazón |
| Rupee | rupia |
| Picolyte | picolito |
| Gust Jar | Vasija Vendaval |
| Cane of Pacci | Bastón de Pacci |
| Mole Mitts | Guantes de Topo |
| Roc's Cape | Capa del Roc |
| Pegasus Boots | Botas de Pegaso |
| Fire Rod | Vara de Fuego |
| Ocarina of Wind | Ocarina del Viento |
| Flame Lantern | Linterna de Fuego |
| Power Bracelets / Grip Ring / Flippers | Brazaletes de Poder / Anillo de Agarre / Aletas |
| Boomerang | Bumerán |
| Four Sword | Espada Cuádruple |
| Smith's Sword | Espada del Herrero |
| Nayru's / Farore's / Din's Charm | Amuleto de Nayru / de Farore / de Din |
| Spin / Roll / Dash Attack | Ataque Giratorio / Rodante / Veloz |
| Rock Breaker / Sword Beam / Down Thrust | Rompepiedras / Rayo de Espada / Estocada Descendente |
| Mt. Crenel / Lon Lon Ranch | Monte Crenel / Rancho Lon Lon |
| Cucco | cuco |
| Swordsman's newsletter | Boletín del Espadachín |

## Rótulos que son imágenes (`games/minishcap_usa/gfx.c`)

"PRESS START" y las cabeceras de los menús ("CHOOSE A FILE", "ENTER A NAME") no son mensajes sino sprites de la ROM. Con
`language=es`, `gfx_frame()` (una vez por fotograma, desde `game_frame()`) los reconoce en la OAM —y, las cabeceras, por el
CRC32 de sus teselas en inglés— y escribe encima teselas dibujadas con un alfabeto propio (nada se copia de la ROM):
"PULSA START", "ELIGE ARCHIVO" y "TU NOMBRE". La revisión del festival añade OPCIONES, JUGAR, COPIAR, BORRAR, FIN,
VOLVER, ELIGE DESTINO, CONFIRMAR BORRADO y HABLAR. La pantalla de objetos y la de estado de la misión (2026-11-17) llevan ahora OBJETOS, ESTADO
(cabeceras de dos sprites de 64x32, tiles 64 y 96 de objetos), REPOSO y GUARDAR (botones de 32x32 + 16x32); se comprueban por OAM
y CRC de las teselas en inglés y el marco se conserva. Pendientes: el mapa y otros gráficos aún sin revisar; ABC/abc se mantienen como indicadores de mayúsculas/minúsculas, y el logotipo se deja.

## Revisión del 2026-11-17

Recuento comprobado: 2705 mensajes traducidos de 3699 entradas del banco. Las restantes incluyen entradas vacías,
nombres propios, depuración y habla Minish; no equivalen a 994 diálogos pendientes.

Se corrigieron 27 mensajes: saltos que aislaban palabras en avisos de guardado y diálogos, las opciones Sí/No del modo
reposo y siete líneas largas en créditos y boletín. `scripts/check_text.ps1` comprueba los 2705 mensajes sin avisos con
su límite actual de 42 caracteres, conservando la secuencia de códigos de control. También se verificó con `text.c`
que se codifican los 2705 mensajes y se reconstruye el banco en memoria desde la ROM USA. Falta revisar estos cambios
en pantalla; el ancho en caracteres no garantiza el ajuste en píxeles ni la colocación de los cursores.

## Revisión visual sin teclado físico

`scripts/review_translation.ps1` compila el ejecutor `run_headless` del motor con el mismo C del juego y la misma PPU.
Las entradas se programan por fotograma, sin depender del foco de Windows ni de pulsaciones de escritorio. Por defecto
pulsa START en el fotograma 2300 y A en el 3100, y captura título, selección de archivo y pantalla de nombre:

```powershell
.\scripts\review_translation.ps1
.\scripts\review_translation.ps1 -Frames 4200 -Press start@2300+20,a@3100+12,right@3600+12 -ShotAt 3500,4000 -Session cursor
```

Requiere el motor compilado, TinyCC (por defecto el de `../alttp-recomp`, o `-Tcc`/`GBA_PORT_CC`) y Python con Pillow.
`-InputScript` acepta el formato del motor: `KEY FRAME LEN`, con teclas `a b select start right left up down r l`.
Los fotogramas cuentan desde el arranque; las capturas son anteriores a que la pulsación de ese fotograma tenga efecto.
La ROM de trabajo, EEPROM, C generado y capturas PPM/PNG quedan en `out/translation-visual/<Session>` (ignorado por git).
La EEPROM de esa sesión persiste entre ejecuciones; usa otro nombre de sesión para empezar una partida vacía.

Comprobado el 2026-11-17: START abre la selección de archivo y A abre la pantalla de nombre. «PULSA START», «ELIGE ARCHIVO»
y «TU NOMBRE» se ven completos, sin recortes. END se sustituyó después por FIN. Esto permite continuar la revisión
visual del juego; no verifica la entrada de teclado de la ventana SDL ni el sonido del dispositivo.

### Segunda revisión visual y pruebas (2026-11-17)

- Preferencia del dueño: se sustituyeron 85 menciones de Picori por Minish en el español, incluidos Festival Minish y Hoja Minish.
- Se revisaron título, selección de archivo, introducción del nombre y la apertura de una partida. Los gráficos OPTIONS,
  START, COPY, ERASE y END se tradujeron en la revisión posterior del festival.
- Se corrigieron seis textos de introducción y nueve diálogos iniciales. Las capturas confirman el saludo, las tres páginas
  del primer diálogo del Herrero y las primeras dos páginas de la respuesta de Zelda, con acentos, colores y nombre del jugador.
  La vidriera del héroe tenía exceso de líneas y un punto fuera de sitio; la versión corregida cabe y ya no deja ese artefacto.
- Recorrido reproducible con una EEPROM de prueba que ya contiene un archivo: START 2300, A 3100 y A 3600; capturas de la
  introducción en 4500, 5000, 5550 y 6000. Pulsar A en 9500, 10000, 10500, 11000 y 11500 permite revisar los primeros diálogos.
  Todos estos números son fotogramas, con pulsaciones de 12 fotogramas (START: 20). No se ha revisado el juego completo.
- Pruebas: 2705 mensajes sin avisos, 22 pruebas Rust, 59 de BIOS y construcción real desde la ROM. En el motor se reprodujo y
  corrigió una carga de estado truncado que sobrescribía parcialmente la RAM (`runtime/gba_state.c`); las 15 comprobaciones de
  `runtime/state_test.c` pasan. Otra ejecución de 10800 fotogramas verificó guardar, cargar y 40 pasos de rebobinado sin diferencias.

### Festival Minish y guardado (2026-11-17)

El recorrido dirigido salió de la habitación y la casa, llegó a Hyrule, siguió las conversaciones de Zelda y completó el
sorteo y la entrega del escudo. Permitió descubrir 19 raíces dinámicas que faltaban (78 en total, en
`games/dynamic_roots.txt`); se añadieron únicamente las direcciones que reportó la ejecución. El perfil traduce ahora
7175 funciones. Se corrigieron 17 textos más: llegada al festival, conversaciones de Zelda, cuento del anciano, vendedor,
sorteo, escudo, descripción de guardar e instrucciones de reposo. El cuento pasó de 33 a 18 líneas, sin eliminar contenido de la leyenda.

Se probaron Guardar y No guardar con sus cursores: No guardar conserva el SHA-256 de la EEPROM; Guardar lo cambia.
Una ejecución nueva, sin cargar estados, recuperó el progreso y mostró el escudo en el inventario. En el festival se
comprobaron también los cursores Sí/No de reposo: No vuelve al menú; Sí permite entrar y volver con SELECT+L+R.
La instrucción de salida se acortó para que quepa holgadamente en su panel. En el festival se
repitieron guardar/cargar estado y 40 pasos de rebobinado, todos verificados. Los nueve rótulos gráficos adicionales
usan una fuente propia y comprobación de OAM/CRC; se verificó que el parche es estable al repetirse y no escribe en VRAM
con el idioma inglés. También se comprobó la copia a otro archivo sobre la EEPROM de prueba.

`scripts/check.ps1` y `scripts/check.sh` incorporan las pruebas C de BIOS y estados cuando encuentran compilador.
La validación de Windows pasa: 22 pruebas Rust, 59 de BIOS, 15 de estados, 2705 mensajes sin avisos y construcción real.
En WSL/Linux también pasan el perfil, la compilación del C propio, las 22 pruebas Rust y las 74 comprobaciones C con GCC
(`check.sh --no-build`); no se repitió el empaquetado Linux. Se eliminó una definición duplicada de `rt_site` en los tests
que TinyCC toleraba pero GCC rechazaba.
Las capturas y los derivados permanecen en `out/`, fuera de Git. Esto cubre el inicio y el festival; no el juego completo.

El ejecutor visual admite `-SaveState 300:nombre`, `-LoadState 1:nombre` y `-GameOpt fast_text=2`; los estados se guardan
dentro de la sesión. Usa sesiones diferentes para ejecuciones simultáneas, porque compilan su propio `run.exe`.
No reutilices estados tomados a mitad de un diálogo después de editar la traducción: los desplazamientos del banco
pueden cambiar. Para volver a comprobar un texto, carga un estado anterior a abrir ese diálogo o arranca desde la EEPROM.
