#ifndef SCREEN_H
#define SCREEN_H

#include <string>
#include "Window_mgr.h"

extern std::ostream& storeOn(std::ostrem &, Screen &);
extern std::BitMap& storeOn(BitMap &, Screen &);

class Screen {
    // Window_mgr::clear must have been declared before class Screen
    friend void Window_mgr::clear(ScreenIndex);
    // ostream version of storeOn may access the private parts of Screen objects
    friend std::ostream& storeOn(std::ostream &, Screen &);
    friend class Window_mgr;
public:
    typedef std::string::size_type pos;
    Screen() = default;  // needed because Screen has another contructor
    // cursor initialized to 0 by its in-class initializer
    Screen(pos ht, pos wd, char c): height(ht), width(wd),
                                    contents(ht * wd, c) { }
    char get() const                  // get the character at the cursor
        { return contents[cursor]; }  // implicity inline
    inline char get(pos ht, pos wd) const;  // explicity inline
    Screen &move(pos r, pos c);       // can be made inline later
    Screen &set(char);
    Screen &set(pos, pos, char);
private:
    pos cursor = 0;
    pos height = 0, width = 0;
    std::string contents;
};

inline                    // we can specify inline on the definition
Screen &Screen::move(pos r, pos c)
{
    pos row = r * width;  // compute the row location
    cursor = row + c;     // move cursor to the column within that row
    return *this;         // return this object as an lvalue
}

char Screen::get(pos r, pos c) const  // declared as inline in the class
{
    pos row = r * width;       // compute row location
    return contents[row + c];  // return character at the given column
}

inline Screen &Screen::set(char c)
{
    contents[cursor] = c;  // set the new value at the current cursor location
    return *this;          // return this object as an lvalue
}

inline Screen &Screen::set(pos r, pos col, char ch)
{
    contents[r*width + col] = ch;   // set specified locationi to given value
    return *this;                   // return this object as an lvalue
}

#endif