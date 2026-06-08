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
};

#endif