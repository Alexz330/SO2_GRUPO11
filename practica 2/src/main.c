

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "users/users.h"
#include "operations/operations.h"

void display_menu();
void handle_massive_user_load();
void handle_massive_transaction_load();
void handle_deposit();
void handle_withdrawal();
void handle_transfer();
void handle_query();
void handle_account_report();
void handle_user_errors_report();
void handle_transaction_errors_report();

int main() {
    int choice;

    while (1) {
        display_menu();
        printf("Seleccione una opción: ");
        if (scanf("%d", &choice) != 1) {
            printf("Entrada inválida. Intente de nuevo.\n");
            while (getchar() != '\n');  // Clear the buffer
            continue;
        }

        switch (choice) {
            case 1:
                handle_massive_user_load();
                break;
            case 2:
                handle_massive_transaction_load();
                break;
            case 3:
                handle_deposit();
                break;
            case 4:
                handle_withdrawal();
                break;
            case 5:
                handle_transfer();
                break;
            case 6:
                handle_query();
                break;
            case 7:
                handle_account_report();
                break;

            case 8:
                printf("Saliendo...\n");
                exit(0);
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    }

    return 0;
}

void display_menu() {
    printf("\n--- Menú de Operaciones ---\n");
    printf("1. Carga masiva de usuarios\n");
    printf("2. Carga masiva de transacciones\n");
    printf("3. Depósito\n");
    printf("4. Retiro\n");
    printf("5. Transferencia\n");
    printf("6. Consultar cuenta\n");
    printf("7. Generar reporte en json de cuentas\n");

    printf("8. Salir\n");
}

void handle_massive_user_load() {
    char file_path[100];
    printf("Ingrese la ruta del archivo JSON de usuarios: ");
    scanf("%s", file_path);
    load_accounts_from_json(file_path);
    printf("Carga masiva de usuarios completada.\n");
}

void handle_massive_transaction_load() {
    char file_path[100];
    printf("Ingrese la ruta del archivo JSON de transacciones: ");
    scanf("%s", file_path);
    load_operations_from_json(file_path);
    printf("Carga masiva de transacciones completada.\n");
}

void handle_deposit() {
    int account_number;
    double amount;

    printf("Ingrese el número de cuenta: ");
    if (scanf("%d", &account_number) != 1 || account_number <= 0) {
        printf("Número de cuenta no válido.\n");
        while (getchar() != '\n');  // Clear the buffer
        return;
    }

    printf("Ingrese el monto a depositar: ");
    if (scanf("%lf", &amount) != 1 || amount <= 0) {
        printf("Monto no válido.\n");
        while (getchar() != '\n');  // Clear the buffer
        return;
    }

    int account_idx = find_account_index(account_number);
    if (account_idx == -1) {
        printf("Número de cuenta no existe.\n");
        return;
    }

    process_deposit_individual(account_idx, amount);
    printf("Depósito realizado con éxito.\n");
}

void handle_withdrawal() {
    int account_number;
    double amount;

    printf("Ingrese el número de cuenta: ");
    if (scanf("%d", &account_number) != 1 || account_number <= 0) {
        printf("Número de cuenta no válido.\n");
        while (getchar() != '\n');  // Clear the buffer
        return;
    }

    printf("Ingrese el monto a retirar: ");
    if (scanf("%lf", &amount) != 1 || amount <= 0) {
        printf("Monto no válido.\n");
        while (getchar() != '\n');  // Clear the buffer
        return;
    }

    int account_idx = find_account_index(account_number);
    if (account_idx == -1) {
        printf("Número de cuenta no existe.\n");
        return;
    }

    if (accounts[account_idx].balance < amount) {
        printf("Saldo insuficiente para retiro.\n");
        return;
    }

    process_withdrawal_individual(account_idx, amount);
    printf("Retiro realizado con éxito.\n");
}

void handle_transfer() {
    int account_number_from, account_number_to;
    double amount;

    printf("Ingrese el número de cuenta de origen: ");
    if (scanf("%d", &account_number_from) != 1 || account_number_from <= 0) {
        printf("Número de cuenta de origen no válido.\n");
        while (getchar() != '\n');  // Clear the buffer
        return;
    }

    printf("Ingrese el número de cuenta de destino: ");
    if (scanf("%d", &account_number_to) != 1 || account_number_to <= 0) {
        printf("Número de cuenta de destino no válido.\n");
        while (getchar() != '\n');  // Clear the buffer
        return;
    }

    printf("Ingrese el monto a transferir: ");
    if (scanf("%lf", &amount) != 1 || amount <= 0) {
        printf("Monto no válido.\n");
        while (getchar() != '\n');  // Clear the buffer
        return;
    }

    int account_idx_from = find_account_index(account_number_from);
    int account_idx_to = find_account_index(account_number_to);

    if (account_idx_from == -1) {
        printf("Número de cuenta de origen no existe.\n");
        return;
    }

    if (account_idx_to == -1) {
        printf("Número de cuenta de destino no existe.\n");
        return;
    }

    if (accounts[account_idx_from].balance < amount) {
        printf("Saldo insuficiente para transferencia.\n");
        return;
    }

    process_transfer_individual(account_idx_from, account_idx_to, amount);
    printf("Transferencia realizada con éxito.\n");
}

void handle_query() {
    int account_number;

    printf("Ingrese el número de cuenta: ");
    if (scanf("%d", &account_number) != 1 || account_number <= 0) {
        printf("Número de cuenta no válido.\n");
        while (getchar() != '\n');  // Clear the buffer
        return;
    }

    int account_idx = find_account_index(account_number);
    if (account_idx == -1) {
        printf("Número de cuenta no existe.\n");
        return;
    }

    USER account = accounts[account_idx];
    printf("Número de cuenta: %d\n", account.account_number);
    printf("Nombre: %s\n", account.name);
    printf("Saldo: %.2f\n", account.balance);
}

void handle_account_report() {
  generate_user_report();
}

void handle_user_errors_report() {
    printf("\n--- Reporte de Errores de Usuarios ---\n");
    report_invalid_accounts();
}
