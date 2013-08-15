/*
  Contains base class for all shaders ShaderBase
  and facilities for loading/compiling shaders.
*/
#pragma once

#include <string>
#include <map>
#include "../External/glm/glm.hpp"
#include "OpenGL.h"

namespace gge { namespace graphics {

	namespace UBOBindingPoint			{ enum Type { Resolution, }; }
	namespace TextureUnit				{ enum Type { Diffuse, Normal, Specular, Depth }; }

	unsigned int						SetupUBO(int size, int binding_point);
	void								UpdateUBO(unsigned int ubo, int size, void const * const data);	

	class ShaderBase
	{
	public:
										~ShaderBase();
		void							Use();
		bool							IsInitialized();
		void							LocateUniform(const std::string& name);
		void							UpdateUniform(const std::string& name, float value);
		void							UpdateUniform(const std::string& name, int value);
		void							UpdateUniform(const std::string& name, const glm::vec3& value);
		void							UpdateUniform(const std::string& name, const glm::vec4& value);
		void							UpdateUniform(const std::string& name, const glm::mat3& value);
		void							UpdateUniform(const std::string& name, const glm::mat3x2& value);
		void							UpdateUniform(const std::string& name, const glm::mat4& value);
		void							UpdateUniform(const std::string& name, const glm::mat4x2& value);
		void							UpdateUniform(const std::string& name, const glm::mat4x3& value);
		void							BindUBO(const std::string& name, UBOBindingPoint::Type bindingPoint );
	protected:
		unsigned int					mVertexShader;
		unsigned int					mGeometryShader;
		unsigned int					mFragmentShader;
		unsigned int					mProgram;
		explicit						ShaderBase() : mProgram(0), mVertexShader(0), mGeometryShader(0), mFragmentShader(0) { }
		void							InitializeWithSource(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc = "", 
											   const std::string& geometryShaderSrc = "", const std::string& shaderDefines = "");
		void							InitializeFromFiles(const std::string& vertexShaderName, const std::string& fragmentShaderName = "", 
											   const std::string& geometryShaderName = "", const std::string& shaderDefines = "");
		
	private:
		std::map<std::string, int>		mHandles;
										ShaderBase(const ShaderBase&);
		ShaderBase&						operator=(const ShaderBase&);
		typedef GLuint (*InitializeShaderFunc) (GLenum, const std::string&, const std::string&);
		void							Initialize(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, 
											   const std::string& shaderDefines, InitializeShaderFunc initializeShaderFunc);
	};
	
} }