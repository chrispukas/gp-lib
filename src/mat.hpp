#pragma once

#include <functional>
#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <iostream>

#include "stringbuilder.hpp"

template<typename T>
struct is_nested_container { static const bool value {false}; };
template<typename T>
struct is_nested_container<std::vector<std::vector<T>>> { static const bool value {true}; };

template<typename T>
struct is_container { static const bool value {false}; };
template<typename T>
struct is_container<std::vector<T>> { static const bool value {true}; };

template<typename T>
using if_container_t = std::enable_if_t<is_container<T>::value, int>;
template<typename T>
using if_not_container_t = std::enable_if_t<!is_container<T>::value, int>;

template<typename T>
class Mat
{
public:
    std::vector<size_t> _dim, _addressPrefixArr;
    size_t _size;
    std::vector<T> _data;


    size_t _getFlatSize() const
    {
        if (_dim.empty())
            return std::string::npos;

        size_t o{1};
        for (const auto& v: _dim)
            o *= v;
        return o;
    };

    const std::vector<size_t> _addressPrefix()
    {
        size_t N{_dim.size()};
        std::vector<size_t> o(N+1, 1);

        for (size_t i = 1; o.size() > i; i++)
        {
            o[i] = o[i-1] * _dim[i];
        }
        std::reverse(o.begin(), o.end());
        return o;
    };
    size_t _getAddress(const std::vector<size_t>& vec)
    {
        if (vec.size() != _dim.size() || vec.empty())
            throw std::runtime_error("Error: Failed to get address: vec.size(): " + std::to_string(vec.size()) + ", _dim.size(): " + std::to_string(_dim.size())) ;

        size_t N{vec.size()}, f{vec.back()};
        for (size_t i = 0; N > i; i++)
            f += _addressPrefixArr[i] * vec[i];

        return f;
    };


    void _setAddress(const std::vector<size_t>& i_n, const T val) { _setAddress(i_n, val, _data); }
    void _setAddress(const std::vector<size_t>& i_n, const T val, std::vector<T>& vec)
    {
        size_t addr = _getAddress(i_n);
        if (addr >= vec.size())
            throw std::out_of_range("Error: attempted to write to an out-of-bounds address. addr.size(): " + std::to_string(addr) + " for indicies: " + vecToString(i_n) +", vec(_data).size(): " + std::to_string(vec.size()) + ", _addressPrefixArr: " + vecToString(_addressPrefixArr) + ".");
        vec[addr] = val;
    };

    template<typename Elem>
    std::vector<T> _flatten(const Elem& v)
    {
        std::vector<T> o(_size, static_cast<T>(0.));
        std::vector<size_t> idx{};

        _flattenHelper(v, o, idx);
        return o;
    }
    template<typename Elem, if_container_t<Elem> = 0>
    void _flattenHelper(const Elem& v, std::vector<T>& vec, std::vector<size_t>& idx)
    {
        if (v.empty())
            return;
        for (size_t i = 0; v.size() > i; i++)
        {
            idx.push_back(i);
            _flattenHelper(v[i], vec, idx);
            idx.pop_back();
        }    
    }
    template<typename Elem, if_not_container_t<Elem> = 0>
    void _flattenHelper(const Elem& v, std::vector<T>& vec, std::vector<size_t>& idx) { _setAddress(idx, v, vec); };

    template<typename Elem, if_not_container_t<Elem> = 0>
    static std::vector<size_t> _getDims(const Elem& v) { return std::vector<size_t>{}; };
    
    template<typename Elem, if_container_t<Elem> = 0>
    static std::vector<size_t> _getDims(const Elem& v)
    {
        if (v.size() == 0)
            return std::vector<size_t>{};
        std::vector<size_t> o {_getDims(v[0])};

        for (size_t i {1}; v.size() > i; i++)
        {
            // Proposed dimensions of new layer
            std::vector<size_t> sub {_getDims(v[i])};
            if (!_verifyShapeCompatability(o, sub))
                throw std::runtime_error("Inserted matrix is incompatible, should be an n-dim hypercube: " + vecToString(o) + ", " + vecToString(sub) + "\n");
        }

        o.push_back(v.size());
        return o;
    }

    template<typename Elem, if_container_t<Elem> = 0>
    std::string& _stringBuilder() { throw "_stringBuilder not implemented!"; };
    template<typename Elem, if_not_container_t<Elem> = 0>
    std::string& _stringBuilder() { throw "_stringBuilder not implemented!"; };
    
    std::string printMat() {
        std::string o;
        o.reserve(_size*3);

        return o;
    };
    std::string printMatFlat() { return vecToString(_data); };

    void _addDim(int v, int _idxDim) {throw "_addDim not implemented!"; };
    void _removeDim(int v, int _idxDim) {throw "_removeDim not implemented!"; };;

    void _addDim(int _idxDim) { _addDim(0, _idxDim); }; // update prefix
    void _removeDim(int _idxDim) { _removeDim(0, _idxDim); }; // update prefix

    // Matrix operators
    template<typename Op>
    static Mat<T> _elementWise(const Mat<T>& l, const Mat<T>& r)
    {
        if (!Mat::_verifyShapeCompatability(l, r))
            throw std::out_of_range("Incorrect shapes of matricies to be added! m1.dim: " + 
                vecToString(l._dim) + ", m2.dim: " + vecToString(r._dim) + "\n");

        Mat<T> m;
        m._like(l); // creates matrix of like shape, elems of 0

        for (size_t i = 0; i < l._size; i++)
            m._data[i] = Op{}(l._data[i], r._data[i]);

        return m;
    }

    friend Mat<T> operator+(const Mat<T>& l, const Mat<T>& r) {return _elementWise<std::plus<void>>(l, r); }
    friend Mat<T> operator-(const Mat<T>& l, const Mat<T>& r) {return _elementWise<std::minus<void>>(l, r); }
    friend Mat<T> operator*(const Mat<T>& l, const Mat<T>& r) {return _elementWise<std::multiplies<void>>(l, r); }
    friend Mat<T> operator/(const Mat<T>& l, const Mat<T>& r) {return _elementWise<std::divides<void>>(l, r); }

    void _inplaceCopy(const Mat<T>& l, Mat<T>& r)
    {
        for (size_t i = 0; i < l._size; i++)
            r._data[i] = l._data[i];
    }
    Mat<T>& _copy(const Mat<T>& m)
    {
        _like(m);
        _inplaceCopy(m, *this); // copy l -> r
        return *this;
    };
    Mat<T>& operator=(const Mat<T>& other) { return _copy(other); }

    template<typename Elem>
    friend std::ostream& operator<<(std::ostream& os, const Mat<T>& v)
    {
        return os << v.template _stringBuilder<Elem>();
    };

    static bool _verifyShapeCompatability(T l, T r) { return true; } // primitives base-cases
    static bool _verifyShapeCompatability(const Mat<T>& l, const Mat<T>& r)
    {
        if (l._size != r._size)
            return false;
        return _verifyShapeCompatability(l._dim, r._dim);
    }

    template<typename Elem>
    static bool _verifyShapeCompatability(const std::vector<Elem>& l, const std::vector<Elem>& r)
    {
        if (l.size() != r.size())
            return false;
        for (int idx_dim = 0; l.size() > idx_dim; idx_dim++)
        {
            if (l[idx_dim] != r[idx_dim])
                return false;
        }

        return true;
    }

    void _like(const Mat<T>& t) { _like(t, static_cast<T>(0.)); }
    void _like(const Mat<T>& t, T v)
    {
        _data = std::vector<T>(t._size, v);
        _size = t._size;
        _dim = t._dim;
        _addressPrefixArr = t._addressPrefixArr;   
    }


public:
    int getRank() { return _dim.size(); };

    template<typename Elem>
    Mat<T>(const std::vector<Elem>& v) : _dim{_getDims(v)}, _addressPrefixArr{_addressPrefix()}, _size{_getFlatSize()}, _data{_flatten(v)} {}// { }

    Mat<T>(const T& v) : Mat(std::vector<T>{v}) {}
    Mat<T>() : Mat(std::vector<T>{}) {}
};


//_size{_getFlatSize()}, _data{_flatten(v)}