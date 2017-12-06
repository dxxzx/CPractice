#ifndef ACCOUNT_H
#define ACCOUNT_H

typedef double Money;

class Account {
public:
    Money balance() { return bal; }  // uses Money from the outer scope
private:
    Money bal;
};

#endif