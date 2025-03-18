#include "World.h"
#include <cmath>
#include <cstdlib>

World::World(float size, TextureManager* texManager) : cubeSize(size), textureManager(texManager), lightsEnabled(true) {
    initTreeFoliage();
    initLightColors();
}

World::~World() {
}

void World::initTreeFoliage() {
    // Initialize random foliage sizes for trees
    for (int t = 0; t < MAX_TREES; t++) {
        for (int s = 0; s < NUM_SPHERES; s++) {
            treeRandSizes[t][s] = 0.85f + (float)(rand() % 30) / 100.0f;
        }
    }
}

void World::initLightColors() {
    // Warm yellow color for street lamps
    for (int i = 0; i < MAX_LIGHTS; i++) {
        lightColors[i][0] = 1.0f;  // Red
        lightColors[i][1] = 0.9f;  // Green
        lightColors[i][2] = 0.6f;  // Blue
        lightColors[i][3] = 1.0f;  // Alpha
    }
}

void World::drawWorld() {
    glDisable(GL_CULL_FACE); // ensure interior faces are visible

    // floor
    textureManager->bindTexture("grass");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, cubeSize);
    glEnd();

    // ceiling
    textureManager->bindTexture("sky");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, cubeSize, cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, cubeSize, -cubeSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, cubeSize, -cubeSize);
    glEnd();

    // front face (z = cubeSize)
    textureManager->bindTexture("horizon");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(cubeSize, cubeSize, cubeSize);
    glEnd();

    // right face (x = cubeSize)
    textureManager->bindTexture("horizon");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, cubeSize, cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(cubeSize, cubeSize, -cubeSize);
    glEnd();

    // back face (z = -cubeSize)
    textureManager->bindTexture("horizon");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, cubeSize, -cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, -cubeSize);
    glEnd();

    // left face (x = -cubeSize)
    textureManager->bindTexture("horizon");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, -cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, cubeSize);
    glEnd();
}

void World::drawTexturedApartmentCube() {
    // set material properties for better lighting
    GLfloat buildingMaterial[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, buildingMaterial);

    textureManager->bindTexture("apartment");
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

    textureManager->bindTexture("apartmentTop");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    textureManager->bindTexture("apartmentTop");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glEnd();
}

void World::drawApartment(float posX, float posZ, float width, float depth, float height) {
    glPushMatrix();
    glTranslatef(posX, -cubeSize + height / 2.0f, posZ);
    glScalef(width, height, depth);
    drawTexturedApartmentCube();
    glPopMatrix();
}

void World::drawTree(float posX, float posZ, float trunkRadius, float trunkHeight, float foliageRadius) const {
    float baseY = -cubeSize; // base of the tree at ground level

    // draw the trunk of the tree
    glPushMatrix();
    GLfloat trunkMaterial[] = { 0.45f, 0.25f, 0.05f, 1.0f }; // brown
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

    GLfloat defaultMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, defaultMaterial);
    glColor3f(1.0f, 1.0f, 1.0f);
}

void World::drawStreetCircuit() {
    textureManager->bindTexture("road");
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

void World::drawStreetLamp(float posX, float posZ, float height, int lightIndex) {
    float baseY = -cubeSize + 0.5f;
    float poleRadius = 0.5f;
    float lampRadius = 1.0f;

    // set material properties for the pole
    GLfloat poleMaterial[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, poleMaterial);

    // lamp pole
    glPushMatrix();
    glColor3f(0.2f, 0.2f, 0.2f); // dark gray
    glTranslatef(posX, baseY + height / 2.0f, posZ);
    glScalef(poleRadius, height, poleRadius);
    glutSolidCube(1.0);
    glPopMatrix();

    // lamp fixture
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f); // light gray
    glTranslatef(posX, baseY + height, posZ);
    glScalef(lampRadius * 1.5f, lampRadius / 2.0f, lampRadius * 1.5f);
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

        if (lightsEnabled) {
            glEnable(lightEnum);
        }
        else {
            glDisable(lightEnum);
        }

        glLightfv(lightEnum, GL_POSITION, lightPositions[lightIndex]);
        glLightfv(lightEnum, GL_DIFFUSE, lightColors[lightIndex]);

        // enable attenuation for more realistic lighting
        glLightf(lightEnum, GL_CONSTANT_ATTENUATION, 0.5f);
        glLightf(lightEnum, GL_LINEAR_ATTENUATION, 0.01f);
        glLightf(lightEnum, GL_QUADRATIC_ATTENUATION, 0.001f);

        // draw a small light source indicator (when lights are enabled)
        if (lightsEnabled) {
            glPushMatrix();
            glDisable(GL_LIGHTING); // disable lighting for the light source
            glColor3fv(lightColors[lightIndex]);
            glTranslatef(posX, baseY + height, posZ);
            glutSolidSphere(0.5f, 8, 8);
            glEnable(GL_LIGHTING);
            glPopMatrix();
        }
    }

    glColor3f(1.0f, 1.0f, 1.0f);
}

void World::drawStreetLamps() {
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

void World::drawStaticObjects() {
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

void World::drawAll() {
    // Apply lighting state
    if (lightsEnabled) {
        glEnable(GL_LIGHTING);
    }
    else {
        glDisable(GL_LIGHTING);
    }

	drawWorld();
    drawStreetCircuit();
    drawStaticObjects();
    drawStreetLamps();
}
    