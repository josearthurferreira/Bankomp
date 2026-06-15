#include "SavingAccount.h"
#include <ctime>
#include <vector>
#include <string>

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

void SavingAccount::updateAccountState() {
    double buckets[29] = {0.0};
    std::time_t lastYieldTime[29] = {0};
    std::time_t firstDepositTime[29] = {0};

    const auto& txs = getTransactions();
    for (const auto& tx : txs) {
        std::time_t t = tx.timestamp;
        struct tm* timeinfo = std::localtime(&t);
        if (!timeinfo) continue;

        int d = timeinfo->tm_mday;
        if (d > 28) d = 1;

        if (tx.type == 4 || tx.type == 3) {
            buckets[d] += tx.amount;
            if (firstDepositTime[d] == 0) {
                firstDepositTime[d] = t;
            }
        } 
        else if (tx.type == 1 || tx.type == 2) { // Saque ou Transferência Enviada
            double toDeduct = tx.amount;
            for (int i = 1; i <= 28 && toDeduct > 0; ++i) {
                if (buckets[i] > 0) {
                    if (buckets[i] >= toDeduct) {
                        buckets[i] -= toDeduct;
                        toDeduct = 0;
                    } else {
                        toDeduct -= buckets[i];
                        buckets[i] = 0;
                    }
                }
            }
        } 
        else if (tx.type == 5) {
            buckets[d] += tx.amount;
            lastYieldTime[d] = t;
        }
    }

    std::time_t now = std::time(nullptr);
    struct tm* nowTm = std::localtime(&now);
    int nowDay = nowTm->tm_mday;
    int nowMonth = nowTm->tm_mon + 1;
    int nowYear = nowTm->tm_year + 1900;

    for (int d = 1; d <= 28; ++d) {
        if (buckets[d] <= 0) continue;

        std::time_t startTime = (lastYieldTime[d] != 0) ? lastYieldTime[d] : firstDepositTime[d];
        if (startTime == 0) continue;

        struct tm* startTm = std::localtime(&startTime);
        int startMonth = startTm->tm_mon + 1;
        int startYear = startTm->tm_year + 1900;

        int monthsPassed = (nowYear - startYear) * 12 + (nowMonth - startMonth);
        
        int simMonth = startMonth;
        int simYear = startYear;

        for (int m = 1; m <= monthsPassed; ++m) {
            simMonth++;
            if (simMonth > 12) {
                simMonth = 1;
                simYear++;
            }

            if (simYear == nowYear && simMonth == nowMonth && nowDay < d) {
                break;
            }

            double interestRate = getSpecialAttribute();
            double yieldAmount = buckets[d] * interestRate;

            if (yieldAmount > 0) {
                deposit(yieldAmount);

                struct tm yieldTm = {0};
                yieldTm.tm_mday = d;
                yieldTm.tm_mon = simMonth - 1;
                yieldTm.tm_year = simYear - 1900;
                std::time_t yieldTimestamp = std::mktime(&yieldTm);

                addTransaction(5, yieldAmount, "Rendimento Poupanca (Dia " + std::to_string(d) + ")", yieldTimestamp);
                
                buckets[d] += yieldAmount; 
            }
        }
    }
}