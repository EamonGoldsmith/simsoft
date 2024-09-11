#include <GL/freeglut.h>
#include <GL/gl.h>
#include <iostream>

#include "gl_helper.h"

// Rotation angles for the cube
GLfloat angleX = 0.0f, angleY = 0.0f;

// Vertices for a cube
GLfloat vertices[] = {
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f,  0.5f, 0.5f,
	-0.5f,  0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  -0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f
};

// Cube indices
GLuint indices[] = {
    0, 1, 2, 2, 3, 0,  // Front face
    4, 5, 6, 6, 7, 4,  // Back face
    0, 1, 5, 5, 4, 0,  // Bottom face
    2, 3, 7, 7, 6, 2,  // Top face
    0, 3, 7, 7, 4, 0,  // Left face
    1, 2, 6, 6, 5, 1   // Right face
};

// Initialize OpenGL settings
void initGL() {
    glEnable(GL_DEPTH_TEST);  // Enable depth testing for 3D rendering
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0,  // Camera position
              0.0, 0.0, 0.0,  // Look-at point
              0.0, 1.0, 0.0); // Up direction
}

// Render the cube
void renderCube() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    
    // Rotate cube
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
    
    // Draw the cube using quads
    glBegin(GL_QUADS);
    for (int i = 0; i < 36; i += 6) {
        for (int j = 0; j < 6; j += 3) {
            glVertex3f(vertices[indices[i + j] * 3], vertices[indices[i + j] * 3 + 1], vertices[indices[i + j] * 3 + 2]);
        }
    }
    glEnd();
    
    glPopMatrix();
    
    glutSwapBuffers();
}

// Update the cube's rotation angle
void update(int value) {
    angleX += 2.0f;  // Increment X rotation angle
    angleY += 3.0f;  // Increment Y rotation angle
    if (angleX > 360) angleX -= 360;
    if (angleY > 360) angleY -= 360;

    glutPostRedisplay();  // Request to redraw the scene
    glutTimerFunc(16, update, 0);  // Update again in 16 ms (approx. 60 FPS)
}

// Handle keyboard input
void handleKeypress(unsigned char key, int x, int y) {
    if (key == 27) {  // ESC key
        glutLeaveMainLoop();  // Exit the program
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);  // Initialize GLUT

    // Set up the window and OpenGL context
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  // Enable double buffering and depth buffer
    glutInitWindowSize(800, 600);
    glutCreateWindow("Simple FreeGLUT OpenGL Cube");

    initGL();  // Initialize OpenGL

    // Set the callback functions
    glutDisplayFunc(renderCube);   // Display callback
    glutKeyboardFunc(handleKeypress);  // Keyboard input callback
    glutTimerFunc(16, update, 0);  // Timer callback for animation

    glutMainLoop();  // Start the main event loop

    return 0;
}
