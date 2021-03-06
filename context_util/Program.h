//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * Program.h
 *
 *  Created on: Nov 17, 2020
 *      Author: gio
 */
#pragma once
#ifndef PROGRAM_H_
#define PROGRAM_H_

#include "CommonHeaders.h"
#include "ShaderException.h"

/*
 * This class create a GLSL program with vertex-Shader and Fragment-Shaders
 */
class Program {
private:
	unsigned int ID;
	std::string name;

	std::string loadsource(const char *source);
	unsigned int compileShader(GLenum type, const char *src, const char *path);
	bool checkCompiledShader(const GLchar * shaderFileName, GLuint shaderID, std::string type);
	bool checkLinkedProgram(unsigned int pId);

	Program(Program &&other) = delete;
	Program& operator=(const Program &other) = delete;
	Program& operator=(Program &&other) = delete;

public:
	Program(std::string name, const char* vertexPath, const char* fragmentPath);
	virtual ~Program();
	Program(const Program &other);

	unsigned int getID();
	bool isLinked();
	std::string getName();



};

#endif /* PROGRAM_H_ */
