#pragma once

#include <vector>
#include <exception>

#include "Point.h"
#include "Line.h"

namespace Gorgon { namespace Geometry {
    
    /**
     * This class represents a set of points. This class can be used for 
     * contours, line lists, polygons, or even point clouds. Some of the
     * functions may be meaningful for some of these uses.
     */
    template<class P_ = Pointf>
    class PointList {
    public:
        
        ///Empty constructor
        PointList() = default;
        
        ///Implicit vector to PointList casting. You may move in the data using std::move
        PointList(std::vector<P_> points) : Points(std::move(points)) { }
        
        ///Initializer list
        PointList(std::initializer_list<P_> points) : Points(std::move(points)) { }
        
        ///Due to relatively high cost, copying is disabled. Use Duplicate instead
        PointList(const PointList &) = delete;
        
        ///Move constructor
        PointList(PointList &&other) {
            Swap(other);
        }
        
        ///Duplicates this PointList
        PointList Duplicate() const {
            return {Points};
        }
        
        
        ///Due to relatively high cost, copying is disabled. Use Duplicate instead
        PointList &operator =(const PointList &) = delete;
        
        ///Move assignment
        PointList &operator =(PointList &&right) {
            this->Destroy();
            
            Swap(right);
            
            return *this;
        }
        
        ///Destroys the storage used by this list
        void Destroy() {
            std::vector<P_> n;
            
            using std::swap;
            
            swap(n, Points);
        }
        
        ///Clears the elements in this list
        void Clear() {
            Points.clear();
        }
        
        ///Returns the number of elements in the list
        auto Size() const {
            return Points.size();
        }
        
        ///Begin iterator to underlying points vector
        auto begin() {
            return Points.begin();
        }
        
        ///End iterator to underlying points vector
        auto end() {
            return Points.end();
        }
        
        ///Begin iterator to underlying points vector
        auto begin() const {
            return Points.begin();
        }
        
        ///End iterator to underlying points vector
        auto end() const {
            return Points.end();
        }
        
        ///Begin iterator to underlying points vector
        auto rbegin() {
            return Points.rbegin();
        }
        
        ///End iterator to underlying points vector
        auto rend() {
            return Points.rend();
        }
        
        ///Begin iterator to underlying points vector
        auto rbegin() const {
            return Points.rbegin();
        }
        
        ///End iterator to underlying points vector
        auto rend() const {
            return Points.rend();
        }
        
        ///Accesses the elements in the list
        P_ &operator [](std::size_t index) {
            return Points[index];
        }
        
        ///Accesses the elements in the list
        const P_ &operator [](std::size_t index) const {
            return Points[index];
        }
        
        ///Accesses the first element in the list
        P_ &Front() {
            return Points.front();
        }
        
        ///Accesses the last element in the list
        P_ &Back() {
            return Points.back();
        }
        
        ///Accesses the first element in the list
        const P_ &Front() const {
            return Points.front();
        }
        
        ///Accesses the last element in the list
        const P_ &Back() const {
            return Points.back();
        }
        
        ///Returns the element at the given index. This function treats
        ///the list as cyclic.
        P_ &At(long index) {
            if(Points.size() == 0)
                throw std::out_of_range("List is empty");
            
            return Points[PositiveMod(index, (long)Points.size())];
        }
        
        ///Returns the element at the given index. This function treats
        ///the list as cyclic.
        const P_ &At(long index) const {
            if(Points.size() == 0)
                throw std::out_of_range("List is empty");
            
            return Points[PositiveMod(index, (long)Points.size())];
        }
        
        ///Returns the line at the given index. This function treats
        ///the list as a cyclic line list. The list is assumed to be
        ///closed.
        Line<P_> LineAt(long index) const {
            if(Points.size() == 0)
                throw std::out_of_range("List is empty");
                        
            return {Points[PositiveMod(index, (long)Points.size())], Points[PositiveMod(index+1, (long)Points.size())]};
        }
        
        /// Adds a new point to the end of the point list
        void Push(P_ point) {
            Points.push_back(point);
        }
        
        /// Adds a new point to the end of the point list
        template<class ...T_>
        void Push(T_&&... params) {
            Points.emplace_back(std::forward<T_>(params)...);
        }
        
        /// Removes the last point from the list
        void Pop() {
            Points.pop_back();
        }
        
        ///Adds the coordinates of the points on the right list to the left.
        ///Right list is treated as cyclic. If right list is empty, nothing is
        ///done.
        PointList operator +(const PointList &right) const {
            PointList ret;
            
            long ind = 0;
            for(const auto &p : *this) {
                ret.Push(p + right.Get(ind));
                
                ind++;
            }
            
            return ret;
        }
        
        ///Adds the coordinates of the points on the right list to the left.
        ///Right list is treated as cyclic. If right list is empty, nothing is
        ///done.
        PointList &operator +=(const PointList &right) {
            PointList ret;
            
            long ind = 0;
            for(auto &p : *this) {
                p += right.Get(ind);
                
                ind++;
            }
            
            return *this;
        }
        
        ///Subtracts the coordinates of the points on the right list to the left.
        ///Right list is treated as cyclic. If right list is empty, nothing is
        ///done.
        PointList operator -(const PointList &right) const {
            PointList ret;
            
            long ind = 0;
            for(const auto &p : *this) {
                ret.Push(p - right.Get(ind));
                
                ind++;
            }
            
            return ret;
        }
        
        ///Subtracts the coordinates of the points on the right list to the left.
        ///Right list is treated as cyclic. If right list is empty, nothing is
        ///done.
        PointList &operator -=(const PointList &right) {
            PointList ret;
            
            long ind = 0;
            for(auto &p : *this) {
                p -= right.Get(ind);
                
                ind++;
            }
            
            return *this;
        }
        
        ///Adds a point to each element of the list.
        template<class O_>
        PointList operator +(const O_ &right) const {
            PointList ret;
            
            for(const auto &p : *this) {
                ret.Push(p + right);
            }
            
            return ret;
        }
        
        ///Adds a point to each element of the list.
        template<class O_>
        PointList &operator +=(const O_ &right) {
            PointList ret;
            
            for(auto &p : *this) {
                p += right;
            }
            
            return *this;
        }
        
        ///Subtracts a point to each element of the list.
        template<class O_>
        PointList operator -(const O_ &right) const {
            PointList ret;
            
            for(const auto &p : *this) {
                ret.Push(p - right);
            }
            
            return ret;
        }
        
        ///Subtracts a point to each element of the list.
        template<class O_>
        PointList &operator -=(const O_ &right) {
            PointList ret;
            
            for(auto &p : *this) {
                p -= right;
            }
            
            return *this;
        }
        
        ///Adds a point to each element of the list.
        template<class O_>
        PointList operator *(const O_ &right) const {
            PointList ret;
            
            for(const auto &p : *this) {
                ret.Push(p * right);
            }
            
            return ret;
        }
        
        ///Adds a point to each element of the list.
        template<class O_>
        PointList &operator *=(const O_ &right) {
            PointList ret;
            
            for(auto &p : *this) {
                p *= right;
            }
            
            return *this;
        }
        
        ///Subtracts a point to each element of the list.
        template<class O_>
        PointList operator /(const O_ &right) const {
            PointList ret;
            
            for(const auto &p : *this) {
                ret.Push(p / right);
            }
            
            return ret;
        }
        
        ///Subtracts a point to each element of the list.
        template<class O_>
        PointList &operator /=(const O_ &right) {
            PointList ret;
            
            for(auto &p : *this) {
                p /= right;
            }
            
            return *this;
        }
        
        ///Swaps two lists, mainly for move operations.
        void Swap(PointList<P_> &right) {
            using std::swap;
            
            swap(this->Points, right.Points);
        }
        
        ///Stored points. You may directly use this class as if it is a point vector.
        std::vector<P_> Points;
    };

    ///Comparison: this operation is expensive: O(n).
    template<class P_>
    bool operator <(PointList<P_> &left, const PointList<P_> &right) {
        return left.Points < right.Points;
    }
    
    ///Comparison: this operation is expensive: O(n).
    template<class P_>
    bool operator >(PointList<P_> &left, const PointList<P_> &right) {
        return left.Points > right.Points;
    }
    
    ///Comparison: this operation is expensive: O(n).
    template<class P_>
    bool operator <=(PointList<P_> &left, const PointList<P_> &right) {
        return left.Points <= right.Points;
    }
    
    ///Comparison: this operation is expensive: O(n).
    template<class P_>
    bool operator >=(PointList<P_> &left, const PointList<P_> &right) {
        return left.Points >= right.Points;
    }
    
    ///Comparison: this operation is expensive: O(n).
    template<class P_>
    bool operator ==(PointList<P_> &left, const PointList<P_> &right) {
        return left.Points == right.Points;
    }
    
    ///Comparison: this operation is expensive: O(n).
    template<class P_>
    bool operator !=(PointList<P_> &left, const PointList<P_> &right) {
        return left.Points != right.Points;
    }
    
    template<class P_>
    void swap(PointList<P_> &left, PointList<P_> &right) {
        left.Swap(right);
    }
    
    //non-member operations: translate, scale, rotate, etc...
    
} }
