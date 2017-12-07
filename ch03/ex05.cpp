#include <iostream>

using namespace std;

int main()
{
    string line;
    // read input a line at a time until end-of-file
    while (getline(cin, line)) {
        cout << line << endl;
    }
    return 0;
}