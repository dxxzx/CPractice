#include <iostream>
#include <string>

using namespace std;

int main()
{
	string s("Hello world!!!");
	// convert s to uppercase
	for (auto &ch : s) {       // for every char in s(note: c is a reference)
		ch = toupper(ch);       // c is a reference, so the assignment changes the char in s
	}
	cout << s << endl;
	return 0;
}
