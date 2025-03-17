#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#include "Car.h"
#include "Drone.h"
#include "TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_TREES 10
#define NUM_SPHERES 5

// pre-computed random values for tree foliage to prevent flickering
float treeRandSizes[MAX_TREES][NUM_SPHERES];

TextureManager textureManager;

// global texture IDs and rotation angles
GLuint grassTexture, horizonTexture, topTexture, roadTexture, apartmentTexture, apartmentTopTexture;
float rotationX = 0.0f, rotationY = 0.0f; // to control view rotation
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 0.0f; // to control camera position
float cameraSpeed = 1.0f;

// global variable to scale the environment
float cubeSize = 60.0f;

// lighting globals
#define MAX_LIGHTS 8
GLfloat lightPositions[MAX_LIGHTS][4];
GLfloat lightColors[MAX_LIGHTS][4];
bool lightsEnabled = true;
GLuint shadowMapTexture;
GLuint shadowMapFBO;                   // framebuffer for shadow map
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

void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // warm yellow for street lamps
    for (int i = 0; i < MAX_LIGHTS; i++) {
        lightColors[i][0] = 1.0f;
        lightColors[i][1] = 0.9f;
        lightColors[i][2] = 0.6f;
        lightColors[i][3] = 1.0f;
    }

    // set up global ambient light
    GLfloat globalAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
}

void initShadowMap() {
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

void init() {
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

    // random foliage sizes for each tree
    for (int t = 0; t < MAX_TREES; t++) {
        for (int s = 0; s < NUM_SPHERES; s++) {
            treeRandSizes[t][s] = 0.85f + (float)(rand() % 30) / 100.0f;
        }
    }

    playerCar = Car(cubeSize);

    drones[0] = new Drone(0.0f, 0.0f, 0.0f, 3.0f); 
    drones[1] = new Drone(-30.0f, -cubeSize + 30.0f, -25.0f, 4.5f);

    lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void drawWorld() {
    glDisable(GL_CULL_FACE); // ensure interior faces are visible

    // floor
    textureManager.bindTexture("grass");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, cubeSize);
    glEnd();

    // ceiling
    textureManager.bindTexture("sky");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, cubeSize, cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, cubeSize, -cubeSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, cubeSize, -cubeSize);
    glEnd();

    // front face (z = cubeSize)
    textureManager.bindTexture("horizon");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(cubeSize, cubeSize, cubeSize);
    glEnd();

    // right face (x = cubeSize)
    textureManager.bindTexture("horizon");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, cubeSize, cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(cubeSize, cubeSize, -cubeSize);
    glEnd();

    // back face (z = -cubeSize)
    textureManager.bindTexture("horizon");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, cubeSize, -cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, -cubeSize);
    glEnd();

    // left face (x = -cubeSize)
    textureManager.bindTexture("horizon");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, -cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, cubeSize);
    glEnd();
}

void drawTexturedApartmentCube() {
    // set material properties for better lighting
    GLfloat buildingMaterial[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, buildingMaterial);

    textureManager.bindTexture("apartment");
    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glEnd();

    textureManager.bindTexture("apartmentTop");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    textureManager.bindTexture("apartmentTop");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glEnd();
}

// posX and posZ determine the center of the block's base on the cube's floor
void drawApartment(float posX, float posZ, float width, float depth, float height) {
    glPushMatrix();
    glTranslatef(posX, -cubeSize + height / 2.0f, posZ);
    glScalef(width, height, depth);
    drawTexturedApartmentCube();
    glPopMatrix();
}

void drawTree(float posX, float posZ, float trunkRadius, float trunkHeight, float foliageRadius) {
    float baseY = -cubeSize; // base of the tree at ground level

    // draw the trunk of the tree
    glPushMatrix();
    GLfloat trunkMaterial[] = {0.45f, 0.25f, 0.05f, 1.0f}; // brown
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, trunkMaterial);
    glColor3f(0.45f, 0.25f, 0.05f);

    glTranslatef(posX, baseY, posZ);
    const int SLICES = 10;
    const int STACKS = 8;

    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadric, trunkRadius, trunkRadius * 0.8, trunkHeight, SLICES, STACKS);
    gluDeleteQuadric(quadric);
    glPopMatrix();

    // draw foliage as multiple overlapping spheres with slight variations for the crown of the tree
    GLfloat foliageColors[][3] = {
        {0.0f, 0.6f, 0.0f},
        {0.1f, 0.7f, 0.1f},
        {0.2f, 0.8f, 0.2f}
    };

    
    int treeIndex = ((int)posX + (int)posZ) % MAX_TREES;
    if (treeIndex < 0) treeIndex += MAX_TREES;

    float sphereOffsets[NUM_SPHERES][3] = {
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.5f, 0.2f},
        {-0.8f, -0.3f, 0.5f},
        {0.3f, 0.7f, -0.8f},
        {-0.5f, 0.4f, -1.0f}
    };

    for (int i = 0; i < NUM_SPHERES; i++) {
        glPushMatrix();
        int colorIndex = i % 3;
        GLfloat foliageMaterial[] = {
            foliageColors[colorIndex][0],
            foliageColors[colorIndex][1],
            foliageColors[colorIndex][2],
            1.0f
        };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, foliageMaterial);
        glColor3fv(foliageColors[colorIndex]);

        float sphereSize = foliageRadius * treeRandSizes[treeIndex][i];

        glTranslatef(
            posX + sphereOffsets[i][0] * (foliageRadius * 0.4f),
            baseY + trunkHeight + foliageRadius * 0.8f + sphereOffsets[i][1] * (foliageRadius * 0.3f),
            posZ + sphereOffsets[i][2] * (foliageRadius * 0.4f)
        );
        glutSolidSphere(sphereSize, 12, 12);
        glPopMatrix();
    }

    GLfloat defaultMaterial[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, defaultMaterial);
    glColor3f(1.0f, 1.0f, 1.0f);
}

// draw the street circuit on the floor as a rectangular ring road
void drawStreetCircuit() {
    textureManager.bindTexture("road");
    float y = -cubeSize + 0.5f; // slightly above the floor level

    // north side of road
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize * 0.8f, y, cubeSize * 0.3f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize * 0.8f, y, cubeSize * 0.3f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize * 0.8f, y, cubeSize * 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize * 0.8f, y, cubeSize * 0.5f);
    glEnd();

    // south side of road
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize * 0.8f, y, -cubeSize * 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize * 0.8f, y, -cubeSize * 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize * 0.8f, y, -cubeSize * 0.3f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize * 0.8f, y, -cubeSize * 0.3f);
    glEnd();

    // west side of road
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize * 0.8f, y, -cubeSize * 0.3f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize * 0.6f, y, -cubeSize * 0.3f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubeSize * 0.6f, y, cubeSize * 0.3f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubeSize * 0.8f, y, cubeSize * 0.3f);
    glEnd();

    // east side of road
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(cubeSize * 0.6f, y, -cubeSize * 0.3f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(cubeSize * 0.8f, y, -cubeSize * 0.3f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize * 0.8f, y, cubeSize * 0.3f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize * 0.6f, y, cubeSize * 0.3f);
    glEnd();
}

void drawStreetLamp(float posX, float posZ, float height, int lightIndex) {
    float baseY = -cubeSize + 0.5f;
    float poleRadius = 0.5f;
    float lampRadius = 1.0f;

    // set material properties for the pole
    GLfloat poleMaterial[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, poleMaterial);

    // lamp pole
    glPushMatrix();
    glColor3f(0.2f, 0.2f, 0.2f); // dark gray
    glTranslatef(posX, baseY + height/2.0f, posZ);
    glScalef(poleRadius, height, poleRadius);
    glutSolidCube(1.0);
    glPopMatrix();

    // lamp fixture
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f); // light gray
    glTranslatef(posX, baseY + height, posZ);
    glScalef(lampRadius*1.5f, lampRadius/2.0f, lampRadius*1.5f);
    glutSolidCube(1.0);
    glPopMatrix();

    // set light position
    if (lightIndex < MAX_LIGHTS) {
        lightPositions[lightIndex][0] = posX;
        lightPositions[lightIndex][1] = baseY + height;
        lightPositions[lightIndex][2] = posZ;
        lightPositions[lightIndex][3] = 1.0f;

        // enable with pre-configured color
        GLenum lightEnum = GL_LIGHT0 + lightIndex;
        glEnable(lightEnum);
        glLightfv(lightEnum, GL_POSITION, lightPositions[lightIndex]);
        glLightfv(lightEnum, GL_DIFFUSE, lightColors[lightIndex]);

        // enable attenuation for more realistic lighting
        glLightf(lightEnum, GL_CONSTANT_ATTENUATION, 0.5f);
        glLightf(lightEnum, GL_LINEAR_ATTENUATION, 0.01f);
        glLightf(lightEnum, GL_QUADRATIC_ATTENUATION, 0.001f);

        // draw a small light source indicator (when lights are enabled)
        if (lightsEnabled) {
            glPushMatrix();
            glDisable(GL_LIGHTING); // sisable lighting for the light source
            glColor3fv(lightColors[lightIndex]);
            glTranslatef(posX, baseY + height, posZ);
            glutSolidSphere(0.5f, 8, 8);
            glEnable(GL_LIGHTING);
            glPopMatrix();
        }
    }

    glColor3f(1.0f, 1.0f, 1.0f);
}

// place street lamps along the road circuit
void drawStreetLamps() {
    float lampHeight = 12.0f;
    int lampIndex = 0;

    float northZ = cubeSize * 0.55f;  // outside the north edge
    drawStreetLamp(-cubeSize * 0.6f, northZ, lampHeight, lampIndex++);
    drawStreetLamp(-cubeSize * 0.2f, northZ, lampHeight, lampIndex++);
    drawStreetLamp(cubeSize * 0.2f, northZ, lampHeight, lampIndex++);
    drawStreetLamp(cubeSize * 0.6f, northZ, lampHeight, lampIndex++);

    float southZ = -cubeSize * 0.55f;  // outside the south edge
    drawStreetLamp(-cubeSize * 0.6f, southZ, lampHeight, lampIndex++);
    drawStreetLamp(cubeSize * 0.6f, southZ, lampHeight, lampIndex++);

    float westX = -cubeSize * 0.85f; // outside the west edge
    drawStreetLamp(westX, 0.0f, lampHeight, lampIndex++);

    float eastX = cubeSize * 0.85f;  // outside the east edge
    drawStreetLamp(eastX, 0.0f, lampHeight, lampIndex++);

    for (int i = lampIndex; i < MAX_LIGHTS; i++) {
        glDisable(GL_LIGHT0 + i);
    }
}

// apartments and trees are placed on the floor and outside the road circuit
void drawStaticObjects() {
    drawApartment(-50.0f, 40.0f, 12.0f, 12.0f, 40.0f);  // top left
    drawApartment(50.0f, 40.0f, 12.0f, 12.0f, 40.0f);  // top right
    drawApartment(-50.0f, -40.0f, 12.0f, 12.0f, 40.0f);  // bottom left
    drawApartment(50.0f, -40.0f, 12.0f, 12.0f, 40.0f);  // bottom right
    drawApartment(0.0f, 40.0f, 12.0f, 12.0f, 40.0f);  // top center
    drawApartment(0.0f, -40.0f, 12.0f, 12.0f, 40.0f);  // bottom center

    drawTree(-55.0f, 0.0f, 2.0f, 10.0f, 5.0f);  //left side
    drawTree(55.0f, 0.0f, 2.0f, 10.0f, 5.0f);  // right side
    drawTree(20.0f, 55.0f, 2.0f, 10.0f, 5.0f);  // top side 
    drawTree(20.0f, -55.0f, 2.0f, 10.0f, 5.0f);  // bottom side 
}

void display() {
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

    if (lightsEnabled) {
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }

    drawWorld();
    drawStreetCircuit();
    drawStaticObjects();
    drawStreetLamps();

    for (int i = 0; i < 2; i++) {
        drones[i]->draw();
    }

    playerCar.draw();

    glutSwapBuffers();
}

void updateCar() {
    playerCar.update(carMovingForward, carMovingBackward, carTurningLeft, carTurningRight, cubeSize);
    carCollision = (playerCar.speed < 0); 
}

// method that handles continously events
void idle() {
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

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 0.1, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

void specialKeys(int key, int x, int y) {
    const float angleStep = 5.0f;
    switch (key) {
    case GLUT_KEY_UP:    rotationX += angleStep; break;
    case GLUT_KEY_DOWN:  rotationX -= angleStep; break;
    case GLUT_KEY_LEFT:  rotationY -= angleStep; break;
    case GLUT_KEY_RIGHT: rotationY += angleStep; break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
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
            playerCar.posX += carFreeSpeed * sin(carAngleRad);
            playerCar.posZ -= carFreeSpeed * cos(carAngleRad);
            break;
        case 'k':  
            playerCar.posX -= carFreeSpeed * sin(carAngleRad);
            playerCar.posZ += carFreeSpeed * cos(carAngleRad);
            break;
        case 'u':  
            playerCar.posX -= carFreeSpeed * cos(carAngleRad);
            playerCar.posZ -= carFreeSpeed * sin(carAngleRad);
            break;
        case 'o': 
            playerCar.posX += carFreeSpeed * cos(carAngleRad);
            playerCar.posZ += carFreeSpeed * sin(carAngleRad);
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
            lightsEnabled = !lightsEnabled;
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
    
    float carBoundary = cubeSize * 0.9f;
    if (playerCar.posX > carBoundary) playerCar.posX = carBoundary;
    if (playerCar.posX < -carBoundary) playerCar.posX = -carBoundary;
    if (playerCar.posY > carBoundary) playerCar.posY = carBoundary;
    if (playerCar.posY < -carBoundary) playerCar.posY = -carBoundary;
    if (playerCar.posZ > carBoundary) playerCar.posZ = carBoundary;
    if (playerCar.posZ < -carBoundary) playerCar.posZ = -carBoundary;
    
    float boundary = cubeSize * 0.9f;
    if (cameraX > boundary) cameraX = boundary;
    if (cameraX < -boundary) cameraX = -boundary;
    if (cameraY > boundary) cameraY = boundary;
    if (cameraY < -boundary) cameraY = -boundary;
    if (cameraZ > boundary) cameraZ = boundary;
    if (cameraZ < -boundary) cameraZ = -boundary;

    glutPostRedisplay();
}

// handle key release events
void keyboardUp(unsigned char key, int x, int y) {
    if (!freeMovementMode) {
        switch (key) {
            case 'u':
                carMovingBackward = false;
                break;
            case 'h':
                carTurningLeft = false;
                break;
            case 'j':
                carTurningRight = false;
                break;
        }
    }
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
    glutKeyboardUpFunc(keyboardUp);  
    glutIdleFunc(idle);              
    glutMainLoop();

    for (int i = 0; i < 2; i++) {
        delete drones[i];
    }

    return 0;
}
