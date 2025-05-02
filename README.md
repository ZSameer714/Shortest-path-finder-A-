# 🧭 A* Pathfinding Algorithm Visualizer (C++ with SDL2)

This project is a visual implementation of the **A\*** (A-Star) pathfinding algorithm using **C++** and **SDL2**. It includes features like random maze generation and animation of the algorithm’s progress and final path.

## 📷 Features

- A\* pathfinding on a 50x50 grid.
- Animated visualization of node exploration.
- Random maze generation using keypress.
- Click to place **Start**, **End**, and **Walls**.
- Real-time rendering with SDL2.

---

## 🎮 Controls

| Action              | Key / Mouse       |
|---------------------|-------------------|
| Place Start Node     | Left Click (1st)   |
| Place End Node       | Left Click (2nd)   |
| Place Walls          | Left Click         |
| Generate Random Maze | Press `M`          |
| Run A* Algorithm     | Press `Space`      |
| Exit Program         | Window Close (`X`) |

---

## 🛠️ Requirements

Make sure you have the following installed:

- [SDL2](https://www.libsdl.org/)
- A C++17-compatible compiler (like g++ or MSVC)

---

## ⚙️ Build Instructions

### Linux / macOS (using `g++`)

```bash
g++ main.cpp -o pathfinding -lSDL2
./pathfinding
```

## Windows (MinGW with SDL2 installed)
```bash
g++ main.cpp -o pathfinding.exe -lmingw32 -lSDL2main -lSDL2
pathfinding.exe
```
