#include "Font.h"
#include <Gorgon/Utils/Assert.h>

namespace Gorgon { namespace Graphics {


    void BasicFont::print(TextureTarget& target, const std::string& text, Geometry::Pointf location, RGBAf color) const { 
        auto cur = location;
		Glyph prev = 0;
        
        for(auto it=text.begin(); it!=text.end(); ++it) {
            Byte b = *it;
            Glyph g;
            
            if(b<127) { //ASCII
                g = b;
            }
            else {
                Utils::NotImplemented("Unicode support is not yet implemented.");
            }

			if(prev) {
				auto dist = renderer->KerningDistance(prev, g);
				cur.X += dist;
			}

			prev = g;

            if(g == '\t') {
                auto stops = renderer->GetMaxWidth() * 8;
                cur.X += stops;
                cur.X /= stops;
                cur.X = std::floor(cur.X);
                cur.X *= stops;
            }
            else if(g == '\n') {
                cur.X = location.X;
                cur.Y += renderer->GetLineHeight();
            }
            else if(g >= 32) {
                renderer->Render(g, target, cur, color);
                cur.X += renderer->GetSize(g).Width;
            }
        }
    }

    void BasicFont::print(TextureTarget &target, const std::string &text, Geometry::Rectanglef location, TextAlignment align, RGBAf color) const {
		auto cur = location.TopLeft();
		Glyph prev = 0;

		std::vector<Glyph> acc;
		std::vector<float> pos;
		int lastspace = 0;

		float dest = location.Right();

		auto donewline = [&](float diff) {
			int end = lastspace;


			if(align == TextAlignment::Center) {
				float m = (dest - pos[end]) / 2.f;

				// if floating point numbers are not used
				if(abs(pos[end]-(int)pos[end]) < 0.1)
					m=std::round(m); // make sure we land on full pixels

				for(int i=0; i<end; i++) {
					pos[i] += m;
				}
			}

			if(align == TextAlignment::Right) {
				float m = (dest - pos[end]);

				for(int i=0; i<end; i++) {
					pos[i] += m;
				}
			}

			for(int i=0; i<end; i++) {
				renderer->Render(acc[i], target, {pos[i], cur.Y}, color);
			}

			if(end+1 == pos.size()) {
				cur.X = location.X;
				acc.clear();
				pos.clear();
			}
			else {
				float shift;
				shift = pos[end+1];
				cur.X -= shift + diff;

				acc.erase(acc.begin(), acc.begin()+end+1);
				pos.erase(pos.begin(), pos.begin()+end+1);

				for(auto &p : pos)
					p -= shift;
			}

			lastspace = 0;

			cur.Y += renderer->GetLineHeight();
		};

		for(auto it=text.begin(); it!=text.end(); ++it) {
			Byte b = *it;
			Glyph g;

			bool nl = false;
			bool rollback = false;

			if(b<127) { //ASCII
				g = b;
			}
			else {
				Utils::NotImplemented("Unicode support is not yet implemented.");
			}

			if(prev) {
				auto dist = renderer->KerningDistance(prev, g);
				cur.X += dist;
			}

			float cx = cur.X;

			prev = g;

			if(g == '\t') {
				auto stops  = renderer->GetMaxWidth() * 8;
				cur.X += stops;
				cur.X /= stops;
				cur.X  = std::floor(cur.X);
				cur.X *= stops;
			}
			else if(g == '\n') {
				pos.push_back(cx);
				acc.push_back('\n');
				lastspace = acc.size() - 1;
				prev = 0;
				donewline(cur.X-cx);
				continue;
			}
			else if(g >= 32) {
				cur.X += renderer->GetSize(g).Width;
			}

			if(location.Width == 0 || cur.X < dest) {
				if(g == '\t' || g ==' ') {
					lastspace = acc.size();
				}

				pos.push_back(cx);
				acc.push_back(g);
			}
			else {
				auto diff = cur.X - cx;
				if(g == '\t' || g ==' ' || lastspace == 0) {
					pos.push_back(cx);
					acc.push_back(g);
					lastspace = acc.size() - 1;
				}

				donewline(diff);

				if(g != '\t' && g !=' ') {
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
        Geometry::Pointf cur = {0, 0};
        int maxx = 0;
        Glyph prev = 0;
       
        auto lh = renderer->GetLineHeight();
        
        for(auto it=text.begin(); it!=text.end(); ++it) {
            Byte b = *it;
            Glyph g;
            
            if(b<127) { //ASCII
                g = b;
            }
            else {
                Utils::NotImplemented("Unicode support is not yet implemented.");
            }

			if(prev) {
				auto dist = renderer->KerningDistance(prev, g);
				cur.X += dist;
			}

			prev = g;
            
            if(g == '\t') {
                auto stops = renderer->GetMaxWidth() * 8;
                cur.X += stops;
                cur.X /= stops;
                cur.X = std::floor(cur.X);
                cur.X *= stops;
            }
            else if(g == '\n') {
                if(maxx < cur.X) maxx = (int)std::round(cur.X);
                
                cur.X = 0;
                cur.Y += lh;
            }
            else if(g >= 32) {
                cur.X += renderer->GetSize(g).Width;
            }
        }
        
        if(maxx < cur.X) maxx = (int)std::round(cur.X);
        
        return {maxx, (int)std::round(cur.Y + lh)};
    }

} }
