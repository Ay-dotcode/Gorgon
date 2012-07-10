//DESCRIPTION
//	This file contains string tokenizer

//REQUIRES:
//	Utils/Iterator

//LICENSE
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the Lesser GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	Lesser GNU General Public License for more details.
//
//	You should have received a copy of the Lesser GNU General Public License
//	along with this program. If not, see < http://www.gnu.org/licenses/ >.

//COPYRIGHT
//	Cem Kalyoncu, DarkGaze.Org (cemkalyoncu[at]gmail[dot]com)

#pragma once

#include <string>
#include "Iterator.h"


namespace gge { namespace utils {

	class StringTokenizer {
	public:
		StringTokenizer() : position(Text.npos) {}

		StringTokenizer(const std::string &Text, const std::string &Delimeters) : Text(Text), Delimeters(Delimeters) {
			position=0;

			auto ind=Text.find_first_of(Delimeters);
			if(ind==Text.npos) {
				Token=Text;
				position=Text.length();
			}
			else {
				Token=Text.substr(position, ind);
				position=ind+1;
			}
		}

		void operator ++() {
			Next();
		}

		void Next() {
			if(position==Text.length())
				position=Text.npos;

			if(position==Text.npos)
				return;

			auto ind=Text.find_first_of(Delimeters, position);
			if(ind==Text.npos) {
				Token=Text.substr(position);
				position=Text.length();
			}
			else {
				Token=Text.substr(position, ind-position);
				position=ind+1;
			}
		}

		std::string Current() const {
			return Token;
		}

		std::string operator *() const {
			return Token;
		}

		operator std::string() const {
			return Token;
		}

		const std::string *operator ->() const {
			return &Token;
		}

		bool IsValid() const {
			return position!=Text.npos;
		}

		bool operator ==(const StringTokenizer &st) const {
			return st.position==position;
		}

		std::string Delimeters;
		std::string Text;
		std::string Token;

	protected:
		unsigned int position;
	};

}}