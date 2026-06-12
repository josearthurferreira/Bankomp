#include "Account.h"

Account::Account(int number, double balance, std::shared_ptr<Client> client, const std::string& hash)
    : number(number), balance(balance), client(client), passwordHash(hash) {}

int Account::getNumber() const { return number; }
double Account::getBalance() const { return balance; }
std::shared_ptr<Client> Account::getClient() const { return client; }
std::string Account::getPasswordHash() const { return passwordHash; }

void Account::deposit(double amount) {
    if (amount > 0) balance += amount;
}

void Account::addTransaction(int type, double amount, const std::string& details) {
    transactions.push_back({type, amount, details});
}

const std::vector<Transaction>& Account::getTransactions() const { return transactions; }

void Account::clearTransactions() { transactions.clear(); }