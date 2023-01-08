#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
class Shader
{
public:
	GLuint Program;
	// Constructor generates the shader on the fly
	Shader(const GLchar *vertexPath, const GLchar *fragmentPath)
	{
		// 1. Retrieve th vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensures ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		try
		{
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}

		/**
		* Create the Vertex and Fragment shader
		*/
		const GLchar *vShaderCode = vertexCode.c_str();
		const GLchar *fShaderCode = fragmentCode.c_str();
		GLuint vertex, fragment;
		GLint success;
		// Create the Vertex shader handle
		vertex = glCreateShader(GL_VERTEX_SHADER);
		// Manipulate the desired shader with the source code
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		// Print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (success != GL_TRUE)
		{
			GLchar infoLog[512];
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED:\n " << infoLog << std::endl;
		}
		// Create the Fragment shader handle
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		// Manipulate the desired shader with the source code
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		// Print compile errors if any
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (success != GL_TRUE)
		{
			GLchar infoLog[512];
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED:\n " << infoLog << std::endl;
		}
		/**
		* Now linking the shaders
		*/

		// Create Shader Program
		this->Program = glCreateProgram();
		// Attach the shaders we've created previously above to the created program object then link it.
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		glLinkProgram(this->Program);
		// Print compile errors if any
		glGetProgramiv(Program, GL_LINK_STATUS, &success);
		if (success != GL_TRUE)
		{
			GLchar infoLog[512];
			glGetProgramInfoLog(Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED:\n " << infoLog << std::endl;
		}
		/**
		* Since our vertex and fragment shader are part of our program
		* and linked to it, then we don't need them anymore
		*/
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	// Uses the current shader
	void Use()
	{
		glUseProgram(this->Program);
	}
};

#endif // !SHADER_H
