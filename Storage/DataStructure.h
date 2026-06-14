#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

struct RecordData {
    char clientName[50];
    char clientCpf[15];
    int accountNumber;
    double balance;
    int accountType; // 1 = Current, 2 = Saving
    double specialLimitOrInterest;
    char passwordHash[65];
    int tier;
    double income;
};

struct TransactionRecord {
    int accountNumber; 
    int type;          // 1 = Saque, 2 = Transf. Enviada, 3 = Transf. Recebida
    double amount;
    char details[100]; 
};

#endif