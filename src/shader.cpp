#include "shader.h"

#include <GL/glew.h>
#include <GL/glx.h>

#include <iostream>
#include <fstream>
#include <sstream>

std::string load_from_file(const std::string& path)
{
	std::ifstream file;
	std::string content;

	file.open(path, std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		std::cerr << "error: couldn't open file: " << path << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	file.close();

	content = buffer.str();

	return content;
}

unsigned int compile_shader(GLenum shader_type, const char *source)
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

unsigned int create_shader_program(const char *v_source, const char *f_source)
{
	// compile shaders
	unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, v_source);
	unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, f_source);

	// link
	unsigned int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	// check for error
	int status;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
	if (!status) {
		char log[512];
		glGetProgramInfoLog(shader_program, 512, nullptr, log);
		std::cerr << "error: shader linking failed\n" << log << std::endl;
	}

	// these are no longer needed
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

Shader::Shader(const char *vertex_path, const char *fragment_path)
{
	// load files into string
	std::string vertex_string = load_from_file(vertex_path);
	std::string fragment_string = load_from_file(fragment_path);

	// convert string to (const char*)
	const char *vertex_code = vertex_string.c_str();
	const char *fragment_code = fragment_string.c_str();

	// create shader
	ID = create_shader_program(vertex_code, fragment_code);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
