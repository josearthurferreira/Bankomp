#include "InterfaceConsole.h"
#include <iostream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>
#include <algorithm>
#include <limits> 
#include <regex> 

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"

InterfaceConsole::InterfaceConsole(std::shared_ptr<Bank> b) : bank(b) {}

int readIntSafe(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << RED << "Entrada inválida! Por favor, digite um número inteiro.\n" << RESET;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double readDoubleSafe(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << RED << "Entrada inválida! Por favor, digite um valor numérico válido.\n" << RESET;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string readStringSafe(const std::string& prompt, bool allowSpaces = false) {
    std::string value;
    while (true) {
        std::cout << prompt;
        if (allowSpaces) {
            std::getline(std::cin, value);
        } else {
            std::cin >> value;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (!value.empty()) {
            return value;
        }
        std::cout << RED << "O campo não pode ficar vazio.\n" << RESET;
    }
}

void InterfaceConsole::showMenu() {
    while (true) {
        std::cout << CYAN << "\n=== BANKOMP - SISTEMA BANCÁRIO ===\n" << RESET;
        std::cout << "1. Acessar Minha Conta\n";
        std::cout << "2. Abrir Nova Conta\n";
        std::cout << "3. Sair\n";
        
        int option = readIntSafe("Escolha uma opção: ");

        if (option == 1) handleLogin();
        else if (option == 2) handleCreateAccount();
        else if (option == 3) break;
        else std::cout << RED << "Opção inválida!\n" << RESET;
    }
}

void InterfaceConsole::handleLogin() {
    int number = readIntSafe("\nNúmero da Conta: ");
    
    std::cout << "Senha: ";
    setStdinEcho(false);
    std::string password = readStringSafe("");
    setStdinEcho(true);
    std::cout << "\n";

    auto acc = bank->authenticate(number, password);
    if (acc) {
        std::cout << GREEN << "Login efetuado com sucesso!\n" << RESET;
        handleUserSession(acc);
    } else {
        std::cout << RED << "Conta ou senha incorretas.\n" << RESET;
    }
}

void InterfaceConsole::handleCreateAccount() {
    std::string name, cpf, password, passwordconfirm;
    int type;
    double deposit;

    std::cout << CYAN << "\n=== ABERTURA DE NOVA CONTA ===\n" << RESET;
    
    name = readStringSafe("Nome do Titular: ", true);
    
    // 1. Validação de formato e unicidade do CPF
    std::regex cpfRegex(R"(\d{3}\.\d{3}\.\d{3}-\d{2})");
    while (true) {
        cpf = readStringSafe("CPF (formato XXX.XXX.XXX-XX ou 0 para voltar): ", false);
        if (cpf == "0") return;

        if (!std::regex_match(cpf, cpfRegex)) {
            std::cout << RED << "Formato inválido! Use os pontos e o traço.\n" << RESET;
            continue;
        }
        if (bank->isCpfRegistered(cpf)) {
            std::cout << RED << "Erro: Este CPF já possui uma conta no sistema!\n" << RESET;
            return; // Interrompe a criação, pois o cliente já é do banco
        }
        break;
    }
    
    while (true) {
        type = readIntSafe("Tipo (1 - Corrente, 2 - Poupança): ");
        if (type == 1 || type == 2) break;
        std::cout << RED << "Tipo inválido! Escolha 1 ou 2.\n" << RESET;
    }
    
    while (true) {
        deposit = readDoubleSafe("Depósito Inicial: R$ ");
        if (deposit >= 0) break;
        std::cout << RED << "O depósito inicial não pode ser negativo.\n" << RESET;
    }
    
    // 2. Removemos os loops que pediam os valores de attr manualmente!
    
    while (true) {
        std::cout << "Crie uma Senha: ";
        setStdinEcho(false);
        password = readStringSafe("");
        setStdinEcho(true);
        
        std::cout << "\nConfirme sua Senha: ";
        setStdinEcho(false);
        passwordconfirm = readStringSafe("");
        setStdinEcho(true);
        std::cout << "\n";

        if (password == passwordconfirm) break;
        std::cout << RED << "As senhas não correspondem! Tente novamente.\n" << RESET;
    }

    // 3. Chamada atualizada sem passar o 'attr'
    int accountNumber = bank->createAccount(name, cpf, type, deposit, password);

    if (accountNumber > 0) {
        std::cout << GREEN << "\n=======================================\n" << RESET;
        std::cout << GREEN << "      CONTA CRIADA COM SUCESSO!        \n" << RESET;
        std::cout << GREEN << "=======================================\n" << RESET;
        std::cout << " Titular: " << name << "\n";
        std::cout << " CPF: " << cpf << "\n";
        std::cout << " Tipo: " << (type == 1 ? "Conta Corrente (Limite Pré-Aprovado: R$ 500,00)" : "Conta Poupança (Rendimento: 5% a.a.)") << "\n";
        std::cout << " Saldo Inicial: R$ " << std::fixed << std::setprecision(2) << deposit << "\n";
        std::cout << "---------------------------------------\n";
        std::cout << " NÚMERO DA CONTA: " << CYAN << accountNumber << RESET << "\n";
        std::cout << "---------------------------------------\n";
        std::cout << RED << " ATENÇÃO: Guarde este número para Login!\n" << RESET;
        std::cout << GREEN << "=======================================\n\n" << RESET;
    }
}

void InterfaceConsole::handleUserSession(std::shared_ptr<Account> acc) {
    while (true) {
        std::cout << BLUE << "\n--- Bem-vindo, " << acc->getClient()->getName() << " ---\n" << RESET;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Saldo Flexível: R$ " << acc->getBalance() << "\n";
        std::cout << "1. Depósito\n";
        std::cout << "2. Saque\n";
        std::cout << "3. Transferência Bancária\n";
        std::cout << "4. Extrato de Transações\n";
        std::cout << "5. Logout (Sair da Conta)\n";
        
        int option = readIntSafe("Escolha uma opção: ");

        if (option == 1) {
            double val = readDoubleSafe("Valor do depósito (ou 0 para voltar): R$ ");
            if (val == 0) continue;

            if (val > 0) {
                acc->deposit(val);
                acc->addTransaction(4, val, "Depósito em Espécie");
                bank->saveToStorage();
                std::cout << GREEN << "Depósito realizado com sucesso!\n" << RESET;
            } else {
                std::cout << RED << "Valor inválido para depósito.\n" << RESET;
            }
        } else if (option == 2) {
            double val = readDoubleSafe("Valor do saque (ou 0 para voltar): R$ ");
            if (val == 0) continue;
            
            if (val > 0 && acc->withdraw(val)) {
                acc->addTransaction(1, val, "Saque em Dinheiro");
                bank->saveToStorage();
                std::cout << GREEN << "Saque realizado com sucesso!\n" << RESET;
            } else {
                std::cout << RED << "Saldo ou limite de crédito insuficiente ou valor inválido.\n" << RESET;
            }
        } else if (option == 3) {
            std::cout << CYAN << "\n=== NOVA TRANSFERÊNCIA ===\n" << RESET;
            std::cout << "Favorecidos Recentes:\n";
            std::vector<std::string> uniqueDests;
            bool hasRecents = false;
            for (const auto& tx : acc->getTransactions()) {
                if (tx.type == 2) { 
                    if (std::find(uniqueDests.begin(), uniqueDests.end(), tx.details) == uniqueDests.end()) {
                        uniqueDests.push_back(tx.details);
                        std::cout << " • " << tx.details << "\n";
                        hasRecents = true;
                    }
                }
            }
            if (!hasRecents) std::cout << " (Nenhum histórico de transferência encontrado)\n";
            std::cout << "---------------------------------------\n";

            int destNumber = readIntSafe("Digite o número da conta destino (ou 0 para VOLTAR): ");
            if (destNumber == 0) continue;

            auto destAcc = bank->findAccount(destNumber);
            if (!destAcc) {
                std::cout << RED << "Erro: Conta informada não existe no sistema.\n" << RESET;
                continue;
            }

            double val = readDoubleSafe("Digite o valor a transferir (ou 0 para VOLTAR): R$ ");
            if (val <= 0) continue;

            std::cout << CYAN << "\n┌──────────────────────────────────────┐\n";
            std::cout << "│        CONFIRMAÇÃO DE ENVIO          │\n";
            std::cout << "└──────────────────────────────────────┘\n" << RESET;
            std::cout << " FAVORECIDO: " << destAcc->getClient()->getName() << "\n";
            std::cout << " CONTA:      " << destAcc->getNumber() << "\n";
            std::cout << " VALOR:      R$ " << val << "\n";
            std::cout << "───────────────────────────────────────\n";
            std::cout << "1. Confirmar e Enviar Dinheiro\n";
            std::cout << "2. Cancelar Transação e Voltar\n";
            
            int confirm = readIntSafe("Escolha uma opção: ");

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
        } else if (option == 5) {
            break;
        } else {
            std::cout << RED << "Opção Inválida!n" << RESET;
        }
    }
}

void InterfaceConsole::setStdinEcho(bool enable) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable) {
        tty.c_lflag &= ~ECHO;
    } else {
        tty.c_lflag |= ECHO;
    }
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}