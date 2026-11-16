#pragma once

#include <cstddef>
#include <ostream>
#include <type_traits>
#include <vector>

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
    std::string printMatFlat();

    void _addDim(int v, int _idxDim);
    void _removeDim(int v, int _idxDim);

    void _addDim(int _idxDim) { _addDim(0, _idxDim); }; // update prefix
    void _removeDim(int _idxDim) { _removeDim(0, _idxDim); }; // update prefix

    friend std::ostream& operator<<(std::ostream& os, const Mat<T>& mat);


public:
    int getRank() { return _dim.size(); };

    template<typename Elem, typename = typename std::enable_if<is_container<Elem>::value>::type>
    Mat<T>(const Elem& v) : _dim{_getDims(v)}, _data{_flatten(v)} { }
    Mat<T>(const T& v) : _dim{_getDims(std::vector<T>{v})}, _data{_flatten(std::vector<T>{v})} { }
};

