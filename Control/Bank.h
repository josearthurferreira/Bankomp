#ifndef BANK_H
#define BANK_H
#include "../Model/Account.h"
#include "../Storage/DataManager.h"
#include <vector>
#include <memory>

class Bank {
private:
    std::vector<std::shared_ptr<Account>> accounts;
    std::shared_ptr<DataManager> dataManager;
public:
    Bank(std::shared_ptr<DataManager> dm);
    void loadFromStorage();
    void saveToStorage();

    bool createAccount(const std::string& name, const std::string& cpf, int type, double initialDeposit, const std::string& password, double specialAttr);
    std::shared_ptr<Account> authenticate(int number, const std::string& password);
    std::shared_ptr<Account> findAccount(int number);
    bool transfer(int fromNumber, int toNumber, double amount);
};

#endif