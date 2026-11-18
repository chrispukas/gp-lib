#pragma once

#include <cstddef>
#include <ostream>
#include <type_traits>
#include <vector>

#include "stringbuilder.h"

template<typename T>
struct is_nested_container { static const bool value {false}; };
template<typename T>
struct is_nested_container<std::vector<std::vector<T>>> { static const bool value {true}; };

template<typename T>
struct is_container { static const bool value {false}; };
template<typename T>
struct is_container<std::vector<T>> { static const bool value {true}; };


template<typename T>
class Mat
{
public:
    std::vector<T> _data;
    std::vector<size_t> _addressPrefixArr, _dim;

    size_t _size;
    size_t _getFlatSize() const;

    const std::vector<size_t> _addressPrefix();
    size_t _getAddress(const std::vector<size_t>& i_n);
    void _setAddress(const std::vector<size_t>& i_n, T val);

    template<typename Elem>
    std::vector<T> _flatten(const Elem& v);
    template<typename Elem> // Is a 1-deep container.
    void _flattenHelper(const Elem& v, std::vector<T>& h, std::vector<size_t>& idx,
        typename std::enable_if<is_container<Elem>::value, Elem>::type* = nullptr);
    template<typename Elem> // Is a primitive
    void _flattenHelper(const Elem& v, std::vector<T>& h, std::vector<size_t>& idx, 
        typename std::enable_if<std::is_same<Elem, T>::value, Elem>::type* = nullptr);
    bool _verifyMat(const std::vector<size_t>& v1, const std::vector<size_t>& v2);

    template<typename Elem>
    std::vector<size_t> _getDims(const Elem& v, 
        typename std::enable_if<is_container<Elem>::value, Elem>::type* = nullptr);
    template<typename Elem>
    std::vector<size_t> _getDims(const Elem& v, 
        typename std::enable_if<!is_container<Elem>::value, Elem>::type* = nullptr);

    template<typename Elem>
    std::string& _stringBuilder(
        typename std::enable_if<is_container<Elem>::value, Elem>::type* = nullptr);
    template<typename Elem>
    std::string& _stringBuilder(
        typename std::enable_if<!is_container<Elem>::value, Elem>::type* = nullptr);
    
    std::string printMat();
    std::string printMatFlat() { return vecToString(_data); };

    void _addDim(int v, int _idxDim);
    void _removeDim(int v, int _idxDim);

    void _addDim(int _idxDim) { _addDim(0, _idxDim); }; // update prefix
    void _removeDim(int _idxDim) { _removeDim(0, _idxDim); }; // update prefix

    static Mat<T> _add(const Mat<T>& l, const Mat<T>& r);
    friend Mat<T> operator+(const Mat<T>& l, const Mat<T>& r) {return _add(l, r); }

    friend std::ostream& operator<<(std::ostream& l, const Mat<T>& r);

    static bool _verifyShapeCompatability(const Mat<T>& l, const Mat<T>& r);

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

    template<typename Elem, typename = typename std::enable_if<is_container<Elem>::value>::type>
    Mat<T>(const Elem& v) : _dim{_getDims(v)}, _size{_getFlatSize()}, _data{_flatten(v)} { }
    Mat<T>(const T& v) : _dim{_getDims(std::vector<T>{v})}, _size{_getFlatSize()}, _data{_flatten(std::vector<T>{v})} { }
    Mat<T>() : _dim{std::vector<size_t>()}, _size{0}, _data{std::vector<T>{}} {}
};

