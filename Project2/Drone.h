#ifndef DRONE_H
#define DRONE_H

#include <cmath>

extern float cubeSize;

class Drone {
public:
    float posX, posY, posZ;
    float rotationY;
    float speed;
    float size;
    float time; 

    float centerX, centerZ;
    float pathRadius;

    Drone(float x, float y, float z, float size);

    void update(float deltaTime);

    void draw();
};

#endif // DRONE_H