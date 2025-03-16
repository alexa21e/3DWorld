#ifndef CAR_H
#define CAR_H

// Forward declaration for cubeSize (since it's a global in the main file)
extern float cubeSize;

// Car properties
struct Car {
    float posX = 0.0f;
    float posY = -cubeSize + 1.0f;
    float posZ = 0.0f;
    float rotationY = 0.0f;
    float speed = 0.0f;
    float maxSpeed = 0.6f;   // Increased from 0.4f for better responsiveness
    float acceleration = 0.02f; // Increased from 0.01f for better acceleration
    float deceleration = 0.04f; // New parameter for better braking
    float friction = 0.005f;    // New parameter for more gradual slowing
    float turnSpeed = 2.0f;   // Increased for more responsive steering
    float turnFactor = 0.6f;   // New parameter to adjust turn radius based on speed

    // Collision boundaries
    float width = 4.0f;
    float height = 2.0f;
    float length = 8.0f;
};

#endif // CAR_H