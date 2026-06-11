#include "Bank.h"
#include "../Model/CurrentAccount.h"
#include "../Model/SavingAccount.h"
#include "../Storage/Cryptography.h"
#include <cstring>

Bank::Bank(std::shared_ptr<DataManager> dm) : dataManager(dm) {}

void Bank::loadFromStorage() {
    accounts.clear();
    auto records = dataManager->loadRecords();
    for (const auto& r : records) {
        auto client = std::make_shared<Client>(r.clientName, r.clientCpf);
        if (r.accountType == 1) {
            accounts.push_back(std::make_shared<CurrentAccount>(r.accountNumber, r.balance, client, r.passwordHash, r.specialLimitOrInterest));
        } else {
            accounts.push_back(std::make_shared<SavingAccount>(r.accountNumber, r.balance, client, r.passwordHash, r.specialLimitOrInterest));
        }
    }
}

void Bank::saveToStorage() {
    std::vector<RecordData> records;
    for (const auto& acc : accounts) {
        RecordData r;
        std::strncpy(r.clientName, acc->getClient()->getName().c_str(), sizeof(r.clientName));
        std::strncpy(r.clientCpf, acc->getClient()->getCpf().c_str(), sizeof(r.clientCpf));
        r.accountNumber = acc->getNumber();
        r.balance = acc->getBalance();
        r.accountType = acc->getType();
        r.specialLimitOrInterest = acc->getSpecialAttribute();
        std::strncpy(r.passwordHash, acc->getPasswordHash().c_str(), sizeof(r.passwordHash));
        records.push_back(r);
    }
    dataManager->saveRecords(records);
}

int Bank::createAccount(const std::string& name, const std::string& cpf, int type, double initialDeposit, const std::string& password, double specialAttr) {
    int number = 1000 + accounts.size(); 
    auto client = std::make_shared<Client>(name, cpf);
    std::string hash = Cryptography::generateHash(password);
    
    if (type == 1) {
        accounts.push_back(std::make_shared<CurrentAccount>(number, initialDeposit, client, hash, specialAttr));
    } else {
        accounts.push_back(std::make_shared<SavingAccount>(number, initialDeposit, client, hash, specialAttr));
    }
    saveToStorage();
    return number;
}

std::shared_ptr<Account> Bank::authenticate(int number, const std::string& password) {
    auto acc = findAccount(number);
    if (acc && acc->getPasswordHash() == Cryptography::generateHash(password)) {
        return acc;
    }
    return nullptr;
}

std::shared_ptr<Account> Bank::findAccount(int number) {
    for (auto acc : accounts) {
        if (acc->getNumber() == number) return acc;
    }
    return nullptr;
}

bool Bank::transfer(int fromNumber, int toNumber, double amount) {
    auto from = findAccount(fromNumber);
    auto to = findAccount(toNumber);
    if (from && to && from->withdraw(amount)) {
        to->deposit(amount);
        saveToStorage();
        return true;
    }
    return false;
}