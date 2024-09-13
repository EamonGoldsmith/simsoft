#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#include "file_loader.h"

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void display()
{
}

void idle()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void handle_keys(unsigned char key, int x, int y)
{
	if (key == 'Q') {

	}
}

int main(int argc, char** argv)
{
	// init GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  // double buffer and depth
	// create new window
	glutInitWindowSize(800, 600);
	glutCreateWindow("robotics SIMulation SOFTware");

	// debug info
	std::cout << "dbg: starting with glut ver:" << glutGet(GLUT_VERSION) << std::endl;

	// init GLEW
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK) {
		std::cerr << "err: failed to initialise GLEW: " << glewGetErrorString(glew_err);
		return 1;
	}

	// set viewport
	glViewport(0, 0, 800, 600);

	// register callbacks
	glutKeyboardFunc(handle_keys);

	while ()
	{
	}

	return 0;
}
