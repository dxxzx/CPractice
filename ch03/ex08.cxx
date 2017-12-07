#include <iostream>
#include <string>

using namespace std;

int main()
{
	string s = "some string";
	if (!s.empty()) {
		s[0] = toupper(s[0]);
	}
	cout << s << endl;
	return 0;
}

