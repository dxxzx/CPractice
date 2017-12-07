#include <iostream>
#include <string>

using namespace std;

int main() {
	string s("some string");
	// process characters in s until we run out of characters or we hit a whitespace
	for (decltype(s.size()) index = 0;
		index != s.size() && !isspace(s[index]); ++index) {
		s[index] = toupper(s[index]);   // capitalize the current character
	}
	cout << s << endl;
	return 0;
}