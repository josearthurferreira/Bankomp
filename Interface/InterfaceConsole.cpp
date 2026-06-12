#include "InterfaceConsole.h"
#include <iostream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>
#include <algorithm>

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
    setStdinEcho(false);
    std::cin >> password;
    setStdinEcho(true);

    auto acc = bank->authenticate(number, password);
    if (acc) {
        std::cout << GREEN << "\nLogin efetuado com sucesso!\n" << RESET;
        handleUserSession(acc);
    } else {
        std::cout << RED << "\nConta ou senha incorretas.\n" << RESET;
    }
}

void InterfaceConsole::handleCreateAccount() {
    std::string name, cpf, password, passwordconfirm;
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
    setStdinEcho(false);
    std::cin >> password;
    setStdinEcho(true);
    std::cout << "\nConfirme sua Senha: ";
    setStdinEcho(false);
    std::cin >> passwordconfirm;
    setStdinEcho(true);

    if(password!=passwordconfirm){
        std::cout << RED << "\nA senha não corresponde!\n" << RESET;
        return;
    }


    int accountNumber = bank->createAccount(name, cpf, type, deposit, password, attr);

    if (accountNumber > 0) {
        std::cout << GREEN << "\n=======================================\n" << RESET;
        std::cout << GREEN << "      CONTA CRIADA COM SUCESSO!        \n" << RESET;
        std::cout << GREEN << "=======================================\n" << RESET;
        std::cout << " Titular: " << name << "\n";
        std::cout << " CPF: " << cpf << "\n";
        std::cout << " Tipo: " << (type == 1 ? "Conta Corrente" : "Conta Poupança") << "\n";
        std::cout << " Saldo Inicial: R$ " << std::fixed << std::setprecision(2) << deposit << "\n";
        std::cout << "---------------------------------------\n";
        std::cout << " NÚMERO DA CONTA: " << CYAN << accountNumber << RESET << "\n";
        std::cout << "---------------------------------------\n";
        std::cout << RED << " ATENÇÃO: Guarde este número para Login!\n" << RESET;
        std::cout << GREEN << "=======================================\n\n" << RESET;
    }
}

void InterfaceConsole::handleUserSession(std::shared_ptr<Account> acc) {
    int option = 0;
    while (true) {
        std::cout << BLUE << "\n--- Bem-vindo, " << acc->getClient()->getName() << " ---\n" << RESET;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Saldo Flexível: R$ " << acc->getBalance() << "\n";
        std::cout << "1. Depósito\n";
        std::cout << "2. Saque\n";
        std::cout << "3. Transferência Bancária\n";
        std::cout << "4. Extrato de Transações\n";
        std::cout << "5. Logout (Sair da Conta)\n";
        std::cout << "Escolha uma opção: ";
        std::cin >> option;

        if (option == 1) {
            double val;
            std::cout << "Valor do depósito ou 0 para voltar): R$ ";
            std::cin >> val;
            if (val == 0) continue;

            if (val > 0) {
                acc->deposit(val);
                acc->addTransaction(4, val, "Depósito em Espécie");
                bank->saveToStorage();
                std::cout << GREEN << "Depósito realizado com sucesso!\n" << RESET;
            }
        } else if (option == 2) {
            double val;
            std::cout << "Valor do saque (ou 0 para voltar): R$ ";
            std::cin >> val;
            if (val == 0) continue;
            
            if (acc->withdraw(val)) {
                acc->addTransaction(1, val, "Saque em Dinheiro");
                bank->saveToStorage();
                std::cout << GREEN << "Saque realizado com sucesso!\n" << RESET;
            } else {
                std::cout << RED << "Saldo ou limite de crédito insuficiente.\n" << RESET;
            }
        } else if (option == 3) {
            std::cout << CYAN << "\n=== NOVA TRANSFERÊNCIA ===\n" << RESET;
            std::cout << "Favorecidos Recentes:\n";
            std::vector<std::string> uniqueDests;
            bool hasRecents = false;
            for (const auto& tx : acc->getTransactions()) {
                if (tx.type == 2) { // Apenas envios
                    if (std::find(uniqueDests.begin(), uniqueDests.end(), tx.details) == uniqueDests.end()) {
                        uniqueDests.push_back(tx.details);
                        std::cout << " • " << tx.details << "\n";
                        hasRecents = true;
                    }
                }
            }
            if (!hasRecents) std::cout << " (Nenhum histórico de transferência encontrado)\n";
            std::cout << "---------------------------------------\n";

            // Requisito: Botão voltar integrado
            int destNumber;
            std::cout << "Digite o número da conta destino (ou 0 para VOLTAR): ";
            std::cin >> destNumber;
            if (destNumber == 0) continue;

            auto destAcc = bank->findAccount(destNumber);
            if (!destAcc) {
                std::cout << RED << "Erro: Conta informada não existe no sistema.\n" << RESET;
                continue;
            }

            double val;
            std::cout << "Digite o valor a transferir (ou 0 para VOLTAR): R$ ";
            std::cin >> val;
            if (val <= 0) continue;

            // Requisito: Página de confirmação de dados segura
            std::cout << CYAN << "\n┌──────────────────────────────────────┐\n";
            std::cout << "│        CONFIRMAÇÃO DE ENVIO          │\n";
            std::cout << "└──────────────────────────────────────┘\n" << RESET;
            std::cout << " FAVORECIDO: " << destAcc->getClient()->getName() << "\n";
            std::cout << " CONTA:      " << destAcc->getNumber() << "\n";
            std::cout << " VALOR:      R$ " << val << "\n";
            std::cout << "───────────────────────────────────────\n";
            std::cout << "1. Confirmar e Enviar Dinheiro\n";
            std::cout << "2. Cancelar Transação e Voltar\n";
            std::cout << "Escolha uma opção: ";
            int confirm;
            std::cin >> confirm;

            if (confirm != 1) {
                std::cout << RED << "Transferência abortada pelo usuário.\n" << RESET;
                continue;
            }

            if (bank->transfer(acc->getNumber(), destNumber, val)) {
                std::cout << GREEN << "Transferência concluída com sucesso!\n" << RESET;
            } else {
                std::cout << RED << "Falha: Saldo ou limite de crédito insuficiente.\n" << RESET;
            }
        } else if (option == 4) {
            std::cout << CYAN << "\n=======================================\n";
            std::cout << "         EXTRATO DE TRANSAÇÕES         \n";
            std::cout << "=======================================\n" << RESET;
            
            const auto& txs = acc->getTransactions();
            if (txs.empty()) {
                std::cout << "  Nenhuma movimentação registrada.\n";
            } else {
                for (const auto& tx : txs) {
                    std::string label;
                    if (tx.type == 1) label = RED "[SAQUE]         " RESET;
                    else if (tx.type == 2) label = RED "[TX ENVIADA]    " RESET;
                    else if (tx.type == 3) label = GREEN "[TX RECEBIDA]   " RESET;
                    else if (tx.type == 4) label = GREEN "[DEPÓSITO]      " RESET;
                    
                    std::cout << label << "R$ " << std::setw(8) << tx.amount << " -> " << tx.details << "\n";
                }
            }
            std::cout << CYAN << "=======================================\n" << RESET;
        } 
        else if (option == 5) {
            break;
        }
    }
}

void InterfaceConsole::setStdinEcho(bool enable) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable) {
        tty.c_lflag &= ~ECHO; // Desativa o echo
    } else {
        tty.c_lflag |= ECHO; // Ativa o echo
    }
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}