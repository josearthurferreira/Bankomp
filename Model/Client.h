#ifndef CLIENT_H
#define CLIENT_H
#include <string>

class Client {
private:
    std::string name;
    std::string cpf;
public:
    Client(const std::string& name, const std::string& cpf);
    std::string getName() const;
    std::string getCpf() const;
};

#endif