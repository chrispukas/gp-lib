#include <cstddef>
#include <vector>
#include "mat.h"



int main()
{
    std::vector<std::vector<std::vector<float>>> vec \
        {
            {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}}, 
            {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}}, 
        };;

    Mat<float>* mat = new Mat<float>{vec};

    delete mat;

    return 0;
}