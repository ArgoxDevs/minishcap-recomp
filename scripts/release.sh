#!/usr/bin/env bash
# Arma out/release/minishcap-recomp-<version>-linux-x86_64.tar.gz: gba-port y gba-recomp del motor (../gba-recomp), su runtime en C,
# TinyCC (compilado aquí desde ../tinycc) y las cabeceras y la biblioteca de SDL2. El usuario descomprime, ejecuta
# `./gba-port <su ROM>` y juega; la primera vez traduce y compila el juego (~2 s) y lo guarda en ~/.cache/gba-port.
# Para compilar necesita las partes de C de la libc (crt1.o, libc.so, libm.so: paquete libc6-dev / glibc-devel; viene con build-essential).
# No lleva ROMs, partidas, estados ni C generado: si algo así acaba en la carpeta, falla y no crea el paquete.
#
#   scripts/release.sh [version]      variables: GBA_ENGINE_REPO (../gba-recomp), TINYCC_DIR (../tinycc, rama mob sin configurar)
# Hace falta (solo para construir): Rust, gcc, make y libsdl2-dev. Para que abra en más distribuciones, constrúyelo en Ubuntu 22.04
# (glibc 2.35): lo hace el workflow linux-package.yml.
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
ver="${1:-0.1.0}"
engine="$(readlink -f "${GBA_ENGINE_REPO:-../gba-recomp}")"
tinycc="$(readlink -f "${TINYCC_DIR:-../tinycc}")"
[ -f "$engine/runtime/gba_rt.h" ] || { echo "falta el motor en $engine (GBA_ENGINE_REPO)"; exit 1; }
[ -f "$tinycc/libtcc.c" ] || { echo "faltan las fuentes de TinyCC en $tinycc (TINYCC_DIR)"; exit 1; }
[ ! -e "$tinycc/config.h" ] || { echo "$tinycc/config.h es de ./configure: usa una copia sin configurar"; exit 1; }
name="minishcap-recomp-$ver-linux-x86_64"
stage="$root/out/release/$name"
work="$root/out/linux-build"
rm -rf "$stage" "$work"; mkdir -p "$stage/toolchain/tcc" "$stage/sdl2/lib" "$work"

# el motor, en un directorio aparte para no tocar su target/
(cd "$engine" && cargo build --release --target-dir "$root/out/engine-target")
cp "$root/out/engine-target/release/gba-port" "$root/out/engine-target/release/gba-recomp" "$stage/"

# TinyCC: se compila en una copia para dejar limpias las fuentes; queda toolchain/tcc/tcc y toolchain/tcc/lib/tcc/ (libtcc1.a, include/)
cp -r "$tinycc" "$work/tinycc"
(cd "$work/tinycc" && ./configure --prefix="$work/tcc-out" >/dev/null && make -j"$(nproc)" >/dev/null && make install >/dev/null)
cp "$work/tcc-out/bin/tcc" "$stage/toolchain/tcc/tcc"
mkdir -p "$stage/toolchain/tcc/lib"
cp -r "$work/tcc-out/lib/tcc" "$stage/toolchain/tcc/lib/tcc"
cp "$tinycc/COPYING" "$stage/toolchain/tcc/COPYING"
cat > "$stage/toolchain/AVISO-TINYCC.txt" <<'TXT'
TinyCC (tcc) se reparte tal cual, sin cambios, bajo la LGPL 2.1 (toolchain/tcc/COPYING). Fuentes: https://github.com/TinyCC/tinycc
TXT

# SDL2: cabeceras y la biblioteca compartida (zlib)
inc="$(sdl2-config --prefix 2>/dev/null || echo /usr)/include"
mkdir -p "$stage/sdl2/include"
cp -r "$inc/SDL2" "$stage/sdl2/include/SDL2"
sdl="$(ldconfig -p | awk '/libSDL2-2.0.so.0 .*x86-64/ {print $NF; exit}')"
[ -n "$sdl" ] || sdl="$(ldd "$stage/gba-recomp" 2>/dev/null | awk '/libSDL2-2.0.so.0/ {print $3}')"
[ -f "${sdl:-}" ] || { echo "no encuentro libSDL2-2.0.so.0 (instala libsdl2-2.0-0)"; exit 1; }
cp -L "$sdl" "$stage/sdl2/lib/libSDL2-2.0.so.0"
[ -f /usr/share/doc/libsdl2-2.0-0/copyright ] && cp /usr/share/doc/libsdl2-2.0-0/copyright "$stage/sdl2/LICENSE-SDL2.txt" || true

cp -r "$engine/runtime" "$stage/runtime"
cp "$engine/LICENSE" "$stage/LICENSE-engine"
cp README.md LICENSE "$stage/"
[ -f README.es.md ] && cp README.es.md "$stage/"
cp -r games "$stage/games"
# entrada de menú de aplicaciones (opcional): ./install-menu.sh la crea apuntando a esta carpeta; sin terminal, gba-port usa los diálogos
# de zenity/kdialog para elegir la ROM la primera vez
cat > "$stage/install-menu.sh" <<'SH'
#!/bin/sh
# Crea ~/.local/share/applications/minishcap-recomp.desktop apuntando a esta carpeta. Quitarla: rm esa entrada.
dir="$(cd "$(dirname "$0")" && pwd)"
apps="${XDG_DATA_HOME:-$HOME/.local/share}/applications"
mkdir -p "$apps"
cat > "$apps/minishcap-recomp.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=The Minish Cap (recomp)
Comment=Game Boy Advance desde tu propia ROM
Exec="$dir/gba-port"
Path=$dir
Icon=applications-games
Terminal=false
Categories=Game;
EOF
echo "creada $apps/minishcap-recomp.desktop"
echo "La primera vez pide la ROM con un diálogo (hace falta zenity o kdialog); gba-port --choose-rom la cambia."
SH
chmod +x "$stage/install-menu.sh"
# lo lee el aviso de versión nueva (update.rs del motor): repositorio y versión instalada
remote="$(git remote get-url origin)"
repo="$(printf '%s' "$remote" | sed -E 's#.*github\.com[:/]([^/]+/[^/]+)$#\1#; s#\.git$##')"
case "$repo" in */*) ;; *) echo "no sé el repositorio de GitHub (git remote origin: $remote)"; exit 1;; esac
printf 'repo=%s\nversion=%s\n' "$repo" "$ver" > "$stage/update.info"
# del motor solo va lo que se ejecuta o se compila: fuera las pruebas y utilidades del runtime
rm -f "$stage/runtime/bios_test.c" "$stage/runtime/gba_test.c" "$stage/runtime/run_headless.c"

# la guarda: nada de ROMs, partidas guardadas ni C generado
bad=$(find "$stage" -type f \( -iname '*.gba' -o -iname '*.sav' -o -iname '*.srm' -o -iname '*.sst' -o -iname '*.wav' -o -iname '*.ppm' -o -iname '*.thumb' -o -regex '.*\.s[1-9]' -o -name gen.c -o -name rom.c \) -print)
[ -z "$bad" ] || { echo "El paquete lleva ficheros que no deben ir:"; echo "$bad"; rm -rf "$stage"; exit 1; }
big=$(find "$stage/games" "$stage/runtime" -type f -size +2M -print)
[ -z "$big" ] || { echo "fichero sospechosamente grande en games/ o runtime/: $big"; rm -rf "$stage"; exit 1; }

tar -C "$root/out/release" -czf "$root/out/release/$name.tar.gz" "$name"
# SHA256SUMS: el aviso de versión nueva comprueba el paquete contra este fichero de la misma release
(cd "$root/out/release" && { grep -v -F "$name.tar.gz" SHA256SUMS 2>/dev/null || true; sha256sum "$name.tar.gz"; } > SHA256SUMS.new && mv SHA256SUMS.new SHA256SUMS)
echo "creado out/release/$name.tar.gz"
