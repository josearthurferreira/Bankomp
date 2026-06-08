#include "Client.h"

Client::Client(const std::string& name, const std::string& cpf) : name(name), cpf(cpf) {}
std::string Client::getName() const { return name; }
std::string Client::getCpf() const { return cpf; }