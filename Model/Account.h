#ifndef ACCOUNT_H
#define ACCOUNT_H
#include "Client.h"
#include <memory>

class Account {
protected:
    int number;
    double balance;
    std::shared_ptr<Client> client;
    std::string passwordHash;
public:
    Account(int number, double balance, std::shared_ptr<Client> client, const std::string& hash);
    virtual ~Account() = default;

    int getNumber() const;
    double getBalance() const;
    std::shared_ptr<Client> getClient() const;
    std::string getPasswordHash() const;

    void deposit(double amount);
    virtual bool withdraw(double amount) = 0;
    virtual int getType() const = 0;
    virtual double getSpecialAttribute() const = 0;
};

#endif