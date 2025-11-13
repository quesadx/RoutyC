# RoutyC

RoutyC is a lightweight Qt/C++ desktop application for visualizing transport networks and computing routes between stations. It provides an interactive GUI to explore a transport graph, run path-finding algorithms, and inspect network elements.

**Status:** Local project — CMake build with Qt (no license file included).

**Screenshot:** Add a screenshot to `docs/` or the project root and replace this section.

**Features**
- **Graph visualization:** Interactive view of stations and routes using custom graphics items.
- **Path algorithms:** Shortest-path and routing algorithms implemented under `PathAlgorithm.cpp`.
- **Network management:** Load and manage network data via `NetworkManager` and station tree UI.
- **Extensible GUI:** Qt Widgets based interface (`MainWindow.ui`, `MainWindow.cpp`) with custom view and item classes.

**Quick Start (Linux)**

Prerequisites:
- `cmake` (>= 3.10 recommended)
- A C++ compiler (GCC/Clang)
- Qt development libraries (Qt5 or Qt6 with Widgets and Core modules)

Build and run from project root:

```bash
# create build directory and configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# build the app
cmake --build build --config Release

# run the executable (from repo root)
./build/RoutyC
```

If you prefer a debug build use `-DCMAKE_BUILD_TYPE=Debug` and run the debug binary at `./build/RoutyC`.

Sample data and runtime files are present in the `build/` folder (for example `rutas.txt`, `estaciones.txt`, `recorridos_rutas.txt`, `cierres.txt`) which the application may use for populating the transport network.

**Project Structure (important files)**
- `CMakeLists.txt` : build configuration.
- `main.cpp` : application entry point.
- `MainWindow.ui`, `MainWindow.cpp` : main GUI window and event handling.
- `TransportGraph.*` : graph model for stations and routes.
- `PathAlgorithm.*` : routing and path-finding implementations.
- `NetworkManager.*` : network loading/saving and data handling.
- `CustomGraphicsView.*`, `GraphicsItems.*` : custom QGraphicsView and items for visualization.
- `resources.qrc`, `styles.qss` : UI resources and styles.

**Development**
- Open the project in Qt Creator (open `CMakeLists.txt`) or use the above `cmake` commands.
- To enable verbose compile output or run unit-style checks, build with `-DCMAKE_BUILD_TYPE=Debug`.

**Contributing**
- Report issues or feature requests using the repository issue tracker.
- Send pull requests with clear descriptions and minimal, focused changes.

**License**
No license file found. Add a `LICENSE` if you want to publish or share the project under specific terms.

---
 
