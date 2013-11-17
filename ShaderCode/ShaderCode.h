/*
	Actual shader implementations of ShaderBase go here.
*/

#pragma once

#include "../Engine/ShaderBase.h"

#include <map>
#include <string>

namespace gge { namespace shaders {

	extern const std::string				TransformVertSrcCode;
	extern const std::string				SimpleFragSrcCode;
	extern const std::string				SimpleTintFragSrcCode;
	extern const std::string 				WidgetVertexSrcCode;
	extern const std::string 				WidgetFragmentSrcCode;
	extern const std::string 				TintedWidgetFragmentSrcCode;

	class SimpleShader : public gge::graphics::ShaderBase
	{
	public:
		static SimpleShader&				Get() {	static SimpleShader me;	return me; }
		static void							Use() {	Get().ShaderBase::Use(); }
	private:
											SimpleShader();
	};

	class SimpleTintShader : public gge::graphics::ShaderBase
	{
	public:
		static SimpleTintShader&			Get() { static SimpleTintShader me; return me; }
		static void							Use() { Get().ShaderBase::Use(); }
	private:
											SimpleTintShader();
	};

	class Shade3DShader : public gge::graphics::ShaderBase
	{
	public:
        static Shade3DShader&				Get() { static Shade3DShader me; return me; }
		static void							Use() { Get().ShaderBase::Use(); }
	private:
											Shade3DShader();
	};

	class MaskedShader : public gge::graphics::ShaderBase
	{
	public:
	    static MaskedShader&				Get() { static MaskedShader me; return me; }
        static void							Use() { Get().ShaderBase::Use(); }
	private:
											MaskedShader();
	};

	class TintedMaskedShader : public gge::graphics::ShaderBase
	{
	public:
	    static TintedMaskedShader&			Get() { static TintedMaskedShader me; return me; }
        static void							Use() { Get().ShaderBase::Use(); }
	private:
											TintedMaskedShader();
	};
} }
