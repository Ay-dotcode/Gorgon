#pragma once
#include <cstring>
#include <cstdlib>

#define ANY_EXISTS
#ifndef NULL
#define NULL	0
#endif

namespace gge {
	class Any {
	public:
		Any() : content(NULL),size(0) { }

		Any(Any& any) {
			if(any.content) {
				size=any.size;
				content=malloc(size);
				std::memcpy(content, any.content, size);
			} else {
				content=NULL;
				size=0;
			}

		}

		template <class T_>
		Any(T_ data) {
			size=sizeof(T_);
			content=std::malloc(size);
			*((T_*)content)=data;
		}

		template <class T_>
		operator T_ &() {
			return *((T_*)content);
		}

		operator Any &() {
			return *this;
		}
	
		Any &operator =(Any &any) {
			size=any.size;
			content=malloc(size);
			std::memcpy(content, any.content, size);

			return *this;
		}

		template <class T_>
		bool operator ==(T_ &content) {
			return *(T_*)this->content==content;
		}

		template <class T_>
		bool operator !=(T_ &content) {
			return *content!=content;
		}

		bool isSet() {
			return content!=NULL;
		}
		
		template <class T_>
		void SetData(T_ data) {
			size=sizeof(T_);
			content=std::malloc(size);
			*((T_*)content)=data;
		}

		void clear() {
			size=0;
			content=NULL;
		}

		~Any() {
			if(content)
				delete content;
		}

	protected:
		void *content;
		int size;
	};
}
