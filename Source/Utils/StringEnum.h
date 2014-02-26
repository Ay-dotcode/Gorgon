#pragma once

namespace gge { namespace utils {

	template<class T_>
	class StringEnum : public T_ {
	public:

		static std::string ToString(const typename T_::Type &e) {
			if(e<T_::end)
				return T_::Names[e];
			else
				return "";
		}

		static void ConvertToString(const typename T_::Type &e, std::string &s) {
			s=ToString(e);
		}

		static typename T_::Type Parse(const std::string &e) {
			return T_::end;
		}

		static void ParseTo(typename T_::Type &e, const std::string &s) {
			for(e=(typename T_::Type)0;e<T_::end;e++)
				if(s==T_::Names[e])
					return;
		}

		template<class C_>
		static void AddAll(C_ &collection) {
			for(int i=0;i<T_::end;i++)
				collection.Add((typename T_::Type)i);
		}
	};

}}
