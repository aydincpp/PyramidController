This is my second project using C++ and OpenGL.
In the past month, I studied linear algebra and tried to understand how the model-view projection works.

![Demo](./demo.GIF)

## Dependencies

- OpenGL 3.3 or higher
- C++ compiler
- CMake
- On Windows, ensure MinGW's g++ is installed and in your PATH.

## Build

### Linux / macOS
```bash
cmake -S . -B build
cmake --build build
./build/PyramidController
```

### Windows (Command Prompt / PowerShell)
```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\PyramidController.exe
```
