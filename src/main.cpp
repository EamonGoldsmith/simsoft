#include "shader.h"

#include <GL/glew.h>
#include <GL/glx.h>

#include <iostream>
#include <chrono>

const char* vertex_shader_source = R"(
	#version 330 core
	layout (location=0) in vec3 aPos;

	out vec4 vertexColor;

	void main()
	{
		gl_Position = vec4(aPos, 1.0);
		vertexColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
)";

const char* fragment_shader_source = R"(
	#version 330 core
	out vec4 FragColor;
	
	in vec4 vertexColor;

	void main()
	{
		FragColor = vertexColor;
	}
)";

GLuint compile_shader(GLenum shader_type, const char* source)
{
	// create a compile shader
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	// check for error
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log);
		std::cerr << "error: shader compilation failed:\n" << log << std::endl;
	}

	return shader;
}

GLuint create_shader_program(const char *v_source, const char *f_source)
{
	// compile shaders
	GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, v_source);
	GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, f_source);

	// link
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	// check for error
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

	GLuint shader_prog = create_shader_program(vertex_shader_source, fragment_shader_source);

	// triangle mesh
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	// create vertex buffers
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// main loop
	bool running = true;
	while (running)
	{
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

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_prog);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glXSwapBuffers(display, win);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shader_prog);

	glXDestroyContext(display, glContext);
	XDestroyWindow(display, win);
	XCloseDisplay(display);

	return 0;
}
