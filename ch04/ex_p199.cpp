#include <iostream>
#include <string>

using namespace std;

int main()
{
  string text[3] = {
    "hello world.",
    "apple.",
    "pear."
  };
  // note s as a reference to const; the elements aren't copied and can't be changed
  for (const auto &s : text) {  // for each element in text
    cout << s;  // print the current element
    // blank lines and those that end with a period get a newline
    if (s.empty() || s[s.size() - 1] == '.')
      cout << endl;
    else
      cout << " ";  // otherwise just separate with a space
  }
}

