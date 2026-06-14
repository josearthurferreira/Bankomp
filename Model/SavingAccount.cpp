#include "SavingAccount.h"

SavingAccount::SavingAccount(int number, double balance, std::shared_ptr<Client> client, const std::string& hash, double rate, int tier, double income)
    : Account(number, balance, client, hash, tier, income), interestRate(rate) {}

bool SavingAccount::withdraw(double amount) {
    if (amount > 0 && balance >= amount) {
        balance -= amount;
        return true;
    }
    return false;
}

void SavingAccount::applyInterest() {
    balance += balance * interestRate;
}