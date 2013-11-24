/*
	Actual shader implementations of ShaderBase go here.
*/

#pragma once

#include "../Engine/ShaderBase.h"
#include "../Engine/Graphics.h"

#include <map>
#include <string>

namespace gge {
namespace graphics {
	union RGBfloat;
	
}

namespace shaders {

	class SimpleShader : public gge::graphics::ShaderBase
	{
	public:
		static SimpleShader &Use() {
			static SimpleShader me;	
			me.ShaderBase::Use(); 
			return me;
		}
		
		SimpleShader &SetVertexCoords(const glm::mat4x3 &value) {
			static int id = LocateUniform("vertex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		SimpleShader &SetTextureCoords(const glm::mat4x2 &value) {
			static int id = LocateUniform("tex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		SimpleShader &SetDiffuse(GLuint value) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, value);
			
			return *this;
		}
		
	private:
		SimpleShader();
	};

	class SimpleTintShader : public gge::graphics::ShaderBase
	{
	public:
		static SimpleTintShader& Use() { 
			static SimpleTintShader me; 
			me.ShaderBase::Use(); 
			return me;
		}
		
		
		SimpleTintShader &SetVertexCoords(const glm::mat4x3 &value) {
			static int id = LocateUniform("vertex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		SimpleTintShader &SetTextureCoords(const glm::mat4x2 &value) {
			static int id = LocateUniform("tex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		SimpleTintShader &SetTint(const graphics::RGBfloat &value) {
			static int id = LocateUniform("tint");
			UpdateUniform(id, *(glm::vec4*)&value.vect);
			
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

	class MaskedShader : public gge::graphics::ShaderBase
	{
	public:
        static MaskedShader& Use() { 
			static MaskedShader me; 
			me.ShaderBase::Use(); 
			return me;
		}
		
		MaskedShader &SetVertexCoords(const glm::mat4x3 &value) {
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
	

	class TintedMaskedShader : public gge::graphics::ShaderBase
	{
	public:
        static TintedMaskedShader& Use() {
			static TintedMaskedShader me;
			me.ShaderBase::Use();
			return me;
		}
		
		TintedMaskedShader &SetVertexCoords(const glm::mat4x3 &value) {
			static int id = LocateUniform("vertex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		TintedMaskedShader &SetTextureCoords(const glm::mat4x4 &value) {
			static int id = LocateUniform("tex_coords");
			UpdateUniform(id, value);
			
			return *this;
		}
		
		TintedMaskedShader &SetTint(const graphics::RGBfloat &value) {
			static int id = LocateUniform("tint");
			UpdateUniform(id, *(glm::vec4*)&value.vect);
			
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
	};
} }
