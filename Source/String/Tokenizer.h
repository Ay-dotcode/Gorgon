/// @file Tokenizer.h contains string tokenizer.

#pragma once

#include <string>
#include <iterator>


namespace Gorgon { namespace String {

	/// Tokenizer is a forward iterator that tokenizes a given string. This class only
	/// supports single character delimeters, however, its possible to specify more than
	/// one delimeter. @see Extract function to tokenize a string using multicharacter
	/// delimeters. Its also possible to change delimeters in the middle of tokenization.
	class Tokenizer : public std::iterator<std::forward_iterator_tag, std::string> {
	public:
		
		/// Creates an empty tokenizer. Effectively creates an end iterator.
		Tokenizer() : position(text.npos) {}

		/// Creates a new tokenizer.
		/// @param  str string to be tokenized
		/// @param  delimeters is the delimeters to use while tokenizing
		Tokenizer(const std::string &str, const std::string &delimeters) : 
		      text(str), Delimeters(delimeters) {
			position=0;

			Next();
		}

		/// Move to the next token
		void operator ++() {
			Next();
		}

		/// Move to the next token
		void Next() {
			if(position==text.length())
				position=text.npos;

			if(position==text.npos)
				return;

			auto ind=text.find_first_of(Delimeters, position);
			if(ind==text.npos) {
				token=text.substr(position);
				position=text.length();
			}
			else {
				token=text.substr(position, ind-position);
				position=ind+1;
			}
		}

		/// Return the current token
		std::string Current() const {
			return token;
		}

		/// Return the current token
		std::string operator *() const {
			return token;
		}

		/// Return the current token
		operator std::string() const {
			return token;
		}

		/// Return the current token
		const std::string *operator ->() const {
			return &token;
		}

		/// Whether the iterator is valid (i.e. dereferencable)
		bool IsValid() const {
			return position!=text.npos;
		}

		/// Compare two iterators, does not check if two iterators are identical. Compares
		/// only current positions and tokens
		bool operator ==(const Tokenizer &st) const {
			return st.position==position && st.token == token;
		}

		/// Delimeters to be used in tokenization. Can be changed while tokenizing.
		std::string Delimeters;

	protected:
		/// Current text
		std::string text;
		/// Current token
		std::string token;
		/// Position of the next token. std::string::npos denotes the iterator reached the end.
		std::string::size_t position;
	};

}}