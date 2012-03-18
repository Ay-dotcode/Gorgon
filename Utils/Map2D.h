//DESCRIPTION
//	This file contains Map2D. A fixed sized or resizable map that can contain 
//	any type. 
//	
//	It has the ability to check for sequential elements. This can be
//	done row wise, column wise, and diagonals. It can return either first element
//	or a vector of encounters. Also some elements can be ignored or a specific
//	element can be checked. These functions are useful in the following games:
//	* Tetris (Both original and color match)
//	* Match 3
//	* Connect 4
//	
//	The map itself can be used for any 2D map purposes, Including imaging. Fixed
//	sized variant uses stack for storage. It is not well suited for larger maps.
//	Also, every time it is passed by value, entire array is copied. For move
//	semantics and heap data allocation use variable sized.
//	Fixed sized:    Map2D<Player, 3, 3> Board
//	Variable sized: Map2D<Player> Board(3,3), you can also use Resize method.
//	Resize method does not properly keep the data. Assigning a variable sized
//	map to another map will link them. To create a duplicate use Duplicate method.

//REQUIRES:
//	gge::utils::ManagedBuffer
//	gge::utils::Size2D

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

#pragma warning(push)
#pragma warning(disable:4996)

#include <algorithm>
#include <stdexcept>

#include "Size2D.h"
#include "ManagedBuffer.h"
#include <iosfwd>
#include <ostream>
#include "Random.h"
#include <vector>


//Remaining: Find all sequential, use alternative to add to a vector instead of creating new
namespace gge { namespace utils {

	template <class Map_, class T_> class mapoperations;

	//Fills maps using << operator.
	template <class Map_, class T_>
	class mapfiller {
		friend static mapfiller<Map_, T_> operator << <Map_, T_>(mapoperations<Map_, T_> &map, const T_ &value);
	public:
		mapfiller &operator <<(const T_ &value) {
			if(offset>=parent.buffersize())
				std::runtime_error("Index out of bounds");

			parent[offset++]=value;

			return *this;
		}

	protected:
		mapfiller(Map_&parent) : parent(parent), offset(0) {

		}
	private:
		int offset;
		Map_ &parent;
	};

	//returned by CheckXXXSequence functions
	template <class T_>
	struct SequenceCheckResult {
		enum directiontype {
			None,
			Horizontal,
			Vertical,
			//*..
			//.*.
			//..*
			ForwardDiagonal, 
			//..*
			//.*.
			//*..
			BackwardDiagonal 
		};
		
		SequenceCheckResult(int x, int y, int amount, const T_ &value, directiontype direction) : location(x,y), 
			amount(amount), value(value), direction(direction) {

		}

		SequenceCheckResult(Point location, int amount, const T_ &value, directiontype direction) : location(location), 
			amount(amount), value(value), direction(direction) {

		}

		SequenceCheckResult() : location(0,0), amount(0), value(T_()), direction(None) {

		}

		Point location;
		int amount;
		T_ value;

		directiontype direction;
	};
	
	//for sorting
	template<class T_>
	bool operator <(const SequenceCheckResult<T_> &seq1, const SequenceCheckResult<T_> &seq2)  {
		return seq1.amount<seq2.amount;
	}

	//for sorting
	template<class T_>
	bool operator >(const SequenceCheckResult<T_> &seq1, const SequenceCheckResult<T_> &seq2)  {
		return seq1.amount>seq2.amount;
	}

	//for inverse sorting you can use these functions
	namespace SequenceSort {
		template<class T_>
		bool less(const SequenceCheckResult<T_> &seq1, const SequenceCheckResult<T_> &seq2)  {
			return seq1<seq2;
		}
		template<class T_>
		bool greater(const SequenceCheckResult<T_> &seq1, const SequenceCheckResult<T_> &seq2)  {
			return seq1>seq2;
		}
	}


	//This class is inherited by maps for functionality
	template <class Map_, class T_>
	class mapoperations {
		friend class mapfiller<Map_, T_>;
	public:

		//Returns the specified member. Fires an exception in debug, no checks are
		// performed in release. You may use this function for assignment as well
		// map(2,4)=Player_X;
		T_ &operator ()(int x, int y) {
#ifdef _DEBUG
			if(x<0 || x>=me().getwidth() || y<0 || y>=me().getheight())
				throw std::runtime_error("Index out of bounds");
#endif

			return me().buffer[x + y*me().getwidth()];
		}
		//Returns the specified member. Fires an exception in debug, no checks are
		// performed in release. You may use this function for assignment as well
		// map(2,4)=Player_X;
		T_ &operator ()(utils::Point location) {
			return (*this)(location.x, location.y);
		}

		T_ &operator()(int location) {
			return me().buffer[location];
		}

		//Returns the specified member. Fires an exception in debug, no checks are
		// performed in release
		const T_ &operator ()(int x, int y) const {
#ifdef _DEBUG
			if(x<0 || x>=me().getwidth() || y<0 || y>=me().getheight())
				throw std::runtime_error("Index out of bounds");
#endif

			return me().buffer[x + y*me().getwidth()];
		}

		const T_ &operator ()(utils::Point location) const {
			return (*this)(location.x, location.y);
		}

		const T_ &operator()(int location) const {
			return me().buffer[location];
		}

		//Changes the value of the specified element. If the element is not found
		// nothing will be done.
		void Set(int x, int y, const T_ &value) {
			if(!(x<0 || x>=me().getwidth() || y<0 || y>=me().getheight()))
				me().buffer[x+y*me().getwidth()]=value;
		}
		void Set(utils::Point location, const T_ &value) {
			Set(location.x, location.y,value)
		}
		void Set(int location, const T_ &value) {
			if(location>=0 && location<me().buffer.GetSize())
				me().buffer[location]=value;
		}

		//Returns the value at the given cell. If the element is not found
		// a new object will be returned. This function returns by value.
		T_ Get(int x, int y) const {
			if(!(x<0 || x>=me().getwidth() || y<0 || y>=me().getheight()))
				return me().buffer[x+y*me().getwidth()];

			return T_();
		}

		//Returns the value at the given cell. If the element is not found
		// a new object will be returned. This function returns by value.
		T_ Get(utils::Point location) const {
			return Get(location.x, location.y);
		}
		T_ Get(int location) const {
			if(location>=0 && location<me().buffer.GetSize())
				return me().buffer[location];
		}

		//Returns the size of the map
		Size GetSize() const {
			return Size(me().getwidth(), me().getheight());
		}

		//Returns the width of the map
		int GetHeight() const { return me().getheight(); }
		//Returns the height of the map
		int GetWidth() const { return me().getwidth(); }

		//Clears the map with the given value
		void Clear(const T_ &value) {
			std::fill(me().buffer.begin(),me().buffer.end(), value);
		}

		//Clears the map with the default value
		void Clear() {
			Clear(T_());
		}

		//Clears the given row of the map with the given value
		void ClearRow(int row, const T_ &value) {
#ifdef _DEBUG
			if(row<0 || row>=me().getheight())
				throw std::runtime_error("Given row is out of bounds");
#endif
			std::fill(me().buffer.begin()+row*me().getwidth(),
				me().buffer.begin()+(row+1)*me().getwidth(), value);
		}

		//Clears the given row of the map with the default value
		void ClearRow(int row) {
			ClearRow(row, T_());
		}

		//Clears the given column of the map with the given value
		void ClearColumn(int column, const T_ &value) {
#ifdef _DEBUG
			if(column<0 || column>=me().getwidth())
				throw std::runtime_error("Given column is out of bounds");
#endif

			for(T_ *it=me().buffer.begin()+column;it<me().buffer.end();it+=me().getwidth()) {
				*it=value;
			}
		}

		//Clears the given column of the map with the default value
		void ClearColumn(int column) {
			ClearColumn(column, T_());
		}

		// shuffle the tiles
		// it's achieved by randomly swapping each cell with another
		void Shuffle() {
			int w=me().getwidth();
			int h=me().getheight();
			for(int y=0;y<h;y++) {
				for(int x=0;x<w;x++) {
					int nx=gge::utils::Random(0,w);
					int ny=gge::utils::Random(0,h);

					// swap
					T_ t=Get(x,y);
					Set(x,y,Get(nx,ny));
					Set(nx,ny,t);
				}
			}

		}

		//Check neighbors of the given cell for a given value, returns the count of the encountered value
		int CheckNeighborsFor(int x, int y, const T_ &v, int rangex=1, int rangey=1) const {
			int w=me().getwidth();
			int h=me().getheight();
			int count=0;
			for(int yy=-rangey;yy<=rangey;yy++) {
				for(int xx=-rangex;xx<=rangex;xx++) {
					if(x+xx>=0 && x+xx<w && y+yy>=0 && y+yy<h && Get(x+xx,y+yy)==v) {
						count++;
					}
				}
			}

			return count;
		}

		//Check neighbors of the given cell for a given value, returns the count of the encountered value
		template <class F_>
		int CheckNeighborsUsing(int x, int y, F_ f, int rangex=1, int rangey=1) const {
			int w=me().getwidth();
			int h=me().getheight();
			int count=0;
			for(int yy=-rangey;yy<=rangey;yy++) {
				for(int xx=-rangex;xx<=rangex;xx++) {
					if(x+xx>=0 && x+xx<w && y+yy>=0 && y+yy<h && f(Get(x+xx,y+yy))) {
						count++;
					}
				}
			}

			return count;
		}

		void SetFirstN(int n, const T_ &v) {
			for(int i=0;i<n;i++)
				Set(i, v);
		}

		SequenceCheckResult<T_> CheckRowsForSequence(int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int y=0;y<h;y++) {
				for(int x=0;x<1+w-atleast;x++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					for(i=x+1;i<w;i++) {
						if(Get(i,y)==last)
							seq++;
						else
							break;
					}
					if(seq>=atleast) {
						return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Horizontal);
					}
					x=i-1;
				}
			}

			return SequenceCheckResult<T_>();
		}

		SequenceCheckResult<T_> CheckRowsForSequence() const {
			return CheckRowsForSequence(me().getwidth());
		}

		SequenceCheckResult<T_> CheckRowsForSequenceOf(const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int y=0;y<h;y++) {
				for(int x=0;x<1+w-atleast;x++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last==value) {
						for(i=x+1;i<w;i++) {
							if(Get(i,y)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Horizontal);
						}
						x=i-1;
					}
				}
			}

			return SequenceCheckResult<T_>();
		}

		SequenceCheckResult<T_> CheckRowsForSequenceOf(const T_ &value) const {
			return CheckRowsForSequence(value, me().getwidth());
		}

		SequenceCheckResult<T_> CheckRowsForSequenceNotOf(const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int y=0;y<h;y++) {
				for(int x=0;x<1+w-atleast;x++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last!=value) {
						for(i=x+1;i<w;i++) {
							if(Get(i,y)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Horizontal);
						}
						x=i-1;
					}
				}
			}

			return SequenceCheckResult<T_>();
		}

		SequenceCheckResult<T_> CheckRowsForSequenceNotOf(const T_ &value) const {
			return CheckRowsForSequence(value, me().getwidth());
		}

		SequenceCheckResult<T_> CheckColumnsForSequence(int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int x=0;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					for(i=y+1;i<h;i++) {
						if(Get(x,i)==last)
							seq++;
						else
							break;
					}
					if(seq>=atleast) {
						return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Vertical);
					}
					y=i-1;
				}
			}

			return SequenceCheckResult<T_>();
		}

		SequenceCheckResult<T_> CheckColumnsForSequence() const {
			return CheckColumnsForSequence(me().getheight());
		}

		SequenceCheckResult<T_> CheckColumnsForSequenceOf(const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int x=0;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last==value) {
						for(i=y+1;i<h;i++) {
							if(Get(x,i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Vertical);
						}
						y=i-1;
					}
				}
			}

			return SequenceCheckResult<T_>();
		}

		SequenceCheckResult<T_> CheckColumnsForSequenceOf(const T_ &value) const {
			return CheckColumnsForSequence(value, me().getheight());
		}

		SequenceCheckResult<T_> CheckColumnsForSequenceNotOf(const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int x=0;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last!=value) {
						for(i=y+1;i<h;i++) {
							if(Get(x,i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Vertical);
						}
						y=i-1;
					}
				}
			}

			return SequenceCheckResult<T_>();
		}

		SequenceCheckResult<T_> CheckColumnsForSequenceNotOf(const T_ &value) const {
			return CheckColumnsForSequence(value, me().getheight());
		}

		SequenceCheckResult<T_> CheckDiagonalsForSequence(int atleast) const {
			int w=me().getwidth(), h=me().getheight();

			//forward diagonal
			for(int x=0;x<1+w-atleast;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;

					seq=1;
					last=Get(x,y);
					for(i=1;i<std::min(w-x,h-y);i++) {//will create problems for multiple
						if(Get(x+i,y+i)==last)
							seq++;
						else
							break;
					}
					if(seq>=atleast) {
						return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::ForwardDiagonal);
					}
				}
			}

			//backward diagonal
			for(int x=atleast-1;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;
					seq=1;
					last=Get(x,y);
					for(i=1;i<std::min(x+1,h-y);i++) {//will create problems for multiple
						if(Get(x-i,y+i)==last)
							seq++;
						else
							break;
					}
					if(seq>=atleast) {
						return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::BackwardDiagonal);
					}
				}
			}

			return SequenceCheckResult<T_>();
		}

		SequenceCheckResult<T_> CheckDiagonalsForSequence() const {
			return CheckDiagonalsForSequence(std::min(me().getwidth(), me().getheight()));
		}

		SequenceCheckResult<T_> CheckDiagonalsForSequenceOf(const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();

			//forward diagonal
			for(int x=0;x<1+w-atleast;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;

					seq=1;
					last=Get(x,y);
					if(last==value) {
						for(i=1;i<std::min(w-x,h-y);i++) {//will create problems for multiple
							if(Get(x+i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::ForwardDiagonal);
						}
					}
				}
			}

			//backward diagonal
			for(int x=atleast-1;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;
					seq=1;
					last=Get(x,y);
					if(last==value) {
						for(i=1;i<std::min(x+1,h-y);i++) {//will create problems for multiple
							if(Get(x-i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::BackwardDiagonal);
						}
					}
				}
			}

			return SequenceCheckResult<T_>();
		}

		SequenceCheckResult<T_> CheckDiagonalsForSequenceOf(const T_ &value) const {
			return CheckDiagonalsForSequenceOf(value, std::min(me().getwidth(), me().getheight()));
		}

		SequenceCheckResult<T_> CheckDiagonalsForSequenceNotOf(const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();

			//forward diagonal
			for(int x=0;x<1+w-atleast;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;

					seq=1;
					last=Get(x,y);
					if(last!=value) {
						for(i=1;i<std::min(w-x,h-y);i++) {//will create problems for multiple
							if(Get(x+i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::ForwardDiagonal);
						}
					}
				}
			}

			//backward diagonal
			for(int x=atleast-1;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;
					seq=1;
					last=Get(x,y);
					if(last!=value) {

						for(i=1;i<std::min(x+1,h-y);i++) {//will create problems for multiple
							if(Get(x-i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							return SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::BackwardDiagonal);
						}
					}
				}
			}

			return SequenceCheckResult<T_>();
		}

		SequenceCheckResult<T_> CheckDiagonalsForSequenceNotOf(const T_ &value) const {
			return CheckDiagonalsForSequenceNotOf(value, std::min(me().getwidth(), me().getheight()));
		}

		SequenceCheckResult<T_> CheckAllForSequence(int atleast) const {
			SequenceCheckResult<T_> ret=CheckRowsForSequence(atleast);
			if(ret.direction!=SequenceCheckResult<T_>::None) return ret;

			ret=CheckColumnsForSequence(atleast);
			if(ret.direction!=SequenceCheckResult<T_>::None) return ret;

			ret=CheckDiagonalsForSequence(atleast);
			if(ret.direction!=SequenceCheckResult<T_>::None) return ret;
		}

		SequenceCheckResult<T_> CheckAllForSequenceOf(const T_ &value, int atleast) const {
			SequenceCheckResult<T_> ret=CheckRowsForSequenceOf(value, atleast);
			if(ret.direction!=SequenceCheckResult<T_>::None) return ret;

			ret=CheckColumnsForSequenceOf(value, atleast);
			if(ret.direction!=SequenceCheckResult<T_>::None) return ret;

			ret=CheckDiagonalsForSequenceOf(value, atleast);
			return ret;
		}

		SequenceCheckResult<T_> CheckAllForSequenceNotOf(const T_ &value, int atleast) const {
			SequenceCheckResult<T_> ret=CheckRowsForSequenceNotOf(value, atleast);
			if(ret.direction!=SequenceCheckResult<T_>None) return ret;

			ret=CheckColumnsForSequenceNotOf(value, atleast);
			if(ret.direction!=SequenceCheckResult<T_>None) return ret;

			ret=CheckDiagonalsForSequenceNotOf(value, atleast);
			if(ret.direction!=SequenceCheckResult<T_>None) return ret;
		}

		SequenceCheckResult<T_> CheckAllForSequence() const {
			return CheckAllForSequence(std::min(me().getwidth(), me().getheight()));
		}

		SequenceCheckResult<T_> CheckAllForSequenceOf(const T_ &value) const {
			return CheckAllForSequenceOf(value, std::min(me().getwidth(), me().getheight()));
		}

		SequenceCheckResult<T_> CheckAllForSequenceNotOf(const T_ &value) const {
			return CheckAllForSequenceNotOf(value, std::min(me().getwidth(), me().getheight()));
		}



		std::vector<SequenceCheckResult<T_> > CheckRowsForSequences(int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			std::vector<SequenceCheckResult<T_> > ret;

			for(int y=0;y<h;y++) {
				for(int x=0;x<1+w-atleast;x++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					for(i=x+1;i<w;i++) {
						if(Get(i,y)==last)
							seq++;
						else
							break;
					}
					if(seq>=atleast) {
						ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Horizontal));
					}
					x=i-1;
				}
			}

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckRowsForSequences() const {
			return CheckRowsForSequences(me().getwidth());
		}

		std::vector<SequenceCheckResult<T_> > CheckRowsForSequencesOf(const T_ &value, int atleast) const {
			std::vector<SequenceCheckResult<T_> > ret;

			int w=me().getwidth(), h=me().getheight();
			for(int y=0;y<h;y++) {
				for(int x=0;x<1+w-atleast;x++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last==value) {
						for(i=x+1;i<w;i++) {
							if(Get(i,y)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Horizontal));
						}
						x=i-1;
					}
				}
			}

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckRowsForSequencesOf(const T_ &value) const {
			return CheckRowsForSequences(value, me().getwidth());
		}

		std::vector<SequenceCheckResult<T_> > CheckRowsForSequencesNotOf(const T_ &value, int atleast) const {
			std::vector<SequenceCheckResult<T_> > ret;

			int w=me().getwidth(), h=me().getheight();
			for(int y=0;y<h;y++) {
				for(int x=0;x<1+w-atleast;x++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last!=value) {
						for(i=x+1;i<w;i++) {
							if(Get(i,y)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Horizontal));
						}
						x=i-1;
					}
				}
			}

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckRowsForSequencesNotOf(const T_ &value) const {
			return CheckRowsForSequence(value, me().getwidth());
		}

		std::vector<SequenceCheckResult<T_> > CheckColumnsForSequences(int atleast) const {
			std::vector<SequenceCheckResult<T_> > ret;

			int w=me().getwidth(), h=me().getheight();
			for(int x=0;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					for(i=y+1;i<h;i++) {
						if(Get(x,i)==last)
							seq++;
						else
							break;
					}
					if(seq>=atleast) {
						ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Vertical));
					}
					y=i-1;
				}
			}

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckColumnsForSequences() const {
			return CheckColumnsForSequences(me().getheight());
		}

		std::vector<SequenceCheckResult<T_> > CheckColumnsForSequencesOf(const T_ &value, int atleast) const {
			std::vector<SequenceCheckResult<T_> > ret;
			int w=me().getwidth(), h=me().getheight();
			for(int x=0;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last==value) {
						for(i=y+1;i<h;i++) {
							if(Get(x,i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Vertical));
						}
						y=i-1;
					}
				}
			}

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckColumnsForSequencesOf(const T_ &value) const {
			return CheckColumnsForSequence(value, me().getheight());
		}

		std::vector<SequenceCheckResult<T_> > CheckColumnsForSequencesNotOf(const T_ &value, int atleast) const {
			std::vector<SequenceCheckResult<T_> > ret;
			int w=me().getwidth(), h=me().getheight();
			for(int x=0;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last!=value) {
						for(i=y+1;i<h;i++) {
							if(Get(x,i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Vertical));
						}
						y=i-1;
					}
				}
			}

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckColumnsForSequencesNotOf(const T_ &value) const {
			return CheckColumnsForSequences(value, me().getheight());
		}

		std::vector<SequenceCheckResult<T_> > CheckDiagonalsForSequences(int atleast) const {
			std::vector<SequenceCheckResult<T_> > ret;
			int w=me().getwidth(), h=me().getheight();

			//forward diagonal
			for(int x=0;x<1+w-atleast;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;

					seq=1;
					last=Get(x,y);
					if( !(x>0 && y>0 && last==Get(x-1, y-1)) ) {
						for(i=1;i<std::min(w-x,h-y);i++) {
							if(Get(x+i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::ForwardDiagonal));
						}
					}
				}
			}

			//backward diagonal
			for(int x=atleast-1;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;
					seq=1;
					last=Get(x,y);
					if( !(x<w-1 && y>0 && last==Get(x+1, y-1)) ) {
						for(i=1;i<std::min(x+1,h-y);i++) {//will create problems for multiple
							if(Get(x-i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::BackwardDiagonal));
						}
					}
				}
			}

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckDiagonalsForSequences() const {
			return CheckDiagonalsForSequences(std::min(me().getwidth(), me().getheight()));
		}

		std::vector<SequenceCheckResult<T_> > CheckDiagonalsForSequencesOf(const T_ &value, int atleast) const {
			std::vector<SequenceCheckResult<T_> > ret;
			int w=me().getwidth(), h=me().getheight();

			//forward diagonal
			for(int x=0;x<1+w-atleast;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;

					seq=1;
					last=Get(x,y);
					if( last==value && !(x>0 && y>0 && last==Get(x-1, y-1)) ) {
						for(i=1;i<std::min(w-x,h-y);i++) {
							if(Get(x+i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::ForwardDiagonal));
						}
					}
				}
			}

			//backward diagonal
			for(int x=atleast-1;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;
					seq=1;
					last=Get(x,y);
					if( last==value && !(x<w-1 && y>0 && last==Get(x+1, y-1)) ) {
						for(i=1;i<std::min(x+1,h-y);i++) {//will create problems for multiple
							if(Get(x-i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::BackwardDiagonal));
						}
					}
				}
			}

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckDiagonalsForSequencesOf(const T_ &value) const {
			return CheckDiagonalsForSequencesOf(value, std::min(me().getwidth(), me().getheight()));
		}

		std::vector<SequenceCheckResult<T_> > CheckDiagonalsForSequencesNotOf(const T_ &value, int atleast) const {
			std::vector<SequenceCheckResult<T_> > ret;
			int w=me().getwidth(), h=me().getheight();

			//forward diagonal
			for(int x=0;x<1+w-atleast;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;

					seq=1;
					last=Get(x,y);
					if( last!=value && !(x>0 && y>0 && last==Get(x-1, y-1)) ) {
						for(i=1;i<std::min(w-x,h-y);i++) {
							if(Get(x+i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::ForwardDiagonal));
						}
					}
				}
			}

			//backward diagonal
			for(int x=atleast-1;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;
					seq=1;
					last=Get(x,y);
					if( last!=value && !(x<w-1 && y>0 && last==Get(x+1, y-1)) ) {
						for(i=1;i<std::min(x+1,h-y);i++) {//will create problems for multiple
							if(Get(x-i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							ret.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::BackwardDiagonal));
						}
					}
				}
			}

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckDiagonalsForSequencesNotOf(const T_ &value) const {
			return CheckDiagonalsForSequencesNotOf(value, std::min(me().getwidth(), me().getheight()));
		}

		std::vector<SequenceCheckResult<T_> > CheckAllForSequences(int atleast) const {
			std::vector<SequenceCheckResult<T_> > &&ret=CheckRowsForSequences(atleast);
			CheckColumnsForSequences(ret,atleast);
			CheckDiagonalsForSequences(ret,atleast);

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckAllForSequencesOf(const T_ &value, int atleast) const {
			std::vector<SequenceCheckResult<T_> > &&ret=CheckRowsForSequencesOf(value, atleast);
			CheckColumnsForSequencesOf(ret, value, atleast);
			CheckDiagonalsForSequencesOf(ret, value, atleast);

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckAllForSequencesNotOf(const T_ &value, int atleast) const {
			std::vector<SequenceCheckResult<T_> > &&ret=CheckRowsForSequencesNotOf(value, atleast);
			CheckColumnsForSequencesNotOf(ret, value, atleast);
			CheckDiagonalsForSequencesNotOf(ret, value, atleast);

			return ret;
		}

		std::vector<SequenceCheckResult<T_> > CheckAllForSequences() const {
			return CheckAllForSequences(std::min(me().getwidth(), me().getheight()));
		}

		std::vector<SequenceCheckResult<T_> > CheckAllForSequencesOf(const T_ &value) const {
			return CheckAllForSequencesOf(value, std::min(me().getwidth(), me().getheight()));
		}

		std::vector<SequenceCheckResult<T_> > CheckAllForSequencesNotOf(const T_ &value) const {
			return CheckAllForSequencesNotOf(value, std::min(me().getwidth(), me().getheight()));
		}
		


		void CheckRowsForSequences(std::vector<SequenceCheckResult<T_> > &vec, int atleast) const {
			int w=me().getwidth(), h=me().getheight();

			for(int y=0;y<h;y++) {
				for(int x=0;x<1+w-atleast;x++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					for(i=x+1;i<w;i++) {
						if(Get(i,y)==last)
							seq++;
						else
							break;
					}
					if(seq>=atleast) {
						vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Horizontal));
					}
					x=i-1;
				}
			}
		}

		void CheckRowsForSequences(std::vector<SequenceCheckResult<T_> > &vec) const {
			CheckRowsForSequences(vec, me().getwidth());
		}

		void CheckRowsForSequencesOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int y=0;y<h;y++) {
				for(int x=0;x<1+w-atleast;x++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last==value) {
						for(i=x+1;i<w;i++) {
							if(Get(i,y)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Horizontal));
						}
						x=i-1;
					}
				}
			}
		}

		void CheckRowsForSequencesOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value) const {
			CheckRowsForSequences(vec, value, me().getwidth());
		}

		void CheckRowsForSequencesNotOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int y=0;y<h;y++) {
				for(int x=0;x<1+w-atleast;x++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last!=value) {
						for(i=x+1;i<w;i++) {
							if(Get(i,y)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Horizontal));
						}
						x=i-1;
					}
				}
			}
		}

		void CheckRowsForSequencesNotOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value) const {
			CheckRowsForSequence(vec, value, me().getwidth());
		}

		void CheckColumnsForSequences(std::vector<SequenceCheckResult<T_> > &vec, int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int x=0;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					for(i=y+1;i<h;i++) {
						if(Get(x,i)==last)
							seq++;
						else
							break;
					}
					if(seq>=atleast) {
						vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Vertical));
					}
					y=i-1;
				}
			}
		}

		void CheckColumnsForSequences(std::vector<SequenceCheckResult<T_> > &vec) const {
			CheckColumnsForSequences(vec, me().getheight());
		}

		void CheckColumnsForSequencesOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int x=0;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last==value) {
						for(i=y+1;i<h;i++) {
							if(Get(x,i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Vertical));
						}
						y=i-1;
					}
				}
			}
		}

		void CheckColumnsForSequencesOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value) const {
			CheckColumnsForSequence(vec, value, me().getheight());
		}

		void CheckColumnsForSequencesNotOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();
			for(int x=0;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq=1;
					T_ last=Get(x,y);
					int i;
					if(last!=value) {
						for(i=y+1;i<h;i++) {
							if(Get(x,i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::Vertical));
						}
						y=i-1;
					}
				}
			}
		}

		void CheckColumnsForSequencesNotOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value) const {
			CheckColumnsForSequences(vec, value, me().getheight());
		}

		void CheckDiagonalsForSequences(std::vector<SequenceCheckResult<T_> > &vec, int atleast) const {
			int w=me().getwidth(), h=me().getheight();

			//forward diagonal
			for(int x=0;x<1+w-atleast;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;

					seq=1;
					last=Get(x,y);
					if( !(x>0 && y>0 && last==Get(x-1, y-1)) ) {
						for(i=1;i<std::min(w-x,h-y);i++) {
							if(Get(x+i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::ForwardDiagonal));
						}
					}
				}
			}

			//backward diagonal
			for(int x=atleast-1;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;
					seq=1;
					last=Get(x,y);
					if( !(x<w-1 && y>0 && last==Get(x+1, y-1)) ) {
						for(i=1;i<std::min(x+1,h-y);i++) {//will create problems for multiple
							if(Get(x-i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::BackwardDiagonal));
						}
					}
				}
			}
		}

		void CheckDiagonalsForSequences(std::vector<SequenceCheckResult<T_> > &vec) const {
			CheckDiagonalsForSequences(vec, std::min(me().getwidth(), me().getheight()));
		}

		void CheckDiagonalsForSequencesOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();

			//forward diagonal
			for(int x=0;x<1+w-atleast;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;

					seq=1;
					last=Get(x,y);
					if( last==value && !(x>0 && y>0 && last==Get(x-1, y-1)) ) {
						for(i=1;i<std::min(w-x,h-y);i++) {
							if(Get(x+i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::ForwardDiagonal));
						}
					}
				}
			}

			//backward diagonal
			for(int x=atleast-1;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;
					seq=1;
					last=Get(x,y);
					if( last==value && !(x<w-1 && y>0 && last==Get(x+1, y-1)) ) {
						for(i=1;i<std::min(x+1,h-y);i++) {//will create problems for multiple
							if(Get(x-i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::BackwardDiagonal));
						}
					}
				}
			}
		}

		void CheckDiagonalsForSequencesOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value) const {
			CheckDiagonalsForSequencesOf(vec, value, std::min(me().getwidth(), me().getheight()));
		}

		void CheckDiagonalsForSequencesNotOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value, int atleast) const {
			int w=me().getwidth(), h=me().getheight();

			//forward diagonal
			for(int x=0;x<1+w-atleast;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;

					seq=1;
					last=Get(x,y);
					if( last!=value && !(x>0 && y>0 && last==Get(x-1, y-1)) ) {
						for(i=1;i<std::min(w-x,h-y);i++) {
							if(Get(x+i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::ForwardDiagonal));
						}
					}
				}
			}

			//backward diagonal
			for(int x=atleast-1;x<w;x++) {
				for(int y=0;y<1+h-atleast;y++) {
					int seq;
					T_ last;
					int i;
					seq=1;
					last=Get(x,y);
					if( last!=value && !(x<w-1 && y>0 && last==Get(x+1, y-1)) ) {
						for(i=1;i<std::min(x+1,h-y);i++) {//will create problems for multiple
							if(Get(x-i,y+i)==last)
								seq++;
							else
								break;
						}
						if(seq>=atleast) {
							vec.push_back(SequenceCheckResult<T_>(x,y,seq,last,SequenceCheckResult<T_>::BackwardDiagonal));
						}
					}
				}
			}
		}

		void CheckDiagonalsForSequencesNotOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value) const {
			CheckDiagonalsForSequencesNotOf(vec, value, std::min(me().getwidth(), me().getheight()));
		}

		void CheckAllForSequences(std::vector<SequenceCheckResult<T_> > &vec, int atleast) const {
			CheckRowsForSequences(vec,atleast);
			CheckColumnsForSequences(vec,atleast);
			CheckDiagonalsForSequences(vec,atleast);
		}

		void CheckAllForSequencesOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value, int atleast) const {
			CheckRowsForSequencesOf(vec, value, atleast);
			CheckColumnsForSequencesOf(vec, value, atleast);
			CheckDiagonalsForSequencesOf(vec, value, atleast);
		}

		void CheckAllForSequencesNotOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value, int atleast) const {
			CheckRowsForSequencesNotOf(vec, value, atleast);
			CheckColumnsForSequencesNotOf(vec, value, atleast);
			CheckDiagonalsForSequencesNotOf(vec, value, atleast);
		}

		void CheckAllForSequences(std::vector<SequenceCheckResult<T_> > &vec) const {
			CheckAllForSequences(vec, std::min(me().getwidth(), me().getheight()));
		}

		void CheckAllForSequencesOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value) const {
			CheckAllForSequencesOf(vec, value, std::min(me().getwidth(), me().getheight()));
		}

		void CheckAllForSequencesNotOf(std::vector<SequenceCheckResult<T_> > &vec, const T_ &value) const {
			CheckAllForSequencesNotOf(vec, value, std::min(me().getwidth(), me().getheight()));
		}
	protected:
		T_ &operator[](int ind) {
			return me().buffer[ind];
		}

		int buffersize() const { return me().getwidth()*me().getheight(); }

	private:
		Map_ &me() {
			return *(Map_*)this;
		}

		const Map_ &me() const {
			return *(const Map_*)this;
		}

	};

	template<class Map_, class T_>
	static mapfiller<Map_, T_> operator << (mapoperations<Map_, T_> &map, const T_ &value) {
		mapfiller<Map_, T_> m((Map_&)map);

		m<<value;

		return m;
	}


	//Specify sizes for small objects. For fixed sized,
	//buffer is allocated on the stack. This means it is
	//copied every time this object is passed by value.
	//In variable case buffer is not duplicated unless
	//it is specified so.
	template <class T_, int width_=0, int height_=0>
	class Map2D : public mapoperations<Map2D<T_, width_, height_>,T_> {
		friend class mapoperations<Map2D<T_, width_, height_>, T_>;
	public:



	protected:
		int getwidth() const { return width_; }
		int getheight() const { return height_; }
	private:
		T_ buffer[width_*height_];
	};
	

	//This class is for variable sized Maps, passing the object by value is
	//no different than passing the object by reference. They will be linked.
	//Data is cleared when there are no variables pointing at it.
	template <class T_>
	class Map2D<T_,0,0> : public mapoperations<Map2D<T_, 0, 0>,T_> {
		friend class mapoperations<Map2D<T_, 0, 0>,T_>;
	public:

		//Size(0,0) map
		Map2D() : width(new int(0)), height(new int(0)) {
		}

		//Initialize using the given size
		Map2D(int width, int height) : width(new int(width)), height(new int(height)), buffer(width*height) {
		}

		//Initialize using the given size
		Map2D(Size size) : width(new int(size.Width)), height(new int(size.Height)), buffer(size.Width*size.Height) {
		}

		//Link with the given map
		Map2D(Map2D &map) : width(map.width), height(map.height), buffer(map.buffer) {
		}

		Map2D Duplicate() {
			Map2D map(*width, *height);
			std::copy(buffer.begin(), buffer.end(), map.buffer.begin());

			return map;
		}

		//Link with the given map, previous one will be abandoned and will be destroyed if
		//needed
		Map2D &operator =(Map2D &map) {
			if(buffer.getReferenceCount()==1) {
				delete width;
				delete height;
			}

			width=map.width;
			height=map.height;
			buffer=map.buffer;
		}

		void Resize(int width, int height) {
			*this->width=width;
			*this->height=height;

			buffer.Resize(width*height);
		}

		void Resize(Size size) {
			Resize(size.Width, size.Height);
		}

	protected:
		int getwidth() const { return *width; }
		int getheight() const { return *height; }
	private:
		ManagedBuffer<T_> buffer;
		int *width, *height;
	};

	template<class T_, int w_, int h_>
	std::ostream &operator <<(std::ostream &out, const Map2D<T_, w_, h_> &map) {
		for(int y=0;y<map.GetHeight();y++) {
			for(int x=0;x<map.GetWidth();x++) {
				out<<map(x,y)<<" ";
			}

			out<<std::endl;
		}

		return out;
	}


} }

#pragma warning(pop)
