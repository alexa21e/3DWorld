#ifndef CAR_H
#define CAR_H

#include <GL/glew.h>
#include <GL/freeglut.h>

class Car {
public:
    // Constructor
    Car(float worldSize = 60.0f);

    // Position and rotation properties
    float posX = 0.0f;
    float posY = 0.0f;  // Will be set in constructor
    float posZ = 0.0f;
    float rotationY = 0.0f;
    
    // Physics properties
    float speed = 0.0f;
    float maxSpeed = 0.6f;   
    float acceleration = 0.02f; 
    float deceleration = 0.04f;
    float friction = 0.005f;    
    float turnSpeed = 2.0f;   
    float turnFactor = 0.6f;   

    // Dimensions
    float width = 4.0f;
    float height = 2.0f;
    float length = 8.0f;

    // Methods
    void draw() const;
    void update(bool movingForward, bool movingBackward, 
                bool turningLeft, bool turningRight, float worldSize);
    bool checkCollision(float worldSize);

private:
    bool checkAABBCollision(
        float x1, float y1, float z1, float w1, float h1, float l1,
        float x2, float y2, float z2, float w2, float h2, float l2) const;
};

#endif // CAR_H