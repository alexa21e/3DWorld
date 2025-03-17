#include "Car.h"
#include <cmath>

Car::Car(float worldSize) {
    // Initialize car position at ground level
    posY = -worldSize + 1.0f;
}

void Car::draw() {
    glPushMatrix();

    // Position and rotation
    glTranslatef(posX, posY, posZ);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

    // Set material properties for car body
    GLfloat carBodyMaterial[] = { 0.1f, 0.3f, 0.7f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, carBodyMaterial);
    GLfloat carSpecular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, carSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, 80.0f);

    // Main body - lower part
    glPushMatrix();
    glScalef(width * 0.9f, height * 0.4f, length * 0.9f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Top cabin - with a slope
    glPushMatrix();
    glTranslatef(0.0f, height * 0.3f, -length * 0.05f);

    // Create a more aerodynamic cabin shape
    glBegin(GL_QUADS);
    // Left side
    glVertex3f(-width * 0.4f, 0.0f, length * 0.3f);
    glVertex3f(-width * 0.4f, 0.0f, -length * 0.2f);
    glVertex3f(-width * 0.4f, height * 0.3f, -length * 0.25f);
    glVertex3f(-width * 0.4f, height * 0.3f, length * 0.2f);

    // Right side
    glVertex3f(width * 0.4f, 0.0f, length * 0.3f);
    glVertex3f(width * 0.4f, 0.0f, -length * 0.2f);
    glVertex3f(width * 0.4f, height * 0.3f, -length * 0.25f);
    glVertex3f(width * 0.4f, height * 0.3f, length * 0.2f);

    // Front windshield
    glVertex3f(-width * 0.4f, 0.0f, length * 0.3f);
    glVertex3f(width * 0.4f, 0.0f, length * 0.3f);
    glVertex3f(width * 0.4f, height * 0.3f, length * 0.2f);
    glVertex3f(-width * 0.4f, height * 0.3f, length * 0.2f);

    // Rear windshield
    glVertex3f(-width * 0.4f, 0.0f, -length * 0.2f);
    glVertex3f(width * 0.4f, 0.0f, -length * 0.2f);
    glVertex3f(width * 0.4f, height * 0.3f, -length * 0.25f);
    glVertex3f(-width * 0.4f, height * 0.3f, -length * 0.25f);

    // Roof
    glVertex3f(-width * 0.4f, height * 0.3f, length * 0.2f);
    glVertex3f(width * 0.4f, height * 0.3f, length * 0.2f);
    glVertex3f(width * 0.4f, height * 0.3f, -length * 0.25f);
    glVertex3f(-width * 0.4f, height * 0.3f, -length * 0.25f);
    glEnd();
    glPopMatrix();

    // Front bumper
    glPushMatrix();
    glTranslatef(0.0f, -height * 0.1f, length * 0.45f);
    glScalef(width * 0.8f, height * 0.2f, length * 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Rear bumper
    glPushMatrix();
    glTranslatef(0.0f, -height * 0.1f, -length * 0.45f);
    glScalef(width * 0.8f, height * 0.2f, length * 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Windows
    GLfloat windowMaterial[] = { 0.05f, 0.05f, 0.05f, 0.7f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, windowMaterial);
    glMaterialfv(GL_FRONT, GL_SPECULAR, carSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, 120.0f);

    // Side windows
    glPushMatrix();
    glTranslatef(-width * 0.45f, height * 0.1f, 0.0f);
    glScalef(0.05f, height * 0.2f, length * 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(width * 0.45f, height * 0.1f, 0.0f);
    glScalef(0.05f, height * 0.2f, length * 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Wheels
    GLfloat wheelMaterial[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wheelMaterial);
    GLfloat wheelSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, wheelSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);

    // Draw the 4 wheels
    float wheelRadius = height * 0.3f;
    float wheelWidth = width * 0.15f;

    GLUquadricObj* wheelObj = gluNewQuadric();
    gluQuadricDrawStyle(wheelObj, GLU_FILL);
    gluQuadricNormals(wheelObj, GLU_SMOOTH);

    // Front left wheel
    glPushMatrix();
    glTranslatef(-width / 2.0f - wheelWidth / 2.0f, -height / 4.0f, length / 3.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluDisk(wheelObj, 0, wheelRadius, 16, 3);
    gluCylinder(wheelObj, wheelRadius, wheelRadius, wheelWidth, 16, 1);
    glTranslatef(0.0f, 0.0f, wheelWidth);
    gluDisk(wheelObj, 0, wheelRadius, 16, 3);
    glPopMatrix();

    // Front right wheel
    glPushMatrix();
    glTranslatef(width / 2.0f + wheelWidth / 2.0f, -height / 4.0f, length / 3.0f);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    gluDisk(wheelObj, 0, wheelRadius, 16, 3);
    gluCylinder(wheelObj, wheelRadius, wheelRadius, wheelWidth, 16, 1);
    glTranslatef(0.0f, 0.0f, wheelWidth);
    gluDisk(wheelObj, 0, wheelRadius, 16, 3);
    glPopMatrix();

    // Rear left wheel
    glPushMatrix();
    glTranslatef(-width / 2.0f - wheelWidth / 2.0f, -height / 4.0f, -length / 3.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluDisk(wheelObj, 0, wheelRadius, 16, 3);
    gluCylinder(wheelObj, wheelRadius, wheelRadius, wheelWidth, 16, 1);
    glTranslatef(0.0f, 0.0f, wheelWidth);
    gluDisk(wheelObj, 0, wheelRadius, 16, 3);
    glPopMatrix();

    // Rear right wheel
    glPushMatrix();
    glTranslatef(width / 2.0f + wheelWidth / 2.0f, -height / 4.0f, -length / 3.0f);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    gluDisk(wheelObj, 0, wheelRadius, 16, 3);
    gluCylinder(wheelObj, wheelRadius, wheelRadius, wheelWidth, 16, 1);
    glTranslatef(0.0f, 0.0f, wheelWidth);
    gluDisk(wheelObj, 0, wheelRadius, 16, 3);
    glPopMatrix();

    gluDeleteQuadric(wheelObj);

    glPopMatrix();
}

void Car::update(bool movingForward, bool movingBackward,
    bool turningLeft, bool turningRight, float worldSize) {
    // Store old position to revert back in case of collision
    float oldX = posX;
    float oldZ = posZ;

    float angleRad = rotationY * 3.14159f / 180.0f;
    float dirX = sin(angleRad);
    float dirZ = -cos(angleRad);

    if (movingForward && !movingBackward) {
        speed += acceleration;
        if (speed > maxSpeed)
            speed = maxSpeed;
    }
    else if (movingBackward && !movingForward) {
        if (speed > 0) {
            speed -= deceleration;
        }
        else {
            speed -= acceleration;
            if (speed < -maxSpeed * 0.6f)
                speed = -maxSpeed * 0.6f;
        }
    }
    else {
        if (speed > 0) {
            speed -= friction;
            if (speed < 0) speed = 0;
        }
        else if (speed < 0) {
            speed += friction;
            if (speed > 0) speed = 0;
        }
    }

    if (fabs(speed) > 0.01f) {
        float turnMultiplier = fmin(fabs(speed) * turnFactor, 1.0f);

        if (turningLeft)
            rotationY += turnSpeed * turnMultiplier * (speed > 0 ? 1 : -1);
        if (turningRight)
            rotationY -= turnSpeed * turnMultiplier * (speed > 0 ? 1 : -1);
    }

    while (rotationY >= 360.0f) rotationY -= 360.0f;
    while (rotationY < 0.0f) rotationY += 360.0f;

    posX += dirX * speed;
    posZ += dirZ * speed;

    bool collision = checkCollision(worldSize);
    if (collision) {
        posX = oldX;
        posZ = oldZ;
        speed = -speed * 0.3f;  // Bounce back on collision
    }
}

bool Car::checkCollision(float worldSize) {
    // Collision with apartment buildings
    float apartmentPositions[][3] = {
        {-50.0f, -worldSize + 20.0f, 40.0f},   // top left
        {50.0f, -worldSize + 20.0f, 40.0f},    // top right
        {-50.0f, -worldSize + 20.0f, -40.0f},  // bottom left
        {50.0f, -worldSize + 20.0f, -40.0f},   // bottom right
        {0.0f, -worldSize + 20.0f, 40.0f},     // top center
        {0.0f, -worldSize + 20.0f, -40.0f}     // bottom center
    };

    for (int i = 0; i < 6; i++) {
        if (checkAABBCollision(
            posX, posY, posZ, width, height, length,
            apartmentPositions[i][0], apartmentPositions[i][1], apartmentPositions[i][2], 12.0f, 40.0f, 12.0f
        )) {
            return true;
        }
    }

    // Collision with trees
    float treePositions[][3] = {
        {-55.0f, -worldSize + 5.0f, 0.0f},
        {55.0f, -worldSize + 5.0f, 0.0f},
        {20.0f, -worldSize + 5.0f, 55.0f},
        {20.0f, -worldSize + 5.0f, -55.0f}
    };

    for (int i = 0; i < 4; i++) {
        if (checkAABBCollision(
            posX, posY, posZ, width, height, length,
            treePositions[i][0], treePositions[i][1], treePositions[i][2], 10.0f, 15.0f, 10.0f
        )) {
            return true;
        }
    }

    // Collision with street lamps
    float lampPositions[][3] = {
        {-worldSize * 0.6f, -worldSize + 6.0f, worldSize * 0.55f},
        {-worldSize * 0.2f, -worldSize + 6.0f, worldSize * 0.55f},
        {worldSize * 0.2f, -worldSize + 6.0f, worldSize * 0.55f},
        {worldSize * 0.6f, -worldSize + 6.0f, worldSize * 0.55f},
        {-worldSize * 0.6f, -worldSize + 6.0f, -worldSize * 0.55f},
        {worldSize * 0.6f, -worldSize + 6.0f, -worldSize * 0.55f},
        {-worldSize * 0.85f, -worldSize + 6.0f, 0.0f},
        {worldSize * 0.85f, -worldSize + 6.0f, 0.0f}
    };

    for (int i = 0; i < 8; i++) {
        if (checkAABBCollision(
            posX, posY, posZ, width, height, length,
            lampPositions[i][0], lampPositions[i][1], lampPositions[i][2], 1.0f, 12.0f, 1.0f
        )) {
            return true;
        }
    }

    // Boundaries to keep car inside the cube
    if (posX - width / 2.0f < -worldSize || posX + width / 2.0f > worldSize ||
        posZ - length / 2.0f < -worldSize || posZ + length / 2.0f > worldSize) {
        return true;
    }

    return false;
}

bool Car::checkAABBCollision(
    float x1, float y1, float z1, float w1, float h1, float l1,
    float x2, float y2, float z2, float w2, float h2, float l2
) {
    float angle = rotationY * 3.14159f / 180.0f;
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    float rotatedWidth = fabs(cosAngle * w1) + fabs(sinAngle * l1);
    float rotatedLength = fabs(sinAngle * w1) + fabs(cosAngle * l1);

    // Check overlap in all three dimensions
    return (
        fabs(x1 - x2) < (rotatedWidth / 2.0f + w2 / 2.0f) &&
        fabs(y1 - y2) < (h1 / 2.0f + h2 / 2.0f) &&
        fabs(z1 - z2) < (rotatedLength / 2.0f + l2 / 2.0f)
        );
}