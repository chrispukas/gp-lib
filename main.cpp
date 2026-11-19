#include <iostream>
#include <string>
#include <vector>

#include "mat.hpp"
#include "stringbuilder.hpp"



int main()
{
    std::vector<std::vector<std::vector<float>>> vec \
        {
            {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}}, 
            {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}}, 
        };;

    Mat<float>* mat = new Mat<float>{vec};
    std::cout << mat->_data.size() << " " << mat->_dim.size() << "\n";
    std::cout << vecToString(mat->_dim) << "\n";
    std::cout << vecToString(Mat<float>::_getDims(vec)) << "\n";
    std::cout << Mat<float>::_getDims(vec).size();

    delete mat;

    return 0;
}