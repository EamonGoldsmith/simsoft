#include "shader.h"

#include <GL/glew.h>
#include <GL/glx.h>

#include <iostream>
#include <chrono>
#include <thread>

const std::chrono::milliseconds update_duration(2);

void update(
	std::chrono::duration<double> delta,
	std::chrono::duration<double> lag
	)
{
}

int main() 
{
	Display* display = XOpenDisplay(nullptr);
	if (display == nullptr) {
		std::cerr << "error: unable to open X display" << std::endl;
		return -1;
	}

	// create a screen
	int screen = DefaultScreen(display);
	GLXFBConfig fbConfig;
	GLXFBConfig* fbConfigs;
	int fbCount;
	static int fbAttribs[] = {
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER, True,
		GLX_DEPTH_SIZE, 24,
		None
	};
	fbConfigs = glXChooseFBConfig(display, screen, fbAttribs, &fbCount);
	fbConfig = fbConfigs[0];
	XFree(fbConfigs);

	// create an opengl context
	XVisualInfo* visualInfo = glXGetVisualFromFBConfig(display, fbConfig);
	Colormap colormap = XCreateColormap(display, RootWindow(display, screen), visualInfo->visual, AllocNone);

	XSetWindowAttributes swa;
	swa.colormap = colormap;
	swa.event_mask = ExposureMask | KeyPressMask;

	Window root = RootWindow(display, screen);
	Window win = XCreateWindow(display, root, 0, 0, 800, 600, 0, visualInfo->depth,
			       InputOutput, visualInfo->visual, CWColormap | CWEventMask, &swa);

	XMapWindow(display, win);
	XStoreName(display, win, "robotics SIMulation SOFTware");

	GLXContext glContext = glXCreateContext(display, visualInfo, nullptr, GL_TRUE);
	glXMakeCurrent(display, win, glContext);

	// initialise glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "error: Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// triangle mesh
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f
	};

	// create a new shader
	Shader ourShader("shaders/shader.vs", "shaders/shader.fs");

	// create vertex buffers
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	// create timers
	auto last = std::chrono::steady_clock::now();
	auto current = last;
	std::chrono::duration<double> delta, lag;

	// main loop
	bool running = true;
	while (running)
	{
		// frame timing
		current = std::chrono::steady_clock::now();
		delta = current - last;
		last = current;
		lag += delta;

		XEvent event;
		while (XPending(display)) {
			XNextEvent(display, &event);

			if (event.type == KeyPress) {
				KeySym key = XLookupKeysym(&event.xkey, 0);

				// quit on 'q'
				if (key == 113) {
					running = false;
				}
			}
		}

		// fix timestep
		while (lag >= update_duration) {
			update(delta, lag);
			lag -= update_duration;
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		ourShader.setFloat("pos_offset", 0.5f);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glXSwapBuffers(display, win);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glXDestroyContext(display, glContext);
	XDestroyWindow(display, win);
	XCloseDisplay(display);

	return 0;
}

