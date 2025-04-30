# Rendering Engine

Simple Vulkan-based 3D renderer written in C++. Designed for learning basics of a game engine and graphics programming with OPENGL. The project is converted to Vulkan for interracting with low-level graphics API.

# Features

- Vulkan API rendering pipeline
- Real-time rendering with shaders (simple PBR shader support)
- Omnidirectional shadow mapping
- HDR skybox
- Model loading (.fbx)
- Camera and input controls (P to disable cursor, basic WASD and mouse movement)
- Cross-platform CMake build system (engine itself is Windows only currently)
  
![Ekran görüntüsü 2025-05-01 020133](https://github.com/user-attachments/assets/d87f63f5-6ba1-4f08-9a0c-fdde3606d911)
![Ekran görüntüsü 2025-05-01 015856](https://github.com/user-attachments/assets/16e2610f-c1ab-4023-961a-944ce59f4a34)
![image](https://github.com/user-attachments/assets/86d1cec9-028d-4db5-bbec-ad38451e7b11)
![image](https://github.com/user-attachments/assets/1d33547b-8b79-4726-ac1e-a5b635d3b1de)

# Will Be Implemented

## Editor Side
- Imgui for real-time control on transformations and object attributes 
- File system to load models properly in real-time
- Save/Load system for level editor

## Render Side
- Better OPENGL abstraction (currently disabled)
- Object transparency
- Tessellation
- Simple physics 
- Skeletal animation

# Current Deficiencies/Bugs

- Just for release configuration; textures may not initialize properly at first run. 2 times restarting may be necessery. 
- Point lights of the scene (yellow and blue boxes) are detected by some fragments even if they should be blocked by object itself. This creates artifacts on shadows on objects.
- Currently, there is no file interaction system to load external .fbx files or textures. It is for demonstration with built-in loaded models.

# Build instructions
## Prerequisites
- C++20 compatible compiler
- Vulkan SDK & FBX SDK
- CMake

- git clone https://github.com/MustafaTolgaTokucu/TokucuEngine.git
- cd TokucuEngine
- mkdir build && cd build
- cmake ..

Textures should be downloaded in Sandbox/assets/, and assets folder should be copied in build/Sandbox/debug/ for renderer to work properly.

!!Vulkan SDK and FBX SDK should be manually linked. 




