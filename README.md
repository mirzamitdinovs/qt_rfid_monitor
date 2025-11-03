# 🧠 RFID Monitor — Qt 6 Desktop Prototype

This project is a **cross-platform desktop application** built with **C++ and Qt 6**, designed as a **prototype for a worker monitoring system using RFID helmets**.  
It visually represents **workers entering and leaving designated areas** and provides search and filtering functionality through a modern UI.

---

## 📘 Overview

The main purpose of the application is to:

- Display the **current number of workers inside a monitored area**
- Show **recent worker entry and exit logs**
- Allow **filtering and searching** by time, area, worker name, and event type
- Demonstrate **database integration** with a remote MySQL server

This prototype focuses on **UI clarity**, **modular code structure**, and **separation of concerns**, aligning with best practices for scalable Qt applications.

---

## 🧩 Project Structure

```
qt_rfid_monitor/
├── CMakeLists.txt                  # CMake configuration
├── resources.qrc                   # Qt resource collection (icons, qss)
├── src/
│   ├── main.cpp                    # Application entry point
│   │
│   ├── app/
│   │   ├── AppWindow.cpp/.h        # Main window with navigation between pages
│   │   └── AppWindow.qss           # Optional style overrides for the app window
│   │
│   ├── pages/
│   │   ├── dashboard/
│   │   │   ├── DashboardPage.cpp/.h   # Shows active worker count and overview
│   │   │   └── DashboardPage.qss      # Stylesheet for the dashboard UI
│   │   │
│   │   └── search/
│   │       ├── SearchPage.cpp/.h      # Displays searchable worker logs
│   │       └── SearchPage.qss         # Stylesheet for search filters and tables
│   │
│   ├── data/
│   │   ├── InMemoryDataProvider.cpp/.h  # Mock data for offline UI testing
│   │   └── MySqlDataProvider.cpp/.h     # Connects to remote MySQL DB
│   │
│   └── db/
│       └── DbConfig.h               # Contains database credentials and settings
│
└── README.md
```

---

## 🧱 Architecture

The project is organized into three main layers:

### 1. **UI Layer (Presentation)**
- Each page (`DashboardPage`, `SearchPage`) is a standalone QWidget-based module.
- Each page includes its own `.qss` stylesheet for scoped styling.
- The app uses a `QStackedWidget` in `AppWindow` for navigation.

### 2. **Data Layer**
- Abstracts how data is fetched (from memory or MySQL).
- `InMemoryDataProvider` is used during design and offline testing.
- `MySqlDataProvider` uses `QSqlDatabase` to connect and fetch real data.

### 3. **Database Layer**
- Centralized in `db/DbConfig.h`:
  ```cpp
  static constexpr const char *DB_HOST = "sql12.freesqldatabase.com";
  static constexpr int DB_PORT = 3306;
  static constexpr const char *DB_NAME = "sql12805810";
  static constexpr const char *DB_USER = "sql12805810";
  static constexpr const char *DB_PASSWORD = "yWzpTuEhBe";
  ```
- Uses MySQL remote hosting via **db4free.net** for easy integration and testing.

---

## 🧭 Navigation & Functionality

### 🏠 Dashboard Page
- Displays total workers currently inside the monitored area.
- Pulls data from a MySQL table (e.g. `workers_inside`).
- Designed with large, centered counters and clean spacing.

### 🔍 Search Page
- Lists all entry and exit logs.
- Includes:
  - Filter bar (date range, zone, worker, event)
  - Column selector (show/hide columns)
  - Styled data table with hover states and badges
- Each row shows:
  - Date / Time
  - Event Type (`Entry`, `Exit`)
  - Worker Name
  - Contact
  - Memo (optional)
- Footer shows total record count and refresh button.

---

## ⚙️ Design Philosophy

- **Modular structure:** Each page, provider, and style lives in its own folder.
- **Maintainable UI:** CSS-like `.qss` styles keep visuals consistent.
- **Testability:** Can run without database using in-memory mock data.
- **Cross-platform ready:** Designed to build cleanly on Linux, macOS, and Windows.

---

## 🌐 Remote Database Setup

The app connects to a hosted MySQL instance:

| Parameter | Value |
|------------|-------|
| Host | `sql12.freesqldatabase.com` |
| Port | `3306` |
| Database | `sql12805810` |
| User | `sql12805810` |
| Password | `yWzpTuEhBe` |

Two tables are expected:
1. **workers_inside** — stores currently active workers.
2. **worker_logs** — stores full entry/exit history.

---

## 🧰 Development Notes

- Uses **Qt 6 Widgets** (not QML) for better native look and performance.
- Built using **CMake** with modern C++17 syntax.
- All styling handled via `.qss` and Qt’s layout system.
- Prepared for **future MySQL integration** (insert/update, not just read).

---

## 📄 Next Steps

- Add **real-time updates** using timers or socket events.
- Extend database to include **zones and departments**.
- Improve UI responsiveness for larger datasets.
- Add user login & role-based access.

---

## 👨‍💻 Author

**Sayyod Mirzamitdinov**  
Full-stack & Qt Developer  
📧 mirzamitdinovsayyod@gmail.com  

---

## 🧾 License

MIT License — free for personal and educational use.
