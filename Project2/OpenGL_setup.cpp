#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cstdlib>
#include <cmath>

#include "World.h"
#include "Car.h"
#include "Drone.h"
#include "TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureManager textureManager;
float rotationX = 0.0f, rotationY = 0.0f; // to control view rotation
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 0.0f; // to control camera position
float cameraSpeed = 1.0f;

// global variable to scale the environment
float cubeSize = 60.0f;

World world(cubeSize, &textureManager);

// lighting globals
bool lightsEnabled = true;
GLuint shadowMapTexture;
GLuint shadowMapFBO; // framebuffer for shadow map
const int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

// car movement flags
bool carMovingForward = false;
bool carMovingBackward = false;
bool carTurningLeft = false;
bool carTurningRight = false;
bool carCollision = false;

// global variables for camera modes
bool thirdPersonView = false;
float thirdPersonDistance = 15.0f;
float thirdPersonHeight = 7.0f;
bool freeMovementMode = false;  // toggle between realistic and free movement

Car playerCar;
float carFreeSpeed = 0.8f; // speed for free movement mode

Drone* drones[2];
float lastTime = 0; // timing for drones' animations

void static initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // set up global ambient light
    GLfloat globalAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
}

void static initShadowMap() {
    glGenFramebuffersEXT(1, &shadowMapFBO);

    // generate texture for shadow map
    glGenTextures(1, &shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);

    // configure texture parameters
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // attach texture to framebuffer's depth attachment
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowMapFBO);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
        GL_TEXTURE_2D, shadowMapTexture, 0);

    // disable color buffer as we only need depth
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT) {
        std::cerr << "Shadow framebuffer is not complete!" << std::endl;
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void static init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    textureManager.loadTexture("grass.jpg", "grass");
    textureManager.loadTexture("lateral.jpeg", "horizon");
    textureManager.loadTexture("sky.jpeg", "sky");
    textureManager.loadTexture("road.jpg", "road");
    textureManager.loadTexture("apartment.jpg", "apartment");
    textureManager.loadTexture("grey.jpg", "apartmentTop");

    initLighting();

    // shadow mapping is available only if this extension exists on the system
    if (glewIsSupported("GL_EXT_framebuffer_object")) {
        initShadowMap();
    } else {
        std::cout << "Shadow mapping not supported on this system." << std::endl;
    }

    // initialize the car at a proper starting position
    playerCar = Car(cubeSize);

    drones[0] = new Drone(0.0f, 0.0f, 0.0f, 3.0f);
    drones[1] = new Drone(-30.0f, -cubeSize + 30.0f, -25.0f, 4.5f);

    lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void static display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (thirdPersonView) {
        float angleRad = playerCar.rotationY * 3.14159f / 180.0f;
        float camX = playerCar.posX - sin(angleRad) * thirdPersonDistance;
        float camY = playerCar.posY + thirdPersonHeight;
        float camZ = playerCar.posZ + cos(angleRad) * thirdPersonDistance;

        gluLookAt(camX, camY, camZ,
            playerCar.posX, playerCar.posY + 2.0f, playerCar.posZ,
            0.0f, 1.0f, 0.0f);
    }
    else {
        float angleX = rotationX * 3.14159f / 180.0f;
        float angleY = rotationY * 3.14159f / 180.0f;

        float lookX = sin(angleY) * cos(angleX);
        float lookY = sin(angleX);
        float lookZ = -cos(angleY) * cos(angleX);

        gluLookAt(cameraX, cameraY, cameraZ,
            cameraX + lookX, cameraY + lookY, cameraZ + lookZ,
            0.0f, 1.0f, 0.0f);
    }

    world.drawAll();

    for (int i = 0; i < 2; i++) {
        drones[i]->draw();
    }

    playerCar.draw();

    glutSwapBuffers();
}

void static updateCar() {
    playerCar.update(carMovingForward, carMovingBackward, carTurningLeft, carTurningRight, cubeSize);
    carCollision = (playerCar.speed < 0);
}

void static idle() {
    // delta time for smooth animation
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    updateCar();

    for (int i = 0; i < 2; i++) {
        drones[i]->update(deltaTime);
    }

    glutPostRedisplay();
}

void static reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 0.1, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

void static specialKeys(int key, int x, int y) {
    const float angleStep = 5.0f;
    switch (key) {
    case GLUT_KEY_UP:    rotationX += angleStep; break;
    case GLUT_KEY_DOWN:  rotationX -= angleStep; break;
    case GLUT_KEY_LEFT:  rotationY -= angleStep; break;
    case GLUT_KEY_RIGHT: rotationY += angleStep; break;
    }
    glutPostRedisplay();
}

float static clampValue(float value, float minVal, float maxVal) {
    return (value < minVal) ? minVal : (value > maxVal ? maxVal : value);
}

void static keyboard(unsigned char key, int x, int y) {
    float angleY = rotationY * 3.14159f / 180.0f; // camera angle in radians
    float carAngleRad = playerCar.rotationY * 3.14159f / 180.0f; // car rotation in radians

    // store old car position to revert back in case of collision
    float oldCarX = playerCar.posX;
    float oldCarZ = playerCar.posZ;
    float oldCarY = playerCar.posY;

    switch (key) {
    case 'w':
        cameraX += cameraSpeed * sin(angleY);
        cameraZ -= cameraSpeed * cos(angleY);
        break;
    case 's':
        cameraX -= cameraSpeed * sin(angleY);
        cameraZ += cameraSpeed * cos(angleY);
        break;
    case 'a':
        cameraX -= cameraSpeed * cos(angleY);
        cameraZ -= cameraSpeed * sin(angleY);
        break;
    case 'd':
        cameraX += cameraSpeed * cos(angleY);
        cameraZ += cameraSpeed * sin(angleY);
        break;
    case ' ':
        cameraY += cameraSpeed;
        break;
    case 'c':
        cameraY -= cameraSpeed;
        break;
    case 'i':
        playerCar.posX += 0.8f * sin(carAngleRad);
        playerCar.posZ -= 0.8f * cos(carAngleRad);
        break;
    case 'k':
        playerCar.posX -= 0.8f * sin(carAngleRad);
        playerCar.posZ += 0.8f * cos(carAngleRad);
        break;
    case 'u':
        playerCar.posX -= 0.8f * cos(carAngleRad);
        playerCar.posZ -= 0.8f * sin(carAngleRad);
        break;
    case 'o':
        playerCar.posX += 0.8f * cos(carAngleRad);
        playerCar.posZ += 0.8f * sin(carAngleRad);
        break;
    case 'j':
        playerCar.rotationY += 5.0f;
        while (playerCar.rotationY >= 360.0f) playerCar.rotationY -= 360.0f;
        break;
    case 'l':
        playerCar.rotationY -= 5.0f;
        while (playerCar.rotationY < 0.0f) playerCar.rotationY += 360.0f;
        break;
    case 'r':
        cameraX = cameraY = cameraZ = 0.0f;
        rotationX = rotationY = 0.0f;
        break;
    case 'm':
        playerCar.posX = 0.0f;
        playerCar.posY = -cubeSize + 1.0f;
        playerCar.posZ = 0.0f;
        playerCar.rotationY = 0.0f;
        break;
    case 'g':
        world.toggleLights(); 
        lightsEnabled = world.areLightsEnabled(); 
        break;
    case 't':
        thirdPersonView = !thirdPersonView;
        break;
    }

    if (key == 'i' || key == 'k' || key == 'u' || key == 'o') {
        if (playerCar.checkCollision(cubeSize)) {
            // revert to old position on collision
            playerCar.posX = oldCarX;
            playerCar.posY = oldCarY;
            playerCar.posZ = oldCarZ;
            carCollision = true;
        } else {
            carCollision = false;
        }
    }

    float boundary = cubeSize * 0.9f;
    playerCar.posX = clampValue(playerCar.posX, -boundary, boundary);
    playerCar.posY = clampValue(playerCar.posY, -boundary, boundary);
    playerCar.posZ = clampValue(playerCar.posZ, -boundary, boundary);
    cameraX = clampValue(cameraX, -boundary, boundary);
    cameraY = clampValue(cameraY, -boundary, boundary);
    cameraZ = clampValue(cameraZ, -boundary, boundary);

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Inside the Cube - Extended Space with Circuit & Objects");

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW\n";
        return 1;
    }

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();

    for (int i = 0; i < 2; i++) {
        delete drones[i];
    }

    return 0;
}
