#include "Font.h"
#include <Gorgon/Utils/Assert.h>
#include <functional>

namespace Gorgon { namespace Graphics {
	namespace internal {
		Glyph decode(std::string::const_iterator &it, std::string::const_iterator end) {
			Byte b = *it;
			if(b < 127)
				return b;
	
			if((b & 0b11100000) == 0b11000000) {
				++it;
                if(it == end) return 0xfffd;
				Byte b2 = *it;
				
                return ((b & 0b11111) << 6) + (b2 & 0xb111111);
			}
	
			if((b & 0b11110000) == 0b11100000) {
				++it;
                if(it == end) return 0xfffd;
                Byte b2 = *it;
				
                ++it;
                if(it == end) return 0xfffd;
                Byte b3 = *it;
	
				return ((b & 0b1111) << 12) + ((b2 & 0b111111) << 6) + (b3 & 0b111111);
			}
	
			if((b & 0b11111000) == 0b11110000) {
				++it;
                if(it == end) return 0xfffd;
				Byte b2 = *it;

                ++it;
                if(it == end) return 0xfffd;
				Byte b3 = *it;

                ++it;
                if(it == end) return 0xfffd;
				Byte b4 = *it;
	
				return ((b & 0b1111) << 18) + ((b2 & 0b111111) << 12) + ((b3 & 0b111111) << 6) + (b4 & 0b111111);
			}
	
			return 0xfffd;
		}
	
		bool isspaced(Glyph g) {
			return g < 0x300 || g > 0x3ff;
		}
		
		bool isnewline(Glyph g, std::string::const_iterator &it, std::string::const_iterator end) {
            if(*it == 0x0d) {
            }
            
            switch(g) {
            case 0x0d:
                if(it+1 == end) return true;
                
                if(*(it+1) == 0x0a) ++it;
                return true;
                
            case 0x0a: //LF
            case 0x0b: //VTAB
            case 0x0c: //FF
            case 0x85: //NEL
            case 0x2028: //LS
            case 0x2029: //PS
                return true;
                
            default:
                return false;
            }
                
        }

		bool isspace(Glyph g) {
			if(g>=0x2000 && g<=0x200b)
				return true;

			switch(g) {
			case 0x20:
			case 0xa0:
			case 0x1680:
			case 0x202F:
			case 0x205F:
			case 0x3000:
			case 0xfeff:
				return true;

			default:
				return false;
			}
		}

		bool isadjusablespace(Glyph g) {
			switch(g) {
			case 0x20:
			case 0xa0:
			case 0x2002:
			case 0x2003:
			case 0x3000:
				return true;

			default:
				return false;
			}
		}

		bool isbreaking(Glyph g) {
			if(g>=0x2000 && g<=0x200b)
				return true;

			switch(g) {
				case 0x20:
				case 0x1680:
				case 0x2010:
				case 0x3000:
					return true;

				default:
					return false;
			}
		}

		int defaultspace(Glyph g, const GlyphRenderer &renderer) {
			auto em = renderer.GetHeight();

			switch(g) {
			case 0x3000:
				return renderer.GetMaxWidth();

			case 0x2001:
			case 0x2003:
				return em;

			case 0x2000:
			case 0x2002:
				return rounddiv(em, 2);

			case 0x2007:
				return renderer.GetDigitWidth();

			case 0x2004:
				return rounddiv(em, 3);

			case 0x20:
			case 0xa0:
			case 0x2005:
			default:
				return ceildiv(em, 4);

			case 0x25f:
				return ceildiv(em, 18.f/4.f);

			case 0x2009:
			case 0x202f:
				return ceildiv(em, 5);

			case 0x2006:
				return ceildiv(em, 6);

			case 0x200a:
				return ceildiv(em, 8);

			case 0x2008:
				return std::max(renderer.GetSize('.').Width, 1);

			case 0x180e:
			case 0xfeff:
			case 0x200b:
				return 0;
			}
		}

		template<class T_>
		void dodefaulttab(T_ s, T_ &x, T_ w) {
			x -= s;
			x += w;
			x /= w;
			x = (T_)std::floor(x);
			x *= w;
			x += s;
		}

		/// helps with the simple layouts, decodes and executes unicode instructions. Offset parameter in render function
		/// is the offset that must be used after rendering the character. If g is 0, only offset should be processed
		void simpleprint(const GlyphRenderer &renderer, const std::string &text, std::function<int(Glyph, Glyph)> spacing,
						 std::function<void(Glyph, int, int)> render, std::function<void()> dotab, std::function<void(Glyph)> donewline) {

			Glyph prev = 0;
			auto end = text.end();

			for(auto it=text.begin(); it!=end; ++it) {
				Glyph g = internal::decode(it, end);

				if(isspace(g)) {
					int poff = 0;
					if(prev && isspaced(prev)) {
						poff = spacing(prev, g);
					}

					if(renderer.Exists(g)) {
						render(g, poff, renderer.GetSize(g).Width);
					}
					else {
						render(0, poff, defaultspace(g, renderer));
					}
				}
				else if(g == '\t') {
					dotab();

					prev = 0;
				}
				else if(isnewline(g, it, end)) {
					donewline(g);
					prev = 0;
				}
				else if(g > 32) {
					int poff = 0;
					if(prev && isspaced(prev)) {
						poff = spacing(prev, g);
					}

					int sp = 0;
					if(isspaced(g))
						sp = renderer.GetSize(g).Width;

					render(g, poff, sp);
					prev = g;
				}
			}
		}
	}

    void BasicFont::print(TextureTarget& target, const std::string& text, Geometry::Point location, RGBAf color) const {
		auto sp = renderer->GetXSpacing();
		auto cur = location;
		
		internal::simpleprint(
			*renderer, text,
			[&](Glyph prev, Glyph next) { return sp + renderer->KerningDistance(prev, next); },
			[&](Glyph g, int poff, int off) { cur.X += poff; renderer->Render(g, target, cur, color); cur.X += off; },
			std::bind(&internal::dodefaulttab<int>, location.X, std::ref(cur.X), renderer->GetMaxWidth() * 8),
			[&](Glyph) { cur.Y += (int)std::round(renderer->GetHeight() * 1.2); cur.X = location.X; }
		);
    }

    void BasicFont::print(TextureTarget &target, const std::string &text, Geometry::Rectangle location, TextAlignment align, RGBAf color) const {
		auto cur = location.TopLeft();
		Glyph prev = 0;

		std::vector<Glyph> acc;
		std::vector<decltype(location.X)> pos;
		int lastspace = 0;

		auto dest = location.Right();
        auto end = text.end();

		auto donewline = [&](decltype(location.X) diff) {
			int end = lastspace;


			if(align == TextAlignment::Center) {
				float m = (dest - pos[end]) / 2.f;

				// if floating point numbers are not used
				if(abs(pos[end]-(int)pos[end]) < 0.1)
					m=std::round(m); // make sure we land on full pixels

				for(int i=0; i<end; i++) {
					pos[i] += (decltype(location.X))m;
				}
			} 
			else if(align == TextAlignment::Right) {
				auto m = (dest - pos[end]);

				for(int i=0; i<end; i++) {
					pos[i] += m;
				}
			}

			for(int i=0; i<end; i++) {
				renderer->Render(acc[i], target, {(float)pos[i], (float)cur.Y}, color);
			}

			if(end+1 == pos.size()) { //if we are at the last char
				cur.X = location.X;
				acc.clear();
				pos.clear();
			}
			else {
				auto shift = pos[end+1];
				// pull back start of the line so when the current diff is added, 
				// it would still be at the start
				cur.X -= shift + diff; 

				acc.erase(acc.begin(), acc.begin()+end+1);
				pos.erase(pos.begin(), pos.begin()+end+1);

				for(auto &p : pos)
					p -= shift;
			}

			lastspace = 0;

			cur.Y += renderer->GetHeight();
		};

		for(auto it=text.begin(); it!=end; ++it) {
			Glyph g = internal::decode(it, end);

			bool nl = false;
			bool rollback = false;

			if(prev && internal::isspaced(prev)) {
				auto dist = renderer->KerningDistance(prev, g);
				cur.X += dist;
			}

			auto cx = cur.X;

			prev = g;

			if(g == '\t') {
				auto stops  = renderer->GetMaxWidth() * 8;
				cur.X += stops;
				cur.X /= stops;
				cur.X  = (decltype(location.X))std::floor(cur.X);
				cur.X *= stops;
			}
			else if(internal::isnewline(g, it, end)) {
				pos.push_back(cx);
				acc.push_back('\n');
				lastspace = acc.size() - 1;
				prev = 0;
				donewline(cur.X-cx);
				continue;
			}
			else if(g >= 32) {
				if(internal::isspaced(g))
					cur.X += renderer->GetSize(g).Width;
			}

			if(location.Width == 0 || cur.X < dest) {
				if(g == '\t' || internal::isbreaking(g)) {
					lastspace = acc.size();
				}

				pos.push_back(cx);
				acc.push_back(g);
			}
			else {
				auto diff = cur.X - cx;
				if(g == '\t' || internal::isbreaking(g) || lastspace == 0) {
					pos.push_back(cx);
					acc.push_back(g);
					lastspace = acc.size() - 1;
				}

				donewline(diff);

				if(g != '\t' && !internal::isbreaking(g)) {
					pos.push_back(cur.X);
					acc.push_back(g);

					cur.X += diff;
				}
				
			}
		}

		if(acc.size()) {
			pos.push_back(cur.X);
			acc.push_back(0);
			lastspace = acc.size() - 1;
			donewline(0);
		}
	}


    Geometry::Size BasicFont::GetSize(const std::string& text) const {
		auto sp = renderer->GetXSpacing();
		auto cur = Geometry::Point(0, 0);

		int maxx = 0;

		internal::simpleprint(
			*renderer, text,
			[&](Glyph prev, Glyph next) { return sp + renderer->KerningDistance(prev, next); },
			[&](Glyph g, int poff, int off) { cur.X += poff; cur.X += off; },
			std::bind(&internal::dodefaulttab<int>, 0, std::ref(cur.X), renderer->GetMaxWidth() * 8),
			[&](Glyph) { cur.Y += (int)std::round(renderer->GetHeight() * 1.2); if(maxx < cur.X) maxx = cur.X; cur.X = 0; }
		);

		return {maxx, cur.Y + renderer->GetHeight()};
	}

	void StyledRenderer::print(TextureTarget &target, const std::string &text, Geometry::Point location) const {
		if(shadow.type == TextShadow::Flat) {
			print(target, text, location + shadow.offset, shadow.color);
		}

		print(target, text, location, color);
	}

	void StyledRenderer::print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color) const {
		//strike through, underline
		auto sp = renderer->GetXSpacing();
		auto cur = location;

		internal::simpleprint(
			*renderer, text,
			[&](Glyph prev, Glyph next) { return hspace + sp + renderer->KerningDistance(prev, next); },
			[&](Glyph g, int poff, int off) { cur.X += poff; renderer->Render(g, target, cur, color); cur.X += off; },
			std::bind(&internal::dodefaulttab<float>, location.X, std::ref(cur.X), (float)tabwidth),
			[&](Glyph) { cur.Y += (int)std::round(renderer->GetHeight() * vspace + pspace); cur.X = location.X; }
		);
	}

} }
