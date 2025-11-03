#!/usr/bin/env bash
set -euo pipefail

APP=qt_rfid_monitor
APPDIR=AppDir

echo "== 0) Deps =="
sudo apt update
# Qt6 runtime + MySQL client + tools so we can locate Qt6 plugin paths
sudo apt install -y \
  patchelf libqt6sql6 libqt6sql6-mysql libmysqlclient21 \
  qt6-base-dev-tools # gives qtpaths6

echo "== 1) Clean build =="
rm -rf build "$APPDIR" ./*.AppImage || true
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

echo "== 2) Get bundlers =="
[ -f linuxdeploy-x86_64.AppImage ] || \
  wget -O linuxdeploy-x86_64.AppImage \
    https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage

# (Do NOT use linuxdeploy-plugin-qt: it's Qt5-only and breaks with Qt6)

echo "== 3) Create AppDir layout =="
mkdir -p "$APPDIR/usr/bin" "$APPDIR/usr/lib" "$APPDIR/usr/plugins" \
         "$APPDIR/usr/share/applications" "$APPDIR/usr/share/icons/hicolor/256x256/apps"

cp build/$APP "$APPDIR/usr/bin/"

# Optional user config next to the binary if you have one
[ -f config.ini ] && cp config.ini "$APPDIR/usr/bin/"

# Minimal desktop file
cat > "$APPDIR/usr/share/applications/${APP}.desktop" << 'EOF'
[Desktop Entry]
Type=Application
Name=RFID Monitor
Exec=qt_rfid_monitor
Icon=rfid
Categories=Utility;
EOF

# Optional: set an icon if you have one at packaging/rfid.png
[ -f packaging/rfid.png ] && cp packaging/rfid.png "$APPDIR/usr/share/icons/hicolor/256x256/apps/rfid.png"

echo "== 4) Bundle Qt6 plugins & MySQL driver =="
# Locate Qt6 plugin directory
QT_PLUGINS_DIR="$(qtpaths6 --plugin-dir)"

# Ensure plugin subdirs exist
mkdir -p "$APPDIR/usr/plugins/platforms" \
         "$APPDIR/usr/plugins/imageformats" \
         "$APPDIR/usr/plugins/styles" \
         "$APPDIR/usr/plugins/sqldrivers"

# Platforms (required)
cp -v "$QT_PLUGINS_DIR/platforms/libqxcb.so" "$APPDIR/usr/plugins/platforms/"

# Image formats (recommended so icons/pngs work)
if [ -d "$QT_PLUGINS_DIR/imageformats" ]; then
  cp -v "$QT_PLUGINS_DIR"/imageformats/*.so "$APPDIR/usr/plugins/imageformats/" || true
fi

# Styles (optional; useful for Fusion style etc.)
if [ -d "$QT_PLUGINS_DIR/styles" ]; then
  cp -v "$QT_PLUGINS_DIR"/styles/*.so "$APPDIR/usr/plugins/styles/" || true
fi

# SQL drivers: MySQL (libqsqlmysql) — some distros install it in a separate pkg
MYSQL_QT_DRIVER=$(dpkg -L libqt6sql6-mysql | grep -E 'libqsqlmysql\.so$' | head -n1 || true)
if [ -n "${MYSQL_QT_DRIVER}" ]; then
  cp -v "$MYSQL_QT_DRIVER" "$APPDIR/usr/plugins/sqldrivers/"
fi

# MySQL client shared lib (needed by libqsqlmysql)
MYSQL_CLIENT=$(ldconfig -p | awk '/libmysqlclient\.so(\.|$)/{print $4; exit}' || true)
if [ -n "${MYSQL_CLIENT}" ]; then
  cp -v "$MYSQL_CLIENT" "$APPDIR/usr/lib/"
fi

echo "== 5) Build AppImage (no qt plugin) =="
./linuxdeploy-x86_64.AppImage --appdir "$APPDIR" \
  --executable "$APPDIR/usr/bin/$APP" \
  --desktop-file "$APPDIR/usr/share/applications/${APP}.desktop" \
  --output appimage

echo "== 6) Done. Test =="
chmod +x ./*.AppImage
echo "Run: ./*.AppImage"
