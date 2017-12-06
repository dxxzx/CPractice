#include <iostream>

class Screen {
public:
    // display overloaded on whether the object is const or not
    Screen &display(std::ostream &os)
                   { do_display(os); return *this; }
    const Screen &display(std::ostream &os) const
                   { do_display(os); return *this; }
private:
    // function to do the work of displaying a Screen
    void do_display(std::ostream &os) const { os << contents; }
}