#include "Shader.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "../External/glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "OpenGL.h"
#include <streambuf>
#include <algorithm>

namespace Gorgon { namespace GL {	


	GLuint CreateShader(GLenum type, const std::string& code, const std::string& name) {
		GLuint shader = glCreateShader(type);

		const GLchar* shader_code_str = code.data();
		glShaderSource(shader, 1, (const GLchar**)&shader_code_str, nullptr);
		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if(status == GL_FALSE) {
			GLint log_length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
			GLchar* shader_log_message = new GLchar[log_length];
			glGetShaderInfoLog(shader, log_length, nullptr, shader_log_message);
			//log << shaderName << " shader failed to compile: " << shader_log_message << '\n';
			delete[] shader_log_message;
		}
		//log << "Compiled shader: " << shaderName << '\n';
		return shader;
	}

	void InsertDefines(std::string& code, const std::string& shader_defines) {
		std::string::size_type pos = code.find("#version");
		if(pos==std::string::npos) {
			pos=0;
		}
		int linesbefore=std::count_if(code.begin(), code.begin()+pos, [](char c) { return c=='\n'; });

		const std::string insert_me = shader_defines + "#line " + std::to_string(linesbefore+1) + "\n";

		code.insert(pos, insert_me);
	}


	Shader::~Shader() {
		if(geometryshader) {
			glDetachShader(program, geometryshader);
			glDeleteShader(geometryshader);
		}

		if(fragmentshader) {
			glDetachShader(program, fragmentshader);
			glDeleteShader(fragmentshader);
		}

		glDetachShader(program, vertexshader);
		glDeleteShader(vertexshader);
		glDeleteProgram(program);
	}

	void Shader::Use() {
		glUseProgram(program);
	}

	unsigned int SetupUBO(int size, int binding_point) {
		GLuint ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);
		return ubo;
	}

	void UpdateUBO(unsigned int ubo, int size, void const * const data) {
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
	}

	int Shader::LocateUniform(const std::string& name) {
		return glGetUniformLocation(program, name.data());
	}

	void Shader::UpdateUniform(int name, float value) {
		glUniform1f(name, value);
	}

	void Shader::UpdateUniform(int name, int value) {
		glUniform1i(name, value);
	}

	void Shader::UpdateUniform(int name, const glm::vec3& value) {
		glUniform3fv(name, 1, (GLfloat*)glm::value_ptr(value));
	}

	void Shader::UpdateUniform(int name, const glm::mat4& value) {
		glUniformMatrix4fv(name, 1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}

	void Shader::UpdateUniform(int name, const glm::mat4x2& value) {
		glUniformMatrix4x2fv(name, 1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}

	void Shader::UpdateUniform(int name, const glm::mat4x3& value) {
		glUniformMatrix4x3fv(name, 1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}

	void Shader::BindUBO(const std::string &name, UBOBindingPoint::Type binding_point) {
		auto index = glGetUniformBlockIndex(program, name.data());
		glUniformBlockBinding(program, index, binding_point);
	}

	void Shader::InitializeFromFiles(const std::string& vertexfile, const std::string& fragmentfile,
		const std::string& geometryfile, std::map<std::string, std::string> defines) {

		std::string vertexsrc, fragmentsrc, geometrysrc;

		std::ifstream file;
		
		file.open(vertexfile);
		if(!file.is_open()) {
			throw std::runtime_error("Cannot open vertex shader file");
		}

		vertexsrc={std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

		file.close();

		if(fragmentfile!="") {
			file.open(fragmentfile);
			if(!file.is_open()) {
				throw std::runtime_error("Cannot open fragment shader file");
			}

			fragmentsrc={std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

			file.close();
		}

		if(geometryfile!="") {
			file.open(fragmentfile);
			if(!file.is_open()) {
				throw std::runtime_error("Cannot open geometry shader file");
			}

			geometrysrc={std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

			file.close();
		}

		InitializeWithSource(vertexsrc, fragmentsrc, geometrysrc, defines);
	}

	void Shader::InitializeWithSource(std::string vertexsrc, std::string fragmentsrc,
		std::string geometrysrc, std::map<std::string, std::string> defines) {

		program = glCreateProgram();

		std::string definesstring;
		for(auto &p : defines) {
			definesstring+="#define "+p.first+"\t"+p.second+"\n";
		}
		
		if(definesstring!="")
			InsertDefines(vertexsrc, definesstring);
		glAttachShader(program, CreateShader(GL_VERTEX_SHADER, vertexsrc, "[vertex]"+name));

		if(fragmentsrc!="") {
			if(definesstring!="")
				InsertDefines(fragmentsrc, definesstring);

			glAttachShader(program, CreateShader(GL_FRAGMENT_SHADER, fragmentsrc, "[fragment]"+name));
		}

		if(geometrysrc!="") {
			if(definesstring!="")
				InsertDefines(geometrysrc, definesstring);

			glAttachShader(program, CreateShader(GL_GEOMETRY_SHADER, geometrysrc, "[geometry]"+name));
		}

		glLinkProgram(program);

		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if(status == GL_FALSE) {
			GLint log_length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
			GLchar* program_log_message = new GLchar[log_length];
			glGetProgramInfoLog(program, log_length, nullptr, program_log_message);

			//log << "Shader program failed to link: " << program_log_message << '\n'; PrintSeparator();
			delete[] program_log_message;
		}
		//log << "Shader program successfully linked.\n"; PrintSeparator();
	}

} }
