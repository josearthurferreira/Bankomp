#ifndef INTERFACE_USER_H
#define INTERFACE_USER_H

class InterfaceUser {
public:
    virtual ~InterfaceUser() = default;
    virtual void showMenu() = 0;
};

#endif