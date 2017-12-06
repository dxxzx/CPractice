#include <iostream>
#include <string>

using namespace std;

int main() 
{
    constexpr size_t rowCnt = 3, colCnt = 4;
    int ia[rowCnt][colCnt];    // 12 uninitialized elements
    // for each row
    for (size_t i = 0; i != rowCnt; ++i) {
        // for each column with in the row
        for (size_t j = 0; j != colCnt; ++j) {
            // assign the element's positional index as its value
            ia[i][j] = i * colCnt + j;
        }
    }

    using int_array = int[4];  // new style type alias declaration
    typedef int int_array[4];  // equivalent typedef declaration;
    // print the value of each element in ia, with each inner array on its own line
    for (int_array *p = ia; p != ia + 3; ++p) {
        for (int *q = *p; q != *p + 4; ++q)
            cout << *q << ' ';
        cout << endl;
    }
}
