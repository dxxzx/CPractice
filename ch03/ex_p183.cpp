#include <iostream>

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
    // print the value of each element in ia, with each inner array on its own line
    // p points to an array of four ints
    for (auto p = ia; p != ia + 3; ++p) {
        // q points to the first element of an array of four ints; that is, q points to an int
        for (auto q = *p; q != *p + 4; ++q) {
            cout << *q << ' ';
        }
        cout << endl;
    }
}