#include "shader.h"

#include <GL/glew.h>
#include <GL/glx.h>

#include <iostream>
#include <chrono>

const char* vertex_shader_source = R"(
	#version 130
	in vec2 aPos;
	void main() {
		gl_Position = vec4(aPos, 0.0, 1.0);
	}
)";

const char* fragment_shader_source = R"(
	#version 130
	out vec4 FragColor;
	void main() {
		FragColor = vec4(1.0, 0.647, 0.0, 1.0); // Orange color
	}
)";

GLuint compile_shader(GLenum shader_type, const char* source)
{
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log);
		std::cerr << "error: shader compilation failed\n" << log << std::endl;
	}
	return shader;
}

GLuint create_shader_program()
{
	GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
	GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	GLint status;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
	if (!status) {
		char log[512];
		glGetProgramInfoLog(shader_program, 512, nullptr, log);
		std::cerr << "error: shader linking failed\n" << log << std::endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
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
		std::cerr << "ERROR: Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// triangle mesh
	float vertices[] = {
		0.0f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	}

	// create vertex buffers
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GLuint shaderProgram = create_shader_program();

	// main loop
	bool runing = true;
	while (runing)
	{
		XEvent event;
		while (XPending(display)) {
			XNextEvent(display, &event);

			if (event.type == KeyPress) {
				XCloseDisplay(display);
				runing = false;
			}
		}

		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glXSwapBuffers(display, win);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glXDestroyContext(display, glContext);
	XDestroyWindow(display, win);
	XCloseDisplay(display);

	return 0;
}
