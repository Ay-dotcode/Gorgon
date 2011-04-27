#pragma once

namespace gge {
	inline float  RandomFloat() { return ((float)rand())/RAND_MAX; }
	template <class T_>
	T_ Random(T_ useless=T_()) { return ((T_)rand())/RAND_MAX; }
	template <>
	inline double Random(double useless) { return (double)Random<float>()*Random<float>(); }
	template <>
	inline int Random(int useless) { return rand()+(rand()<<16); }
	template <>
	inline unsigned char Random(unsigned char useless) { return rand()%256; }


	inline int    Random(int min, int max) {     return (rand()%(max-min)) + min; }

	template<class T_>
	T_ Random(T_ min, T_ max) { return Random<T_>()*(max-min) + min; }
	template<>
	inline unsigned char Random(unsigned char min, unsigned char max) {     return (rand()%(max-min)) + min; }
	template<>
	inline int Random(int min, int max) {    return (rand()%(max-min)) + min; }
	template<>
	inline long Random(long min, long max) { return (rand()%(max-min)) + min; }
}
