# 🧠 Qt RFID Monitor

A cross-platform **Qt 6 desktop application** for monitoring workers via RFID helmets.  
Built with **CMake** + **Qt Widgets**, supporting **MySQL (remote)** and designed for Windows, Linux (AppImage), and macOS.

---

## 🚀 Features

- Modular UI: `DashboardPage` and `SearchPage` under `src/pages/`
- Connects to remote MySQL database (`db4free.net`)
- Responsive dashboard layout
- App packaging for:
  - **Linux** → `.AppImage` using `linuxdeploy`
  - **Windows** → `.exe` installer (via windeployqt)
  - **macOS** → `.app` and `.dmg` bundles (via macdeployqt)

---

## 🛠️ Build Instructions

### Linux (AppImage)
```bash
chmod +x build_appimage.sh
./build_appimage.sh
