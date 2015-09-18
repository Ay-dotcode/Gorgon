/// Contains base class for all shaders ShaderBase
/// and facilities for loading/compiling shaders.

#pragma once

#include <string>
#include <map>
#include "../External/glm/glm.hpp"

namespace Gorgon {
	namespace GL {

		namespace UBOBindingPoint			{ enum Type { Resolution, }; }
		namespace TextureUnit				{ enum Type { Diffuse, Normal, Specular, Depth, AlphaMask }; }

		unsigned int						SetupUBO(int size, int binding_point);
		void								UpdateUBO(unsigned int ubo, int size, void const * const data);

		class Shader
		{
		protected:
			~Shader();

			Shader(const Shader&) = delete;
			Shader&operator=(const Shader&) = delete;

			void Use();
			bool IsInitialized();
			int  LocateUniform(const std::string& name);
			void UpdateUniform(int name, float value);
			void UpdateUniform(int name, int value);
			void UpdateUniform(int name, const glm::vec3& value);
			void UpdateUniform(int name, const glm::vec4& value);
			void UpdateUniform(int name, const glm::mat3& value);
			void UpdateUniform(int name, const glm::mat3x2& value);
			void UpdateUniform(int name, const glm::mat4& value);
			void UpdateUniform(int name, const glm::mat4x2& value);
			void UpdateUniform(int name, const glm::mat4x3& value);
			void BindUBO(const std::string& name, UBOBindingPoint::Type bindingPoint);

		protected:

			Shader(const std::string &name) : name(name), program(0), vertexshader(0), geometryshader(0), fragmentshader(0) {}

			void InitializeWithSource(std::string vertexsrc, std::string fragmentsrc = "",
				std::string geometrysrc = "", std::map<std::string, std::string> defines={});

			void InitializeFromFiles(const std::string& vertexfile, const std::string& fragmentfile = "",
				const std::string& geometryfile = "", std::map<std::string, std::string> defines={});

			std::string name;

		private:
			unsigned int					vertexshader;
			unsigned int					geometryshader;
			unsigned int					fragmentshader;
			unsigned int					program;


			//void Initialize(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader,
			//	const std::string& shaderDefines, InitializeShaderFunc initializeShaderFunc);
		};

	}
}
