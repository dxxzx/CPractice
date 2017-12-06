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

    // p points to the first array in ia
    for (auto p = begin(ia); p != end(ia); ++p) {
        // q points to the first element in an inner array
        for (auto q = begin(*p); q != end(*p); ++q) {
            cout << *q << ' ';   // prints the int value to which q points
        }
        cout << endl;
    }
}