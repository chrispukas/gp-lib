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
template<typename Elem, if_container_t<Elem>> // Is at least 1-deep container.
void Mat<T>::_flattenHelper(const Elem& v, std::vector<T>& h, std::vector<size_t>& idx)
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
template<typename Elem, if_not_container_t<Elem>>
void Mat<T>::_flattenHelper(const Elem& v, std::vector<T>& h, std::vector<size_t>& idx)
{
    _setAddress(idx, v);
}


template<typename T>
template<typename Elem, if_container_t<Elem>>
std::vector<size_t> Mat<T>::_getDims(const Elem& v)
{
    if (v.size() == 0)
        return std::vector<size_t>{};
    std::vector<size_t> o {_getDims(v[0])};

    for (size_t i {1}; v.size() > i; i++)
    {
        // Proposed dimensions of new layer
        std::vector<size_t> sub {_getDims(v[i])};
        if (!_verifyShapeCompatability(o, sub))
            return std::vector<size_t>{};        
    }

    o.push_back(v.size());
    return o;
}

template<typename T>
template<typename Elem, if_not_container_t<Elem>>
std::vector<size_t> Mat<T>::_getDims(const Elem& v)
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
template<typename Elem, if_container_t<Elem>>
std::string& Mat<T>::_stringBuilder()
{
    
}
template<typename T>
template<typename Elem, if_not_container_t<Elem>>
std::string& Mat<T>::_stringBuilder()
{
    
}

template<typename T>
std::string Mat<T>::printMat()
{
    std::string o;
    o.reserve(_size*3);

    return o;
}

template<typename T, typename Elem>
std::ostream& operator<<(std::ostream& os, const Mat<T>& v)
{
    return os << v.template _stringBuilder<Elem>();
}

template<typename T>
template<typename Op>
Mat<T> Mat<T>::_elementWise(const Mat<T>& l, const Mat<T>& r)
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

// Statics
template<typename T>
bool Mat<T>::_verifyShapeCompatability(const Mat<T>& l, const Mat<T>& r)
{
    if (l._size != r._size)
        return false;
    return _verifyShapeCompatability(l._dim, r._dim);
}

template<typename T>
bool Mat<T>::_verifyShapeCompatability(const std::vector<T>& l, const std::vector<T>& r)
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


// Explicit instantiation of template types
template class Mat<float>;
template class Mat<double>;
template class Mat<long double>;