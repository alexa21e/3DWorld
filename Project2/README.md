# OpenGL Environment Project

This project is a 3D virtual environment created with OpenGL/GLUT that simulates a small city circuit with interactive elements. The environment is enclosed within a cube with textured walls, featuring a drivable car, autonomous drones, buildings, trees, and street lamps with dynamic lighting.

## Features

- **Environment/**
  - Textured environment (sky, grass, road, building textures)
  - A street circuit
  - Urban elements including buildings, trees, and street lamps
  - Day/night lighting system with multiple light sources

- **Interactive Objects/**
  - Drivable car
  - Collision detection system to prevent passing through objects
  - Autonomous drones that navigate the environment

- **Camera Systems/**
  - Third-person view following the car
  - Free camera movement for exploring the environment
  - Dynamic camera controls for different perspectives

- **Visual Effects/**
  - Shadow mapping for realistic lighting
  - Material properties for objects
  - Realistic lighting attenuation

## Controls

1. **Car Controls**
    I: Drive forward
    K: Drive backward
    J: Turn left
    L: Turn right
    U/O: Lateral movement (strafe)
    M: Reset car position

2. **Camera Controls**
    Space: Move camera up
    C: Move camera down
    R: Reset camera position
    T: Toggle between third-person and free camera view

3. **Environment Controls**
    G: Toggle lights for day/night mode