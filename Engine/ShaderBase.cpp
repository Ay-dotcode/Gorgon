#include "ShaderBase.h"
#include "OS.h"
#include "Graphics.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "../External/glm/gtc/type_ptr.hpp"

#define SHADER_DIRECTORY "Shaders"

namespace gge { namespace graphics {	

	GLuint	LoadShaderFromFiles(GLenum shaderType, const std::string& shaderFileName, const std::string& shaderDefines);
	GLuint	LoadShaderSourceCode(GLenum shaderType, const std::string& shaderCode, const std::string& shaderDefines);
	GLuint	CreateProgram(const std::vector<GLuint>& shaderList);
	void	InsertDefines( std::string& shader_code, const std::string& shader_defines );
	void	PrintSeparator() { log << "------------------------------------\n"; }

	void ShaderBase::Initialize(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& shaderDefines, InitializeShaderFunc initializeShaderFunc)
	{
		std::vector<GLuint>	shader_list;
		mVertexShader = initializeShaderFunc(GL_VERTEX_SHADER, vertexShader, shaderDefines);
		shader_list.push_back(mVertexShader);
		if ( !vertexShader.empty() ) {
			mFragmentShader	= initializeShaderFunc(GL_FRAGMENT_SHADER, fragmentShader, shaderDefines);
			shader_list.push_back(mFragmentShader);
		}	
		if ( !geometryShader.empty() ) {
			mGeometryShader	= initializeShaderFunc(GL_GEOMETRY_SHADER, geometryShader, shaderDefines);
			shader_list.push_back(mGeometryShader);
		}	
		mProgram = CreateProgram(shader_list);		
		if (mProgram == 0) {
			throw std::runtime_error("Cannot initialize shader.");
		}
	}

	void ShaderBase::InitializeWithSource(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc, const std::string& geometryShaderSrc, const std::string& shaderDefines)
	{
		Initialize(vertexShaderSrc, fragmentShaderSrc, geometryShaderSrc, shaderDefines, LoadShaderSourceCode);
	}

	void ShaderBase::InitializeFromFiles(const std::string& vertexShaderName, const std::string& fragmentShaderName, const std::string& geometryShaderName, const std::string& shaderDefines)
	{
		Initialize(vertexShaderName, fragmentShaderName, geometryShaderName, shaderDefines, LoadShaderFromFiles);
	}
	
	ShaderBase::~ShaderBase()
	{
		glUseProgram(0);//!??
		if ( mGeometryShader ) {
			glDetachShader( mProgram, mGeometryShader );
			glDeleteShader( mGeometryShader );
		}
		if ( mFragmentShader ) {
			glDetachShader( mProgram, mFragmentShader );
			glDeleteShader( mFragmentShader );
		}
		glDetachShader( mProgram, mVertexShader );
		glDeleteShader( mVertexShader );	
		glDeleteProgram( mProgram );
	}

	void ShaderBase::Use()
	{
		glUseProgram( mProgram );
	}	

	GLuint CreateShader(GLenum shaderType, const std::string& shaderCode, const std::string& shaderName)
	{
		GLuint shader = glCreateShader( shaderType );

		const GLchar* shader_code_str = shaderCode.data();
		glShaderSource(shader, 1, (const GLchar**) &shader_code_str, nullptr);
		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint log_length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
			GLchar* shader_log_message = new GLchar[log_length];	
			glGetShaderInfoLog(shader, log_length, nullptr, shader_log_message);
			log << shaderName << " shader failed to compile: " << shader_log_message << '\n';
			delete[] shader_log_message;
		}
		log << "Compiled shader: " << shaderName << '\n';
		return shader;
	}

	GLuint LoadShaderFromFiles( GLenum shaderType, const std::string& shaderFileName, const std::string& shaderDefines )
	{
		std::string shaderFileFullName(os::filesystem::LocateResource(shaderFileName, SHADER_DIRECTORY));
		std::fstream shaderFile(shaderFileFullName);
		if (!shaderFile) {
			throw std::runtime_error("Make sure " + shaderFileFullName + " is present.");
		}
		std::stringstream buffer;
		buffer << shaderFile.rdbuf();
		std::string shader_code = buffer.str();
		InsertDefines(shader_code, shaderDefines);	// changes shader_code
		log << "Opened shader: " << shaderFileFullName << '\n';
		return CreateShader(shaderType, shader_code, shaderFileName);
	}

	GLuint LoadShaderSourceCode( GLenum shaderType, const std::string& shaderCode, const std::string& shaderDefines )
	{
		std::string shader_code_ext(shaderCode);
		InsertDefines(shader_code_ext, shaderDefines);	// changes shader_code_ext
		log << "Opened shader: embedded" << '\n';
		return CreateShader( shaderType, shaderCode, "embedded" );
	}

	GLuint CreateProgram( const std::vector<GLuint>& shader_list )
	{
		GLuint program = glCreateProgram();
		for (size_t i = 0; i < shader_list.size(); ++i) {
			glAttachShader( program, shader_list[i] );
		}
		glLinkProgram( program );

		GLint status;
		glGetProgramiv( program, GL_LINK_STATUS, &status );
		if ( status == GL_FALSE ) {
			GLint log_length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
			GLchar* program_log_message = new GLchar[log_length];	
			glGetProgramInfoLog( program, log_length, nullptr, program_log_message );

			log << "Shader program failed to link: " << program_log_message << '\n'; PrintSeparator();
			delete[] program_log_message;
		}
		log << "Shader program successfully linked.\n"; PrintSeparator();
		return program;
	}

	void InsertDefines( std::string& shader_code, const std::string& shader_defines )
	{
		std::stringstream stream( shader_code );
		std::string line;
		int line_count = 1;
		std::string::size_type	char_count = 0;
		while ( std::getline( stream, line ) ) {
			line_count++;
			char_count += line.size() + 1;			// +1 because the newline was discarded when being read from stream, and we want to count that newline as well
			if ( line.find( "#version" ) != std::string::npos ) break;
		}
		const std::string insert_me = shader_defines + "#line " + std::to_string( static_cast<long long>( line_count ) ) + "\n";	// change this mess when moved to msvc2012
		shader_code.insert( char_count, insert_me );
	}
		
	unsigned int SetupUBO(int size, int binding_point)
	{		
		GLuint ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);
		return ubo;
	}

	void UpdateUBO(unsigned int ubo, int size, void const * const data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data );
	}

	void ShaderBase::LocateUniform(const std::string& name)
	{
		if (mHandles.find(name) != end(mHandles)) std::runtime_error("Trying to create uniforms with same names.");	
		mHandles[name] = glGetUniformLocation(mProgram, name.data());
	}

	void ShaderBase::UpdateUniform(const std::string& name, float value)
	{
		glUseProgram(mProgram);
		glUniform1f(mHandles[name], value);
	}

	void ShaderBase::UpdateUniform(const std::string& name, int value)
	{
		glUseProgram(mProgram);
		glUniform1i(mHandles[name], value);
	}

	void ShaderBase::UpdateUniform(const std::string& name, const glm::vec3& value)
	{
		glUseProgram(mProgram);
		glUniform3fv(mHandles[name], 1, (GLfloat*)glm::value_ptr(value));
	}

	void ShaderBase::UpdateUniform(const std::string& name, const glm::vec4& value)
	{
		glUseProgram(mProgram);
		glUniform4fv(mHandles[name], 1, (GLfloat*)glm::value_ptr(value));
	}

	void ShaderBase::UpdateUniform(const std::string& name, const glm::mat3& value)
	{
		glUseProgram(mProgram);
		glUniformMatrix3fv(mHandles[name], 1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}

	void ShaderBase::UpdateUniform(const std::string& name, const glm::mat3x2& value)
	{
		glUseProgram(mProgram);
		glUniformMatrix3x2fv(mHandles[name], 1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}

	void ShaderBase::UpdateUniform(const std::string& name, const glm::mat4& value)
	{
		glUseProgram(mProgram);
		glUniformMatrix4fv(mHandles[name], 1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}
	
	void ShaderBase::UpdateUniform(const std::string& name, const glm::mat4x2& value)
	{
		glUseProgram(mProgram);
		glUniformMatrix4x2fv(mHandles[name], 1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}

	void ShaderBase::UpdateUniform(const std::string& name, const glm::mat4x3& value)
	{
		glUseProgram(mProgram);
		glUniformMatrix4x3fv(mHandles[name], 1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}

	void ShaderBase::BindUBO(const std::string& name, UBOBindingPoint::Type binding_point)
	{		
		glUseProgram(mProgram);
		auto index = glGetUniformBlockIndex(mProgram, name.data());
		glUniformBlockBinding(mProgram, index, binding_point);
	}

} }