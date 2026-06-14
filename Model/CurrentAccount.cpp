#include "CurrentAccount.h"

CurrentAccount::CurrentAccount(int number, double balance, std::shared_ptr<Client> client, const std::string& hash, double limit, int tier, double income)
    : Account(number, balance, client, hash, tier, income), overdraftLimit(limit) {}

bool CurrentAccount::withdraw(double amount) {
    if (amount > 0 && (balance + overdraftLimit) >= amount) {
        balance -= amount;
        return true;
    }
    return false;
}