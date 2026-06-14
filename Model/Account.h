#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <vector>
#include <string>
#include "Client.h"
#include <memory>

struct Transaction {
    int type;
    double amount;
    std::string details;
};

class Account {
protected:
    int number;
    double balance;
    std::shared_ptr<Client> client;
    std::string passwordHash;
    std::vector<Transaction> transactions;
    int tier;
    double income;
public:
    Account(int number, double balance, std::shared_ptr<Client> client, const std::string& hash, int tier = 1, double income = 0.0);
    virtual ~Account() = default;

    int getNumber() const;
    double getBalance() const;
    std::shared_ptr<Client> getClient() const;
    std::string getPasswordHash() const;

    void deposit(double amount);
    virtual bool withdraw(double amount) = 0;
    virtual int getType() const = 0;
    virtual double getSpecialAttribute() const = 0;

    void addTransaction(int type, double amount, const std::string& details);
    const std::vector<Transaction>& getTransactions() const;
    void clearTransactions();

    int getTier() const;
    double getIncome() const;
    std::string getTierName() const;
};

#endif