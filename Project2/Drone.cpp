#include "Drone.h"
#include <GL/glut.h>

Drone::Drone(float x, float y, float z, float objectSize) {
    posX = x;
    posY = y;
    posZ = z;
    rotationY = 0.0f;
    speed = 0.3f;
    size = objectSize;
    time = 0.0f;

    centerX = x;
    centerZ = z;
    pathRadius = 20.0f;
}

void Drone::update(float deltaTime) {
    time += deltaTime;

    
        float t = time * 0.5f;
        posX = centerX + pathRadius * sin(t);
        posZ = centerZ + pathRadius * 0.5f * sin(2 * t);

        // drone hovers up and down slightly
        posY = -cubeSize + 20.0f + 5.0f * sin(time * 0.3f);

        // update rotation to face movement direction
        float nextX = centerX + pathRadius * sin(t + 0.1f);
        float nextZ = centerZ + pathRadius * 0.5f * sin(2 * (t + 0.1f));
        rotationY = atan2(nextX - posX, posZ - nextZ) * 180.0f / 3.14159f;
}

void Drone::draw() {
    glPushMatrix();
    glTranslatef(posX, posY, posZ);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

    
        glColor3f(0.6f, 0.6f, 0.7f);
        glutSolidSphere(size * 0.5f, 16, 16);

        // arms for rotors
        glPushMatrix();
        glColor3f(0.3f, 0.3f, 0.4f);
        for (int i = 0; i < 4; i++) {
            glPushMatrix();
            glRotatef(i * 90.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(size * 0.8f, 0.0f, 0.0f);

            // arm
            glPushMatrix();
            glScalef(size * 0.8f, size * 0.1f, size * 0.1f);
            glutSolidCube(1.0f);
            glPopMatrix();

            // rotor
            glColor3f(0.2f, 0.2f, 0.2f);
            glTranslatef(0.0f, size * 0.1f, 0.0f);
            glutSolidTorus(size * 0.05f, size * 0.3f, 8, 16);

            // rotors' animation
            glRotatef(time * 500.0f, 0.0f, 1.0f, 0.0f);
            glColor3f(0.7f, 0.7f, 0.7f);
            glBegin(GL_LINES);
            for (int j = 0; j < 4; j++) {
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(cos(j * 1.57f) * size * 0.4f, 0.0f, sin(j * 1.57f) * size * 0.4f);
            }
            glEnd();
            glColor3f(0.3f, 0.3f, 0.4f);

            glPopMatrix();
        }
        glPopMatrix();
    
    glColor3f(1.0f, 1.0f, 1.0f); 
    glPopMatrix();
}