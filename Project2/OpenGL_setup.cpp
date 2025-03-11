#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Global texture IDs and rotation angles
GLuint grassTexture, horizonTexture, topTexture, roadTexture, apartmentTexture, apartmentTopTexture;
float rotationX = 0.0f, rotationY = 0.0f; // to control view orientation

// Global variable to scale the environment
float cubeSize = 60.0f;

// Texture loading method
GLuint loadTexture(const char* filename) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << "\n";
        std::cerr << "stb_image error: " << stbi_failure_reason() << "\n";
        return 0;
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    grassTexture = loadTexture("grass.jpg");         
    horizonTexture = loadTexture("lateral.jpeg");    
    topTexture = loadTexture("sky.jpeg");            
    roadTexture = loadTexture("road.jpg");         
    apartmentTexture = loadTexture("apartment.jpg");
    apartmentTopTexture = loadTexture("grey.jpg");  

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// Draw world method
void drawCube() {
    glDisable(GL_CULL_FACE); // Ensure interior faces are visible

    // Floor
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, cubeSize);
    glEnd();

    // Ceiling
    glBindTexture(GL_TEXTURE_2D, topTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, cubeSize, cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, cubeSize, -cubeSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, cubeSize, -cubeSize);
    glEnd();

    // Front face (z = cubeSize)
    glBindTexture(GL_TEXTURE_2D, horizonTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(cubeSize, cubeSize, cubeSize);
    glEnd();

    // Right face (x = cubeSize)
    glBindTexture(GL_TEXTURE_2D, horizonTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, cubeSize, cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(cubeSize, cubeSize, -cubeSize);
    glEnd();

    // Back face (z = -cubeSize)
    glBindTexture(GL_TEXTURE_2D, horizonTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize, cubeSize, -cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, -cubeSize);
    glEnd();

    // Left face (x = -cubeSize)
    glBindTexture(GL_TEXTURE_2D, horizonTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, cubeSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubeSize, -cubeSize, -cubeSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, -cubeSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize, cubeSize, cubeSize);
    glEnd();
}

// Draw a textured unit cube for apartment blocks.
void drawTexturedApartmentCube() {
    glBindTexture(GL_TEXTURE_2D, apartmentTexture);
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

    glBindTexture(GL_TEXTURE_2D, apartmentTopTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, apartmentTexture);
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
    // Draw trunk
    glPushMatrix();
    glColor3f(0.55f, 0.27f, 0.07f); // Brown
    glTranslatef(posX, -cubeSize + trunkHeight / 2.0f, posZ);
    glScalef(trunkRadius * 2, trunkHeight, trunkRadius * 2);
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw foliage
    glPushMatrix();
    glColor3f(0.0f, 0.8f, 0.0f); // Green
    glTranslatef(posX, -cubeSize + trunkHeight + foliageRadius, posZ);
    glutSolidSphere(foliageRadius, 16, 16);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
}

// Draw the street circuit on the floor as a rectangular ring road
void drawStreetCircuit() {
    glBindTexture(GL_TEXTURE_2D, roadTexture);
    float y = -cubeSize + 0.5f; // Slightly above the floor level

    // North side of road
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize * 0.8f, y, cubeSize * 0.3f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize * 0.8f, y, cubeSize * 0.3f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize * 0.8f, y, cubeSize * 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize * 0.8f, y, cubeSize * 0.5f);
    glEnd();

    // South side of road
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize * 0.8f, y, -cubeSize * 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize * 0.8f, y, -cubeSize * 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize * 0.8f, y, -cubeSize * 0.3f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize * 0.8f, y, -cubeSize * 0.3f);
    glEnd();

    // West side of road
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubeSize * 0.8f, y, -cubeSize * 0.3f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeSize * 0.6f, y, -cubeSize * 0.3f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubeSize * 0.6f, y, cubeSize * 0.3f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubeSize * 0.8f, y, cubeSize * 0.3f);
    glEnd();

    // East side of road
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(cubeSize * 0.6f, y, -cubeSize * 0.3f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(cubeSize * 0.8f, y, -cubeSize * 0.3f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(cubeSize * 0.8f, y, cubeSize * 0.3f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(cubeSize * 0.6f, y, cubeSize * 0.3f);
    glEnd();
}

// Apartments and trees are placed on the floor and outside the road circuit
void drawStaticObjects() {
    drawApartment(-50.0f, 40.0f, 12.0f, 12.0f, 40.0f);  // Top left
    drawApartment(50.0f, 40.0f, 12.0f, 12.0f, 40.0f);  // Top right
    drawApartment(-50.0f, -40.0f, 12.0f, 12.0f, 40.0f);  // Bottom left
    drawApartment(50.0f, -40.0f, 12.0f, 12.0f, 40.0f);  // Bottom right
    drawApartment(0.0f, 40.0f, 12.0f, 12.0f, 40.0f);  // Top center
    drawApartment(0.0f, -40.0f, 12.0f, 12.0f, 40.0f);  // Bottom center

    drawTree(-55.0f, 0.0f, 2.0f, 10.0f, 5.0f);  // Left side
    drawTree(55.0f, 0.0f, 2.0f, 10.0f, 5.0f);  // Right side
    drawTree(20.0f, 55.0f, 2.0f, 10.0f, 5.0f);  // Top side 
    drawTree(20.0f, -55.0f, 2.0f, 10.0f, 5.0f);  // Bottom side 
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Place the camera at the center (inside the cube)
    gluLookAt(0.0, 0.0, 0.0,
        0.0, 0.0, -1.0,
        0.0, 1.0, 0.0);

    // Apply rotations based on arrow key input
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

    drawCube();
    drawStreetCircuit();
    drawStaticObjects();

    glutSwapBuffers();
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
    glutMainLoop();
    return 0;
}
