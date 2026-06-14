#ifndef CURRENT_ACCOUNT_H
#define CURRENT_ACCOUNT_H
#include "Account.h"

class CurrentAccount : public Account {
private:
    double overdraftLimit;
public:
    CurrentAccount(int number, double balance, std::shared_ptr<Client> client, const std::string& hash, double limit, int tier = 1, double income = 0.0);
    bool withdraw(double amount) override;
    int getType() const override { return 1; }
    double getSpecialAttribute() const override { return overdraftLimit; }
};

#endif