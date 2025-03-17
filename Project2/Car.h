#ifndef CAR_H
#define CAR_H

extern float cubeSize;

struct Car {
    float posX = 0.0f;
    float posY = -cubeSize + 1.0f;
    float posZ = 0.0f;
    float rotationY = 0.0f;
    float speed = 0.0f;
    float maxSpeed = 0.6f;   
    float acceleration = 0.02f; 
    float deceleration = 0.04f;
    float friction = 0.005f;    
    float turnSpeed = 2.0f;   
    float turnFactor = 0.6f;   

    float width = 4.0f;
    float height = 2.0f;
    float length = 8.0f;
};

#endif // CAR_H