/*
	Actual shader implementations of ShaderBase go here.
*/

#pragma once

#include <map>
#include <string>

#include "../GL/Shader.h"

#include "../GL/OpenGL.h"
#include "Color.h"


namespace Gorgon { namespace Graphics {

	class SimpleShader : public GL::Shader
	{
	public:
		static SimpleShader &Use() {
			static SimpleShader me;	
			me.Shader::Use(); 
			return me;
		}
		
		SimpleShader &SetVertexCoords(const GL::QuadVertices &value) {
			static int id = LocateUniform("vertex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		/// Sets texture coordinates
		SimpleShader &SetTextureCoords(const GL::QuadTextureCoords &value) {
			static int id = LocateUniform("tex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		/// Sets diffuse texture
		SimpleShader &SetDiffuse(GLuint value) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, value);
			
			return *this;
		}
		
	private:
		SimpleShader();
	};

	class SimpleTintShader : public GL::Shader
	{
	public:
		static SimpleTintShader& Use() { 
			static SimpleTintShader me; 
			me.Shader::Use(); 
			return me;
		}
		
		
		SimpleTintShader &SetVertexCoords(const GL::QuadVertices &value) {
			static int id = LocateUniform("vertex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		SimpleTintShader &SetTextureCoords(const GL::QuadTextureCoords &value) {
			static int id = LocateUniform("tex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		SimpleTintShader &SetTint(const Graphics::RGBAf &value) {
			static int id = LocateUniform("tint");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		SimpleTintShader &SetDiffuse(GLuint value) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, value);
			
			return *this;
		}
		
	private:
		SimpleTintShader();
	};
	/*
	class MaskedShader : public GL::Shader
	{
	public:
        static MaskedShader& Use() { 
			static MaskedShader me; 
			me.Shader::Use(); 
			return me;
		}
		
		MaskedShader &SetVertexCoords(const GL::QuadVertices &value) {
			static int id = LocateUniform("vertex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		MaskedShader &SetTextureCoords(const glm::mat4x4 &value) {
			static int id = LocateUniform("tex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		MaskedShader &SetDiffuse(GLuint value) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, value);
			
			return *this;
		}
		
		MaskedShader &SetMask(GLuint value) {
			static int id = LocateUniform("mask");
			UpdateUniform(id, 1);
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, value);
			
			return *this;
		}
	private:
		MaskedShader();
	};
	

	class TintedMaskedShader : public GL::Shader
	{
	public:
        static TintedMaskedShader& Use() {
			static TintedMaskedShader me;
			me.Shader::Use();
			return me;
		}
		
		TintedMaskedShader &SetVertexCoords(const GL::QuadVertices &value) {
			static int id = LocateUniform("vertex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		TintedMaskedShader &SetTextureCoords(const glm::mat4x4 &value) {
			static int id = LocateUniform("tex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		TintedMaskedShader &SetTint(const Graphics::RGBAf &value) {
			static int id = LocateUniform("tint");
			UpdateUniform(id, *(glm::vec4*)&value);
			
			return *this;
		}
		
		TintedMaskedShader &SetDiffuse(GLuint value) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, value);
			
			return *this;
		}
		
		TintedMaskedShader &SetMask(GLuint value) {
			static int id = LocateUniform("mask");
			UpdateUniform(id, 1);
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, value);
			
			return *this;
		}
	private:
		TintedMaskedShader();
	};*/
} }
