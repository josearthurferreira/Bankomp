#include "InterfaceConsole.h"
#include <iostream>
#include <iomanip>

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"

InterfaceConsole::InterfaceConsole(std::shared_ptr<Bank> b) : bank(b) {}

void InterfaceConsole::showMenu() {
    int option = 0;
    while (true) {
        std::cout << CYAN << "\n=== BANKOMP - SISTEMA BANCÁRIO ===\n" << RESET;
        std::cout << "1. Acessar Minha Conta\n";
        std::cout << "2. Abrir Nova Conta\n";
        std::cout << "3. Sair\n";
        std::cout << "Escolha uma opção: ";
        std::cin >> option;

        if (option == 1) handleLogin();
        else if (option == 2) handleCreateAccount();
        else if (option == 3) break;
        else std::cout << RED << "Opção inválida!\n" << RESET;
    }
}

void InterfaceConsole::handleLogin() {
    int number;
    std::string password;
    std::cout << "\nNúmero da Conta: ";
    std::cin >> number;
    std::cout << "Senha: ";
    std::cin >> password;

    auto acc = bank->authenticate(number, password);
    if (acc) {
        std::cout << GREEN << "\nLogin efetuado com sucesso!\n" << RESET;
        handleUserSession(acc);
    } else {
        std::cout << RED << "\nConta ou senha incorretas.\n" << RESET;
    }
}

void InterfaceConsole::handleCreateAccount() {
    std::string name, cpf, password;
    int type;
    double deposit, attr;

    std::cin.ignore();
    std::cout << "\nNome do Titular: ";
    std::getline(std::cin, name);
    std::cout << "CPF: ";
    std::cin >> cpf;
    std::cout << "Tipo (1 - Corrente, 2 - Poupança): ";
    std::cin >> type;
    std::cout << "Depósito Inicial: R$ ";
    std::cin >> deposit;
    
    if (type == 1) {
        std::cout << "Limite do Cheque Especial: R$ ";
        std::cin >> attr;
    } else {
        std::cout << "Taxa de Rendimento Anual (ex: 0.05 para 5%): ";
        std::cin >> attr;
    }
    std::cout << "Crie uma Senha: ";
    std::cin >> password;

    if (bank->createAccount(name, cpf, type, deposit, password, attr)) {
        std::cout << GREEN << "\nConta criada com sucesso!\n" << RESET;
    }
}

void InterfaceConsole::handleUserSession(std::shared_ptr<Account> acc) {
    int option = 0;
    while (true) {
        std::cout << BLUE << "\n--- Bem-vindo, " << acc->getClient()->getName() << " ---\n" << RESET;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Saldo Atual: R$ " << acc->getBalance() << "\n";
        std::cout << "1. Depósito\n";
        std::cout << "2. Saque\n";
        std::cout << "3. Transferência\n";
        std::cout << "4. Logout\n";
        std::cout << "Escolha uma opção: ";
        std::cin >> option;

        if (option == 1) {
            double val;
            std::cout << "Valor do depósito: R$ ";
            std::cin >> val;
            acc->deposit(val);
            bank->saveToStorage();
            std::cout << GREEN << "Depósito realizado!\n" << RESET;
        } else if (option == 2) {
            double val;
            std::cout << "Valor do saque: R$ ";
            std::cin >> val;
            if (acc->withdraw(val)) {
                bank->saveToStorage();
                std::cout << GREEN << "Saque realizado!\n" << RESET;
            } else {
                std::cout << RED << "Saldo/Limite insuficiente.\n" << RESET;
            }
        } else if (option == 3) {
            int dest;
            double val;
            std::cout << "Conta de destino: ";
            std::cin >> dest;
            std::cout << "Valor da transferência: R$ ";
            std::cin >> val;
            if (bank->transfer(acc->getNumber(), dest, val)) {
                std::cout << GREEN << "Transferência concluída!\n" << RESET;
            } else {
                std::cout << RED << "Falha na transferência.\n" << RESET;
            }
        } else if (option == 4) {
            break;
        }
    }
}