#ifndef WORLD_H
#define WORLD_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "TextureManager.h"

constexpr auto MAX_LIGHTS = 8;
constexpr auto MAX_TREES = 10;
constexpr auto NUM_SPHERES = 5;

class World {
public:
    World(float size, TextureManager* texManager);
    ~World();

    void drawWorld();
    void drawStreetCircuit();
    void drawStaticObjects();
    void drawStreetLamps();
    void drawAll();

    float getSize() const { return cubeSize; }
    void toggleLights() { lightsEnabled = !lightsEnabled; }
    bool areLightsEnabled() const { return lightsEnabled; }

private:
    float cubeSize;
    TextureManager* textureManager;
    float treeRandSizes[MAX_TREES][NUM_SPHERES];

    GLfloat lightPositions[MAX_LIGHTS][4];
    GLfloat lightColors[MAX_LIGHTS][4];
    bool lightsEnabled;

    void drawTexturedApartmentCube();
    void drawApartment(float posX, float posZ, float width, float depth, float height);
    void drawTree(float posX, float posZ, float trunkRadius, float trunkHeight, float foliageRadius) const;
    void drawStreetLamp(float posX, float posZ, float height, int lightIndex);

    void initTreeFoliage();
    void initLightColors();
};

#endif // WORLD_H
