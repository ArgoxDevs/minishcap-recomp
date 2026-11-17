/* Gráficos en español (con language=es). Los rótulos del título y los menús son imágenes de la ROM, no mensajes: aquí se dibujan
   con un alfabeto propio (nada se copia de la ROM) y se escriben en la VRAM cuando la pantalla correspondiente los ha cargado.
   gfx_frame() se llama una vez por fotograma desde game_frame() (options.c). */
#include <string.h>
#include "gba_sys.h"

int gfx_es; /* lo pone game_init (text.c) */

/* Letras de 6x10 con trazo de dos píxeles; 'X' es relleno. */
static const char *glyph(char c) {
    switch (c) {
    case 'P': return "XXXXX. XXXXXX XX..XX XX..XX XXXXXX XXXXX. XX.... XX.... XX.... XX....";
    case 'U': return "XX..XX XX..XX XX..XX XX..XX XX..XX XX..XX XX..XX XX..XX XXXXXX .XXXX.";
    case 'L': return "XX.... XX.... XX.... XX.... XX.... XX.... XX.... XX.... XXXXXX XXXXXX";
    case 'S': return ".XXXXX XXXXXX XX.... XX.... XXXXX. .XXXXX ....XX ....XX XXXXXX XXXXX.";
    case 'A': return ".XXXX. XXXXXX XX..XX XX..XX XX..XX XXXXXX XXXXXX XX..XX XX..XX XX..XX";
    case 'T': return "XXXXXX XXXXXX ..XX.. ..XX.. ..XX.. ..XX.. ..XX.. ..XX.. ..XX.. ..XX..";
    case 'R': return "XXXXX. XXXXXX XX..XX XX..XX XXXXX. XXXXX. XX.XX. XX..XX XX..XX XX..XX";
    }
    return NULL;
}

/* Lienzo de 96x16 con los índices de la paleta 14 del rótulo (6..2 relleno amarillo-verde, 14 contorno blanco, 9 sombra). */
static void draw_title(unsigned char px[16][96], const char *text) {
    unsigned char fill[16][96]; memset(fill, 0, sizeof fill); memset(px, 0, 16 * 96);
    int x0 = 6, n = (int)strlen(text);
    for (int i = 0; i < n; i++) {
        const char *g = glyph(text[i]);
        if (!g) continue;
        for (int r = 0; r < 10; r++) for (int c = 0; c < 6; c++)
            if (g[r * 7 + c] == 'X') { int x = x0 + i * 8 + c; if (x < 96) { fill[3 + r][x] = 1; px[3 + r][x] = (unsigned char)(6 - r / 2); } }
    }
    for (int y = 0; y < 16; y++) for (int x = 0; x < 96; x++) {
        if (fill[y][x]) continue;
        for (int dy = -1; dy <= 1; dy++) for (int dx = -1; dx <= 1; dx++) {
            int yy = y + dy, xx = x + dx;
            if (yy >= 0 && yy < 16 && xx >= 0 && xx < 96 && fill[yy][xx]) px[y][x] = 14;
        }
    }
    for (int y = 1; y < 16; y++) for (int x = 0; x < 96; x++)
        if (!px[y][x] && ((px[y - 1][x] == 14) || (y >= 8 && x + 1 < 96 && px[y][x + 1] == 14 && !fill[y][x + 1]))) px[y][x] = 9;
}

/* "PRESS START": tres sprites de 32x16 (4x2 teselas, 8 teselas cada uno) en la tesela 0, 8 y 16 de los objetos, paleta 14. */
static void title_press_start(void) {
    int found = 0;
    for (int i = 0; i < 128; i++) {
        const unsigned char *o = rt_oam + i * 8;
        unsigned a0 = o[0] | o[1] << 8, a1 = o[2] | o[3] << 8, a2 = o[4] | o[5] << 8;
        if (a0 == 0x407C && a2 == 0xE000 && a1 == 0x8048) found |= 1;
        if (a0 == 0x407C && a2 == 0xE008 && a1 == 0x8068) found |= 2;
        if (a0 == 0x407C && a2 == 0xE010 && a1 == 0x8088) found |= 4;
    }
    if (found != 7) return;
    static unsigned char px[16][96], want[768];
    static int built;
    if (!built) {
        draw_title(px, "PULSA START");
        for (int s = 0; s < 3; s++) for (int ty = 0; ty < 2; ty++) for (int tx = 0; tx < 4; tx++)
            for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c += 2) {
                int y = ty * 8 + r, x = s * 32 + tx * 8 + c;
                want[(s * 8 + ty * 4 + tx) * 32 + r * 4 + c / 2] = (unsigned char)(px[y][x] | px[y][x + 1] << 4);
            }
        built = 1;
    }
    if (memcmp(rt_vram + 0x10000, want, sizeof want)) memcpy(rt_vram + 0x10000, want, sizeof want);
}


/* ---- Cabeceras azules de los menús (CHOOSE A FILE, ENTER A NAME): cuatro sprites de texto seguidos (32x16 x3 + 8x16) ---- */

/* Alfabeto de 5x7 (cada trazo se engrosa a 2 píxeles al dibujar). */
static const char *font5(char c) {
    switch (c) {
    case 'A': return "01110 10001 10001 11111 10001 10001 10001";
    case 'B': return "11110 10001 10001 11110 10001 10001 11110";
    case 'C': return "01110 10001 10000 10000 10000 10001 01110";
    case 'D': return "11110 10001 10001 10001 10001 10001 11110";
    case 'E': return "11111 10000 10000 11110 10000 10000 11111";
    case 'F': return "11111 10000 10000 11110 10000 10000 10000";
    case 'G': return "01111 10000 10000 10011 10001 10001 01110";
    case 'H': return "10001 10001 10001 11111 10001 10001 10001";
    case 'I': return "01110 00100 00100 00100 00100 00100 01110";
    case 'J': return "00111 00010 00010 00010 00010 10010 01100";
    case 'K': return "10001 10010 10100 11000 10100 10010 10001";
    case 'L': return "10000 10000 10000 10000 10000 10000 11111";
    case 'M': return "10001 11011 10101 10101 10001 10001 10001";
    case 'N': return "10001 11001 10101 10011 10001 10001 10001";
    case 'O': return "01110 10001 10001 10001 10001 10001 01110";
    case 'P': return "11110 10001 10001 11110 10000 10000 10000";
    case 'Q': return "01110 10001 10001 10001 10101 10010 01101";
    case 'R': return "11110 10001 10001 11110 10100 10010 10001";
    case 'S': return "01111 10000 10000 01110 00001 00001 11110";
    case 'T': return "11111 00100 00100 00100 00100 00100 00100";
    case 'U': return "10001 10001 10001 10001 10001 10001 01110";
    case 'V': return "10001 10001 10001 10001 10001 01010 00100";
    case 'W': return "10001 10001 10001 10101 10101 11011 10001";
    case 'X': return "10001 10001 01010 00100 01010 10001 10001";
    case 'Y': return "10001 10001 01010 00100 00100 00100 00100";
    case 'Z': return "11111 00001 00010 00100 01000 10000 11111";
    }
    return NULL;
}

#define BW 160
/* Lienzo de hasta BWx16: paleta azul 7 o aviso rojo 6; fondo 5, anillos c/e y relleno 7. */
static void draw_banner(unsigned char px[16][BW], const char *text, int width) {
    static unsigned char fill[16][BW];
    memset(fill, 0, sizeof fill); memset(px, 5, 16 * BW);
    int n = (int)strlen(text), x0 = (width - (n * 8 - 2)) / 2 - 1;
    for (int i = 0; i < n; i++) {
        const char *g = font5(text[i]);
        if (!g) continue;
        for (int r = 0; r < 7; r++) for (int c = 0; c < 5; c++)
            if (g[r * 6 + c] == '1')
                for (int dy = 0; dy < 2; dy++) for (int dx = 0; dx < 2; dx++) {
                    int x = x0 + i * 8 + c + dx, y = 3 + r + dy;
                    if (x >= 0 && x < width && y < 16) fill[y][x] = 1;
                }
    }
    for (int y = 0; y < 16; y++) for (int x = 0; x < width; x++) {
        int d = 9;
        for (int dy = -3; dy <= 3; dy++) for (int dx = -3; dx <= 3; dx++) {
            int yy = y + dy, xx = x + dx;
            if (yy < 0 || yy >= 16 || xx < 0 || xx >= width || !fill[yy][xx]) continue;
            int m = (dx < 0 ? -dx : dx) > (dy < 0 ? -dy : dy) ? (dx < 0 ? -dx : dx) : (dy < 0 ? -dy : dy);
            if (m < d) d = m;
        }
        px[y][x] = fill[y][x] ? (y == 3 ? 6 : 7) : d == 1 ? 12 : d == 2 ? 14 : d == 3 ? 6 : 5;
    }
}

static unsigned crc32_of(const unsigned char *p, int n) {
    unsigned c = 0xFFFFFFFFu;
    while (n--) { c ^= *p++; for (int k = 0; k < 8; k++) c = c & 1 ? (c >> 1) ^ 0xEDB88320u : c >> 1; }
    return ~c;
}

/* Cabeceras de 104 o 160 píxeles: sprites contiguos de 32x16 y, si hace falta, uno final de 8x16. */
static void banner(const char *text, unsigned crc_en, int width, int palette) {
    int tile = -1;
    for (int i = 0; i < 128 && tile < 0; i++) {
        const unsigned char *o = rt_oam + i * 8;
        unsigned a0 = o[0] | o[1] << 8, a1 = o[2] | o[3] << 8, a2 = o[4] | o[5] << 8;
        if (a0 == 0x4000 && (a1 & 0x1FF) == 32 && (a2 >> 12) == (unsigned)palette && (a1 >> 14) == 2) tile = a2 & 0x3FF;
    }
    int tiles = width / 4;
    if (tile < 0 || tile > 1024 - tiles || crc32_of(rt_vram + 0x10000 + tile * 32, tiles * 32) != crc_en) return;
    static unsigned char px[16][BW];
    draw_banner(px, text, width);
    unsigned char *dst = rt_vram + 0x10000 + tile * 32;
    for (int s = 0; s * 32 < width; s++) {
        int tiles_w = width - s * 32 >= 32 ? 4 : (width - s * 32) / 8;
        for (int ty = 0; ty < 2; ty++) for (int tx = 0; tx < tiles_w; tx++) {
            int t = s * 8 + ty * tiles_w + tx;
            for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c += 2) {
                int y = ty * 8 + r, x = s * 32 + tx * 8 + c;
                dst[t * 32 + r * 4 + c / 2] = (unsigned char)(px[y][x] | px[y][x + 1] << 4);
            }
        }
    }
}

/* Rótulos de 40/48x16: sprite de 32x16 seguido de uno de 8/16x16.
   Se comprueban tanto la OAM como el CRC antes de escribir; el dibujo es propio y conserva la paleta de selección. */
static void menu_label(int tile, int width, unsigned crc_en, const char *text, int style) {
    int seen = 0, tiles = width / 4;
    for (int i = 0; i < 128; i++) {
        const unsigned char *o = rt_oam + i * 8;
        unsigned a0 = o[0] | o[1] << 8, a1 = o[2] | o[3] << 8, a2 = o[4] | o[5] << 8;
        if ((a0 & 0xC300) == 0x4000 && (a1 >> 14) == 2 && (a2 & 0x3FF) == (unsigned)tile) seen = 1;
    }
    unsigned char *dst = rt_vram + 0x10000 + tile * 32;
    if (!seen || tile < 0 || tile + tiles > 1024 || crc32_of(dst, tiles * 32) != crc_en) return;
    unsigned char px[16][48] = {{0}}, ink[16][48] = {{0}};
    if (style == 1) { /* FIN: botón propio; la paleta cambia al seleccionarlo. */
        for (int y = 1; y < 15; y++) for (int x = 1; x < width - 1; x++) {
            if ((x == 1 || x == width - 2) && (y == 1 || y == 14)) continue;
            px[y][x] = y == 1 || y == 14 || x == 1 || x == width - 2 ? 7 :
                           y == 2 || x == 2 ? 14 : 10;
        }
    }
    int n = (int)strlen(text), x0 = (width - (n * 6 - 1)) / 2, y0 = style == 2 ? 1 : 4;
    for (int i = 0; i < n; i++) {
        const char *g = font5(text[i]);
        if (!g) continue;
        for (int y = 0; y < 7; y++) for (int x = 0; x < 5; x++) if (g[y * 6 + x] == '1') {
            int xx = x0 + i * 6 + x, yy = y0 + y;
            if (xx < 0 || xx >= width) continue;
            ink[yy][xx] = 1;
            if (style == 0) ink[yy + 1][xx] = 1;
        }
    }
    if (style == 2) { /* OPCIONES: letras blancas con contorno negro sobre el fondo. */
        for (int y = 0; y < 16; y++) for (int x = 0; x < width; x++) if (ink[y][x])
            for (int dy = -1; dy <= 1; dy++) for (int dx = -1; dx <= 1; dx++)
                if (x + dx >= 0 && x + dx < width && y + dy >= 0 && y + dy < 16) px[y + dy][x + dx] = 15;
    } else if (style == 0) {
        for (int y = 0; y < 15; y++) for (int x = 0; x < width - 1; x++) if (ink[y][x]) px[y + 1][x + 1] = 12;
    }
    for (int y = 0; y < 16; y++) for (int x = 0; x < width; x++) if (ink[y][x]) px[y][x] = style == 1 ? 7 : 14;
    for (int y = 0; y < 16; y++) for (int x = 0; x < width; x += 2) {
        int t = x < 32 ? (y / 8) * 4 + x / 8 : 8 + (y / 8) * ((width - 32) / 8) + (x - 32) / 8;
        dst[t * 32 + (y % 8) * 4 + (x % 8) / 2] = px[y][x] | px[y][x + 1] << 4;
    }
}

/* ---- Pantalla de estado de la misión: cabecera (dos sprites de 64x32) y botones REPOSO/GUARDAR (32x32 + 16x32, 48x19 útiles) ---- */

#define PW 128
/* Lienzo de dos sprites contiguos en 1D (A de wa píxeles y B de wb, h de alto) leído de la VRAM de objetos y vuelto a escribir. */
static int pair_idx(int tileA, int wa, int tileB, int wb, int x, int y, int *off, int *odd) {
    int inA = x < wa, w = inA ? wa : wb, t = inA ? tileA : tileB, xx = inA ? x : x - wa;
    *off = (t + (y / 8) * (w / 8) + xx / 8) * 32 + (y % 8) * 4 + (xx % 8) / 2; *odd = xx & 1;
    return 0;
}
static int pair_get(int tileA, int wa, int tileB, int wb, int x, int y) {
    int off, odd; pair_idx(tileA, wa, tileB, wb, x, y, &off, &odd);
    unsigned char b = rt_vram[0x10000 + off]; return odd ? b >> 4 : b & 15;
}
static void pair_set(int tileA, int wa, int tileB, int wb, int x, int y, int v) {
    int off, odd; pair_idx(tileA, wa, tileB, wb, x, y, &off, &odd);
    unsigned char *b = &rt_vram[0x10000 + off]; *b = odd ? (unsigned char)((*b & 15) | v << 4) : (unsigned char)((*b & 0xF0) | v);
}
static int obj_seen(int tile, unsigned shape, unsigned size) {
    for (int i = 0; i < 128; i++) {
        const unsigned char *o = rt_oam + i * 8;
        unsigned a0 = o[0] | o[1] << 8, a1 = o[2] | o[3] << 8, a2 = o[4] | o[5] << 8;
        if ((a0 & 0x0300) == 0 && (a0 >> 14) == shape && (a1 >> 14) == size && (int)(a2 & 0x3FF) == tile) return 1;
    }
    return 0;
}

/* Texto de font5 en un lienzo de índices: cada fila origen se repite hasta rows filas; bold ensancha el trazo a 2 columnas. */
static void paint5(unsigned char *cv, int stride, int h, int x0, int y0, const char *text, int pitch, int bold, int rows, int ink) {
    for (int i = 0; text[i]; i++) {
        const char *g = font5(text[i]);
        if (!g) continue;
        for (int y = 0; y < rows; y++) for (int c = 0; c < 5; c++) {
            if (g[(y * 7 / rows) * 6 + c] != '1') continue;
            for (int d = 0; d <= bold; d++) {
                int x = x0 + i * pitch + c + d, yy = y0 + y;
                if (x >= 0 && x < stride && yy >= 0 && yy < h) cv[yy * stride + x] = (unsigned char)ink;
            }
        }
    }
}

/* Botón de 48 píxeles: se conserva el marco (columnas 0-1 y 46-47, filas 0-3 y 16-18), se borra el interior y se escribe el texto. */
static void status_button(int tileA, int tileB, unsigned crc_en, const char *text, int pitch, int bold) {
    if (!obj_seen(tileA, 0, 2) || crc32_of(rt_vram + 0x10000 + tileA * 32, 16 * 32) != crc_en) return;
    int n = (int)strlen(text), width = (n - 1) * pitch + 5 + bold;
    static unsigned char cv[32 * 48];
    memset(cv, 0, sizeof cv);
    for (int y = 3; y <= 16; y++) for (int x = 2; x <= 45; x++) cv[y * 48 + x] = 14; /* el fondo es el índice 14 (e) */
    paint5(cv, 48, 32, (48 - width) / 2, 4, text, pitch, bold, 12, 3);
    for (int y = 3; y <= 16; y++) for (int x = 2; x <= 45; x++) pair_set(tileA, 32, tileB, 16, x, y, cv[y * 48 + x]);
}

/* Cabecera "QUEST STATUS": se borra la zona del texto con el color de la columna 8 de cada fila y se escribe con relleno 9 y contorno 6. */
static void status_title(unsigned crc_en, const char *text) {
    if (!obj_seen(64, 1, 3) || crc32_of(rt_vram + 0x10000 + 64 * 32, 32 * 32) != crc_en) return;
    static unsigned char cv[32 * PW], ink[32 * PW];
    memset(ink, 0, sizeof ink);
    int n = (int)strlen(text), x0 = 12 + (90 - ((n - 1) * 9 + 7)) / 2;
    paint5(ink, PW, 32, x0, 6, text, 9, 2, 14, 1);
    for (int y = 0; y < 32; y++) for (int x = 0; x < PW; x++) cv[y * PW + x] = (unsigned char)pair_get(64, 64, 96, 64, x, y);
    for (int y = 5; y <= 21; y++) for (int x = 12; x <= 101; x++) cv[y * PW + x] = cv[y * PW + 8];
    for (int y = 5; y <= 21; y++) for (int x = 12; x <= 101; x++) {
        if (ink[y * PW + x]) { cv[y * PW + x] = 9; continue; }
        for (int dy = -1; dy <= 1; dy++) for (int dx = -1; dx <= 1; dx++)
            if (ink[(y + dy) * PW + x + dx]) cv[y * PW + x] = 6;
    }
    for (int y = 0; y < 32; y++) for (int x = 0; x < PW; x++) pair_set(64, 64, 96, 64, x, y, cv[y * PW + x]);
}

void gfx_frame(void) {
    if (!gfx_es) return;
    title_press_start();
    banner("ELIGE ARCHIVO", 0xD2B12865u, 104, 7);
    banner("TU NOMBRE", 0x648BB2C5u, 104, 7);
    banner("ELIGE DESTINO", 0xD14F8C40u, 160, 7);
    banner("CONFIRMAR BORRADO", 0x4C33DF43u, 160, 6);
    menu_label(270, 48, 0x8AF1BF8Cu, "OPCIONES", 2);
    menu_label(768, 40, 0xAEA3E515u, "COPIAR", 0);
    menu_label(790, 40, 0x13BDF516u, "BORRAR", 0);
    menu_label(800, 40, 0x1E34E143u, "JUGAR", 0);
    menu_label(747, 40, 0x144EAB32u, "FIN", 1);
    menu_label(779, 40, 0xB8C70533u, "VOLVER", 0);
    menu_label(270, 48, 0x76FFA16Cu, "HABLAR", 2);
    status_title(0xBC9F626Du, "ESTADO");
    status_title(0x689BC7FBu, "OBJETOS");
    status_button(736, 752, 0x97542EE3u, "REPOSO", 6, 0);
    status_button(704, 720, 0x97BF24E2u, "GUARDAR", 6, 0);
}
