#include "Font.h"
#include <Gorgon/Utils/Assert.h>
#include <functional>

//!todo y-kerning support

namespace Gorgon { namespace Graphics {
	namespace internal {
		Glyph decode(std::string::const_iterator &it, std::string::const_iterator end) {
			Byte b = *it;
			if(b < 127) {
				if(b == '\r') {
					if(it+1 != end && *(it+1) == '\n') {
						++it;

						return '\n';
					}
					else
						return b;
				}
				else
					return b;
			}
			
			if(b == 255) {
				++it;
                if(it == end) return 0xfffd;
				Byte b2 = *it;
                
                if(b2 == 254) return 0; //bom
                
                --it;
                return 0xfffd;
            }
	
			if((b & 0b11100000) == 0b11000000) {
				++it;
                if(it == end) return 0xfffd;
				Byte b2 = *it;
				
                return ((b & 0b11111) << 6) | (b2 & 0b111111);
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

		bool isnewline(Glyph g) {
			switch(g) {
				case 0x0d: //CR
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

		bool isadjustablespace(Glyph g) {
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

		float defaultspace(Glyph g, const GlyphRenderer &renderer) {
			auto em = renderer.GetEMSize();

			switch(g) {
			case 0x3000:
				return (float)renderer.GetMaxWidth();

			case 0x2001:
			case 0x2003:
				return (float)em;

			case 0x2000:
			case 0x2002:
				return (float)rounddiv(em, 2);

			case 0x2007:
				return (float)renderer.GetDigitWidth();

			case 0x2004:
				return (float)rounddiv(em, 3);

			case 0x20:
			case 0xa0:
			case 0x2005:
			default:
				return (float)ceildiv(em, 4);

			case 0x25f:
				return (float)ceildiv(em, 18.f/4.f);

			case 0x2009:
			case 0x202f:
				return (float)ceildiv(em, 5);

			case 0x2006:
				return (float)ceildiv(em, 6);

			case 0x200a:
				return (float)ceildiv(em, 8);

			case 0x2008:
				return std::max(renderer.GetCursorAdvance('.'), 1.f);

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
		void simpleprint(
		const GlyphRenderer &renderer, std::string::const_iterator begin, std::string::const_iterator end,
		std::function<int(Glyph, Glyph)> spacing,
		std::function<int(Glyph)> advance,
		std::function<void(Glyph, int, float)> render,
        std::function<void()> dotab, std::function<void(Glyph)> donewline) {
			Glyph prev = 0;
			int ind = 0;

			for(auto it=begin; it!=end; ++it) {
				Glyph g = internal::decode(it, end);
				int poff = 0;

				ind++;

				if(isspace(g)) {
					if(prev) {
						poff = spacing(prev, g);
					}

					if(renderer.Exists(g)) {
						render(g, poff, renderer.GetCursorAdvance(g));
					}
					else {
						render(0, poff, defaultspace(g, renderer));
					}

					prev = g;
				}
				else if(g == '\t') {
					if(prev) {
						poff = spacing(prev, g);
					}

					render(g, 0, renderer.GetCursorAdvance(g));

					dotab();

					prev = 0;
				}
				else if(isnewline(g)) {
					donewline(g);
					prev = 0;

					ind = 0;
				}
				else if(g > 32) {
					if(prev) {
						poff = spacing(prev, g);
					}

					int sp = 0;
                    sp = advance(g);

					render(g, poff, (float)sp);
					prev = g;
				}
			}

			if(ind != 0) {
				donewline(0);
			}
		}

		struct glyphmark {
			int location;
			Glyph g;
		};

		using markvecit = std::vector<glyphmark>::iterator;

		void boundedprint(
        const GlyphRenderer &renderer, std::string::const_iterator begin, std::string::const_iterator end, int width,
        std::function<void(Glyph/*terminator, 0 => wrap*/, markvecit/*begin*/, markvecit/*end*/, int/*totalwidth*/)> doline,
        std::function<int(Glyph, Glyph)> spacing,
		std::function<int(Glyph)> advance,
        std::function<void(int &)> dotab) {
			std::vector<glyphmark> acc;
			int lastbreak = 0;
			int ind = 0;
			int x = 0;
			bool autobreak = false;
			Glyph prev = 0;

			for(auto it=begin; it!=end; ++it) {
				Glyph g = internal::decode(it, end);

				int cur_spacing = 0, prev_gw = 0;

				if(isbreaking(g)) {
					lastbreak = (int)acc.size();
				}

				if(isspace(g)) {
					if(prev) {
						cur_spacing = spacing(prev, g);
                        acc.back().location += cur_spacing;
					}

					if(renderer.Exists(g)) {
						cur_spacing += advance(g);
					}
					else {
						cur_spacing += (int)defaultspace(g, renderer);
					}

					prev = g;
				}
				else if(g == '\t') {
					auto px = x;
					dotab(x);
					cur_spacing = x - px;
					x = px;

					prev = 0;
				}
				else if(isnewline(g)) {
					doline(g, acc.begin(), acc.end(), x);

					autobreak = false;
					x = 0;
					prev = 0;
					acc.clear();
					lastbreak = 0;
					continue;
				}
				else if(g > 32) {
					if(prev) {
						cur_spacing = spacing(prev, g);
					}

					prev_gw = advance(g);

					prev = g;
				}

				if(x + cur_spacing + prev_gw > width) {
					int totw = 0;
					// if we are placing a space
					if(g == '\t' || isbreaking(g)) {
						lastbreak = (int)acc.size();
						totw = x;
					}
					else if(lastbreak == 0) {
						// this means we cannot fit any characters at all
						// in this case we should at least do one char.
						if(ind == 0) {
							acc.push_back({x, g});

							x += cur_spacing;
                            x += prev_gw;
						}
						else {
							it--;
						}

						totw = x;
						lastbreak = (int)acc.size();
					}
					else { //regular break
						acc.push_back({x, g});

						x += cur_spacing;
                        x += prev_gw;
					}

					//if exists rollback spaces at the end
					int sp = 0;
					while(acc.rbegin()+sp != acc.rend() && isspace((acc.rbegin()+sp)->g)) sp++;

					if(totw == 0 && acc.size()) //in the middle
						totw = (acc.begin()+lastbreak)->location;

					doline(0, acc.begin(), acc.begin()+lastbreak-sp, totw);

					//rollback section
					if(lastbreak == acc.size()) {
						acc.clear();
					}
					else {
						acc.erase(acc.begin(), acc.begin()+lastbreak+1);
					}

					//remove spaces from start
					//!??
					sp = 0;
					while(acc.begin() + sp != acc.end() && isspace((acc.begin() + sp)->g)) sp++;

					if(sp > 0)
						acc.erase(acc.begin(), acc.begin() + sp);

					//move everything back
					if(!acc.empty()) {
						auto startx = acc.begin()->location;

						for(auto &e : acc)
							e.location -= startx;

						x -= startx;
					}
					else {
						x = 0;
					}

					autobreak=true;
					lastbreak = 0;
				}
				//if we wrapped, ignore spaces at start
				else if(!autobreak || ind != 0 || !isspace(g)) { 
					acc.push_back({x+cur_spacing, g});

					x += cur_spacing;
                    x += prev_gw;

					ind++;
				}
			}//for

			//last line
			if(autobreak) {
                //!??
				int sp = 0; //ignore spaces at the start
				while(acc.size() && isspace((acc.begin()+sp)->g)) sp++;

				if(acc.size())
					acc.erase(acc.begin(), acc.begin() + sp);
			}

			if(!acc.empty()) {
				doline(-1, acc.begin(), acc.end(), x);
			}
		}

	} //internal

	
	Geometry::Size BasicFont::GetSize(const std::string& text) const {
        if(renderer->NeedsPrepare())
            renderer->Prepare(text);
        
		auto cur = Geometry::Point(0, 0);

		int maxx = 0;

		internal::simpleprint(
			*renderer, text.begin(), text.end(),
			[&](Glyph prev, Glyph next) { return (int)renderer->KerningDistance(prev, next).X; },
			[&](Glyph g) { return (int)renderer->GetCursorAdvance(g);  },
			[&](Glyph g, int poff, float off) { cur.X += poff; cur.X += (int)off; },
			std::bind(&internal::dodefaulttab<int>, 0, std::ref(cur.X), renderer->GetMaxWidth() * 8),
			[&](Glyph) { cur.Y += (int)renderer->GetLineGap(); if(maxx < cur.X) maxx = cur.X; cur.X = 0; }
		);

		return{maxx, cur.Y};
	}
	
	Geometry::Size BasicFont::GetSize(const std::string& text, int w) const {
        if(renderer->NeedsPrepare())
            renderer->Prepare(text);
        
		auto y   = 0;
		auto tot = w;

		internal::boundedprint(
			*renderer, text.begin(), text.end(), tot,

			[&](Glyph, internal::markvecit begin, internal::markvecit end, int w) {			
				y += (int)renderer->GetLineGap();
			},

			[&](Glyph prev, Glyph next) { return (int)renderer->KerningDistance(prev, next).X; },
				[&](Glyph g) { return (int)renderer->GetCursorAdvance(g);  },
			std::bind(&internal::dodefaulttab<int>, 0, std::placeholders::_1, renderer->GetMaxWidth() * 8)
		);

		return {w, y};
	}
	
    void BasicFont::print(TextureTarget& target, const std::string& text, Geometry::Point location, RGBAf color) const {
        if(renderer->NeedsPrepare())
            renderer->Prepare(text);
        
		auto cur = location;
		
		internal::simpleprint(
			*renderer, text.begin(), text.end(),
			[&](Glyph prev, Glyph next) { return int(renderer->KerningDistance(prev, next).X); },
			[&](Glyph g) { return (int)renderer->GetCursorAdvance(g);  },
			[&](Glyph g, int poff, float off) { cur.X += poff; renderer->Render(g, target, cur, color); cur.X += (int)off; },
			std::bind(&internal::dodefaulttab<int>, location.X, std::ref(cur.X), renderer->GetMaxWidth() * 8),
			[&](Glyph) { cur.Y += (int)renderer->GetLineGap(); cur.X = location.X; }
		);
    }

    void BasicFont::print(TextureTarget &target, const std::string &text, Geometry::Rectangle location, TextAlignment align, RGBAf color) const {
        if(renderer->NeedsPrepare())
            renderer->Prepare(text);
        
		auto y   = location.Y;
		auto tot = location.Width;

		internal::boundedprint(
			*renderer, text.begin(), text.end(), tot,

			[&](Glyph, internal::markvecit begin, internal::markvecit end, int w) {
				auto off = location.X;

				if(align == TextAlignment::Center) {
					off += (int)std::round((tot - w) / 2.f);
				}
				else if(align == TextAlignment::Right) {
					off += tot - w;
				}

				for(auto it = begin; it != end; ++it) {
					renderer->Render(it->g, target, {(float)it->location + off, (float)y}, color);
				}

				y += (int)renderer->GetLineGap();
			},

			[&](Glyph prev, Glyph next) { return int(renderer->KerningDistance(prev, next).X); },
			[&](Glyph g) { return (int)renderer->GetCursorAdvance(g);  },
			std::bind(&internal::dodefaulttab<int>, 0, std::placeholders::_1, renderer->GetMaxWidth() * 8)
		);
	}

	void StyledRenderer::print(TextureTarget &target, const std::string &text, Geometry::Point location) const {
		if(shadow.type == TextShadow::Flat) {
			print(target, text, Geometry::Pointf(location) + shadow.offset, shadow.color, shadow.color, shadow.color);
		}

		print(target, text, location, color, strikecolor, underlinecolor);
	}

	void StyledRenderer::print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color, RGBAf strikecolor, RGBAf underlinecolor) const {
        if(renderer->NeedsPrepare())
            renderer->Prepare(text);
        
		//strike through, underline
		auto cur = location;

		if(strikecolor.R == -1)
			strikecolor = color;

		if(underlinecolor.R == -1)
			underlinecolor = color;

		internal::simpleprint(
			*renderer, text.begin(), text.end(),
			[&](Glyph prev, Glyph next) { return int(hspace + renderer->KerningDistance(prev, next).X); },
			[&](Glyph g) { return (int)renderer->GetCursorAdvance(g);  },
			[&](Glyph g, int poff, float off) { cur.X += poff; renderer->Render(g, target, cur, color); cur.X += (int)off; },
			std::bind(&internal::dodefaulttab<float>, location.X, std::ref(cur.X), (float)tabwidth),
			[&](Glyph) { 
				if(strike) {
					target.Draw(location.X, cur.Y + GetStrikePosition(), cur.X - location.X, (float)renderer->GetLineThickness(), strikecolor);
				}

				if(underline) {
					target.Draw(location.X, cur.Y + renderer->GetUnderlineOffset(), cur.X - location.X, (float)renderer->GetLineThickness(), underlinecolor);
				}

				cur.Y += (int)std::round(renderer->GetLineGap() * vspace + pspace);
				cur.X = location.X;
			}
		);

	}

	Geometry::Size StyledRenderer::GetSize(const std::string &text) const {
        if(renderer->NeedsPrepare())
            renderer->Prepare(text);
        
		auto cur = Geometry::Point(0, 0);

		int maxx = 0;

		internal::simpleprint(
			*renderer, text.begin(), text.end(),
			[&](Glyph prev, Glyph next) { return int(hspace + renderer->KerningDistance(prev, next).X); },
			[&](Glyph g) { return (int)renderer->GetCursorAdvance(g);  },
			[&](Glyph g, int poff, float off) { cur.X += poff; cur.X += (int)off; },
			std::bind(&internal::dodefaulttab<int>, 0, std::ref(cur.X), tabwidth),
			[&](Glyph) { cur.Y += (int)std::round(renderer->GetLineGap() * vspace + pspace); if(maxx < cur.X) maxx = cur.X; cur.X = 0; }
		);

		return{maxx, cur.Y};
	}
	
    Geometry::Size StyledRenderer::GetSize(const std::string &text, int width) const {
        if(renderer->NeedsPrepare())
            renderer->Prepare(text);
        
        auto y   = 0;
		auto tot = width;

		internal::boundedprint(
			*renderer, text.begin(), text.end(), tot,
			[&](Glyph, internal::markvecit begin, internal::markvecit end, int width) {			
				y += (int)std::round(renderer->GetLineGap() * vspace + pspace);
			},
			[&](Glyph prev, Glyph next) { return int(hspace + renderer->KerningDistance(prev, next).X); },
			[&](Glyph g) { return (int)renderer->GetCursorAdvance(g);  },
			std::bind(&internal::dodefaulttab<int>, 0, std::placeholders::_1, tabwidth)
		);

        // !!! TODO pick min(width, maxX)??
		return {width, y};
    }

	void StyledRenderer::print(TextureTarget &target, const std::string &text, Geometry::Rectangle location, TextAlignment align_override) const {
        if(renderer->NeedsPrepare())
            renderer->Prepare(text);
        
		if(shadow.type == TextShadow::Flat) {
			print(target, text, Geometry::Rectanglef(location) + shadow.offset, align_override, shadow.color, shadow.color, shadow.color);
		}

		print(target, text, location, align_override, color, strikecolor, underlinecolor);
	}

	void StyledRenderer::print(TextureTarget &target, const std::string &text, Geometry::Rectanglef location, TextAlignment align, RGBAf color, RGBAf strikecolor, RGBAf underlinecolor) const {
        if(renderer->NeedsPrepare())
            renderer->Prepare(text);
        
		auto y   = location.Y;
		int tot  = (int)location.Width;

		if(strikecolor.R == -1)
			strikecolor = color;

		if(underlinecolor.R == -1)
			underlinecolor = color;

		internal::boundedprint(
			*renderer, text.begin(), text.end(), tot,

			[&](Glyph g, internal::markvecit begin, internal::markvecit end, int w) {
				auto off = location.X;

				if(justify && g == 0) {
					//count spaces and letters
					int sps = 0;
					int letters = 0;
					Glyph prev = 0;

					for(auto it=begin; it!=end; ++it) {
						if(internal::isadjustablespace(it->g))
							sps++;

						//ignore before and after tabs
						if(it->g == '\t') {
							prev = 0;
						}
						else {
							if(prev && internal::isspaced(prev))
								letters++;

							prev = it->g;
						}
					}

					auto target = tot - w;
					int gs = 0; //glyph spacing
					int spsp = 0; //space spacing
					int extraspsp = 0; //extra spaced spaces

					if(letters && target/letters >= 1) { //we can increase glyph spacing
						gs = target/letters;
						if(gs > 1 && gs > renderer->GetHeight()/2) //1 is always usable
							gs = renderer->GetHeight()/2;

						target -= gs*letters;
					}

					if(sps > 0) {
						spsp = target/sps;

						//max 2em
						if(spsp > renderer->GetHeight() * 2) {
							spsp = renderer->GetHeight() * 2;
							target -= spsp*sps;
						}
						else {
							target -= spsp*sps;

							extraspsp = target;
							target = 0;
						}
					}

					if(target == 0) {
						//go over all glyphs and set widths
						int off = 0;
						for(auto it=begin; it!=end; ++it) {
							it->location += off;

							if(internal::isadjustablespace(it->g)) {
								off += spsp;

								if(extraspsp-->0)
									off++;
							}

							if(it->g != '\t' && internal::isspaced(it->g)) {
								off += gs;
							}
						}

						w = tot - target;
					}
				}

				if(align == TextAlignment::Center) {
					off += (int)std::round((tot - w) / 2.f);
				}
				else if(align == TextAlignment::Right) {
					off += tot - w;
				}

				for(auto it=begin; it!=end; ++it) {
					renderer->Render(it->g, target, {(float)it->location + off, (float)y}, color);
				}

				if(strike) {
					target.Draw((float)begin->location + off, y + GetStrikePosition(), (float)w, (float)renderer->GetLineThickness(), strikecolor);
				}

				if(underline) {
					target.Draw((float)begin->location + off, y + renderer->GetUnderlineOffset(), (float)w, (float)renderer->GetLineThickness(), underlinecolor);
				}

				y += (int)std::round(renderer->GetLineGap() * vspace);

				if(g != 0)
					y += pspace;
			},

			[&](Glyph prev, Glyph next) { return int(hspace + renderer->KerningDistance(prev, next).X); }, //modify this system to allow horizontal kerning
			[&](Glyph g) { return (int)renderer->GetCursorAdvance(g);  },
			std::bind(&internal::dodefaulttab<int>, 0, std::placeholders::_1, tabwidth)
		);
	}

} }
