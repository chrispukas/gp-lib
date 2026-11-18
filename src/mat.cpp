#include "mat.h"
#include "stringbuilder.h"

#include <algorithm>
#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>


template<typename T>
const std::vector<size_t> Mat<T>::_addressPrefix()
{
    size_t N{_dim.size()};
    std::vector<size_t> o(N+1, 1);

    for (size_t i = 1; o.size() > i; i++)
    {
        o[i] = o[i-1] * _dim[i];
    }
    std::reverse(o.begin(), o.end());
    return o;
}

template<typename T>
size_t Mat<T>::_getAddress(const std::vector<size_t>& vec)
{
    if (vec.size() != _dim.size() || vec.empty())
        return std::string::npos;

    size_t N{vec.size()}, f{vec.back()};
    for (size_t i = 0; N > i; i++)
        f += _addressPrefixArr[i] * vec[i];

    return f;
}

template<typename T>
void Mat<T>::_setAddress(const std::vector<size_t>& i_n, T val)
{
    size_t addr = _getAddress(i_n);
    if (addr >= _data.size())
        return; // out of bounds

    _data[addr] = val;
}

template<typename T>
size_t Mat<T>::_getFlatSize() const
{
    if (_dim.empty())
        return std::string::npos;

    size_t o{1};
    for (const auto& v: _dim)
        o *= v;
    return o;
}

template<typename T>
template<typename Elem>
std::vector<T> Mat<T>::_flatten(const Elem& v)
{
    std::vector<T> o(_size, static_cast<T>(0.));
    std::vector<size_t> idx{};
    
    _flattenHelper(v, o, idx);
    return o;
}

template<typename T>
template<typename Elem> // Is at least 1-deep container.
void Mat<T>::_flattenHelper(const Elem& v, std::vector<T>& h, std::vector<size_t>& idx,
    typename std::enable_if<is_container<Elem>::value, Elem>::type*)
{
    if (v.empty())
        return;
    for (size_t i = 0; v.size() > i; i++)
    {
        idx.push_back(i);
        _flattenHelper(v[i], h, idx);
        idx.pop_back();
    }    
}

template<typename T>
template<typename Elem> // Is a primitive
void Mat<T>::_flattenHelper(const Elem& v, std::vector<T>& h, std::vector<size_t>& idx, 
    typename std::enable_if<std::is_same<Elem, T>::value, Elem>::type*)
{
    _setAddress(idx, v);
}


template<typename T>
bool Mat<T>::_verifyMat(const std::vector<size_t>& v1, const std::vector<size_t>& v2)
{
    if (v1.size() != v2.size())
        return false;

    for (size_t j {0}; v1.size() > j; j++)
    {
        if (v1[j] != v2[j])
            return false;
    }

    return true;
}

template<typename T>
template<typename Elem>
std::vector<size_t> Mat<T>::_getDims(const Elem& v, 
    typename std::enable_if<is_container<Elem>::value, Elem>::type*)
{
    if (v.size() == 0)
        return std::vector<size_t>{};
    std::vector<size_t> o {_getDims(v[0])};

    for (size_t i {1}; v.size() > i; i++)
    {
        // Proposed dimensions of new layer
        std::vector<size_t> sub {_getDims(v[i])};
        if (!_verifyMat(o, sub))
            return std::vector<size_t>{};        
    }

    o.push_back(v.size());
    return o;
}

template<typename T>
template<typename Elem>
std::vector<size_t> Mat<T>::_getDims(const Elem& v, 
    typename std::enable_if<!is_container<Elem>::value, Elem>::type*)
{
    return std::vector<size_t>{};
}

template<typename T>
void Mat<T>::_addDim(int v, int _idxDim)
{

}

template<typename T>
void Mat<T>::_removeDim(int v, int _idxDim) 
{

}




template<typename T>
template<typename Elem>
std::string& Mat<T>::_stringBuilder(
    typename std::enable_if<is_container<Elem>::value, Elem>::type*)
{
    
}
template<typename T>
template<typename Elem>
std::string& Mat<T>::_stringBuilder(
    typename std::enable_if<!is_container<Elem>::value, Elem>::type*)
{
    
}

template<typename T>
std::string Mat<T>::printMat()
{
    std::string o;
    o.reserve(_size*3);

    return o;
}

template<typename T>
std::ostream& operator<<(std::ostream& l, const Mat<T>& r)
{



}

template<typename T>
Mat<T> Mat<T>::_add(const Mat<T>& l, const Mat<T>& r)
{
    // following improvements needed:
    // 1. checks for type of l, and r (maybe some type promotion would be interesting?).

    if (!Mat::_verifyShapeCompatability(l, r))
        throw std::out_of_range("Incorrect shapes of matricies to be added! m1.dim: " + 
            vecToString(l._dim) + ", m2.dim: " + vecToString(r._dim) + "\n");

    Mat<T> m;
    m._like(l); // creates matrix of like shape, elems of 0

    for (size_t i = 0; i < l._size; i++)
        m._data[i] = l._data[i] + r._data[i];

    return m;
} 








// Statics
template<typename T>
bool Mat<T>::_verifyShapeCompatability(const Mat<T>& l, const Mat<T>& r)
{
    if (l._size != r._size || l._dim.size() != r._dim.size())
        return false;
    for (int idx_dim = 0; l._dim.size() > idx_dim; idx_dim++)
    {
        if (l._dim[idx_dim] != r._dim[idx_dim])
            return false;
    }

    return true;
}





// Explicit instantiation of template types
template class Mat<float>;
template class Mat<double>;
template class Mat<long double>;