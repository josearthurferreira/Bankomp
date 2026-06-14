#ifndef SAVING_ACCOUNT_H
#define SAVING_ACCOUNT_H
#include "Account.h"

class SavingAccount : public Account {
private:
    double interestRate;
public:
    SavingAccount(int number, double balance, std::shared_ptr<Client> client, const std::string& hash, double rate, int tier = 1, double income = 0.0);
    bool withdraw(double amount) override;
    void applyInterest();
    int getType() const override { return 2; }
    double getSpecialAttribute() const override { return interestRate; }
};

#endif