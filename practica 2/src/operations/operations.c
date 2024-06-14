
#include "operations.h"
#include "../users/users.h" // Incluye users.h para las variables globales
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_THREADS 4
#define MAX_ACCOUNTS 1000

int thread_usage[NUM_THREADS] = {0};
OPERATION invalid_operations[MAX_ACCOUNTS];
int invalid_operation_count = 0;

pthread_mutex_t operation_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t operation_sem;

typedef struct {
    struct json_object *json_operations;
    int thread_id;
    int start_idx;
    int end_idx;
} ThreadArgs;

void* thread_function_operation(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    struct json_object *json_operations = args->json_operations;

    for (int i = args->start_idx; i < args->end_idx; i++) {
        struct json_object *json_operation = json_object_array_get_idx(json_operations, i);
        process_operation(json_operation, i + 1, args->thread_id); // i + 1 para obtener el número de línea
    }

    free(arg);
    return NULL;
}


void generate_operation_report() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char filename[100];
    strftime(filename, sizeof(filename), "operaciones_%Y_%m_%d-%H_%M_%S.log", t);

    FILE *report_file = fopen(filename, "w");
    if (!report_file) {
        perror("Cannot open report file");
        return;
    }

    fprintf(report_file, "--- Reporte de Operaciones Masivas ---\n");
    int total_processed = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        fprintf(report_file, "Hilo %d procesó %d operaciones\n", i, thread_usage[i]);
        total_processed += thread_usage[i];
    }
    fprintf(report_file, "Total de operaciones procesadas por todos los hilos: %d\n\n", total_processed);

    fprintf(report_file, "Operaciones inválidas:\n");
    for (int i = 0; i < invalid_operation_count; i++) {
        fprintf(report_file, "Línea: %d, Operación: %d, Cuenta1: %d, Cuenta2: %d, Monto: %.2f, Error: %s\n",
               invalid_operations[i].line_number, // Ajusta esto según tu estructura OPERATION
               invalid_operations[i].operation,
               invalid_operations[i].cuenta1,
               invalid_operations[i].cuenta2,
               invalid_operations[i].monto,
               invalid_operations[i].error_msg);
    }

    fclose(report_file);
}

void load_operations_from_json(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Cannot open file");
        return;
    }

    struct json_object *parsed_json;
    char *buffer;
    long file_size;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return;
    }

    fread(buffer, file_size, 1, file);
    buffer[file_size] = '\0';
    fclose(file);

    parsed_json = json_tokener_parse(buffer);
    free(buffer);

    if (json_object_get_type(parsed_json) != json_type_array) {
        fprintf(stderr, "Error: JSON is not an array.\n");
        return;
    }

    sem_init(&operation_sem, 0, NUM_THREADS);

    pthread_t threads[NUM_THREADS];
    size_t n_operations = json_object_array_length(parsed_json);
    size_t chunk_size = (n_operations + NUM_THREADS - 1) / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->json_operations = parsed_json;
        args->thread_id = i;
        args->start_idx = i * chunk_size;
        args->end_idx = (i + 1) * chunk_size < n_operations ? (i + 1) * chunk_size : n_operations;
        pthread_create(&threads[i], NULL, thread_function_operation, args);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&operation_sem);
    json_object_put(parsed_json);

    // Generar el reporte después de la carga
    generate_operation_report();
}
void log_invalid_operation(int operacion, int cuenta1, int cuenta2, double monto, const char *error_msg, int line_number) {
    invalid_operations[invalid_operation_count].line_number = line_number; // Asigna el número de línea actual
    invalid_operations[invalid_operation_count].operation = operacion;
    invalid_operations[invalid_operation_count].cuenta1 = cuenta1;
    invalid_operations[invalid_operation_count].cuenta2 = cuenta2;
    invalid_operations[invalid_operation_count].monto = monto;
    strncpy(invalid_operations[invalid_operation_count].error_msg, error_msg, sizeof(invalid_operations[invalid_operation_count].error_msg));
    invalid_operation_count++;
}

void process_operation(struct json_object *json_operation, int line_number, int thread_id) {
    struct json_object *json_operacion, *json_cuenta1, *json_cuenta2, *json_monto;
    json_object_object_get_ex(json_operation, "operacion", &json_operacion);
    json_object_object_get_ex(json_operation, "cuenta1", &json_cuenta1);
    json_object_object_get_ex(json_operation, "cuenta2", &json_cuenta2);
    json_object_object_get_ex(json_operation, "monto", &json_monto);

    int operacion = json_object_get_int(json_operacion);
    int cuenta1 = json_object_get_int(json_cuenta1);
    int cuenta2 = json_object_get_int(json_cuenta2);
    double monto = json_object_get_double(json_monto);

    pthread_mutex_lock(&operation_mutex);

    if (cuenta1 <= 0 || monto <= 0) {
        log_invalid_operation(operacion, cuenta1, cuenta2, monto, "Monto no válido o número de cuenta no válido", line_number);
        cleanup_mutex_sem();
        return;
    }

    int cuenta1_idx = find_account_index(cuenta1);
    int cuenta2_idx = find_account_index(cuenta2);

    if (!validate_account_exists(cuenta1_idx, cuenta1, "Número de cuenta1 no existe",line_number)) {
        cleanup_mutex_sem();
        return;
    }

    switch (operacion) {
        case 1: // Depósito
            process_deposit(cuenta1_idx, monto, thread_id);
            break;
        case 2: // Retiro
            process_withdrawal(cuenta1_idx, monto, thread_id,line_number);
            break;
        case 3: // Transferencia
            process_transfer(cuenta1_idx, cuenta2_idx, monto, thread_id,line_number);
            break;
        default:
            log_invalid_operation(operacion, cuenta1, cuenta2, monto, "Identificador de operación no existe", line_number);
            break;
    }

    cleanup_mutex_sem();
}

void process_deposit(int cuenta_idx, double monto, int thread_id) {
    accounts[cuenta_idx].balance += monto;
    thread_usage[thread_id]++;
}

void process_deposit_individual(int cuenta_idx, double monto) {
    accounts[cuenta_idx].balance += monto;
}

void process_withdrawal(int cuenta_idx, double monto, int thread_id,int line_number) {
    if (accounts[cuenta_idx].balance < monto) {
        log_invalid_operation(2, accounts[cuenta_idx].account_number, -1, monto, "Saldo insuficiente para retiro",line_number);
    } else {
        accounts[cuenta_idx].balance -= monto;
        thread_usage[thread_id]++;
    }
}

void process_withdrawal_individual(int cuenta_idx, double monto) {
    if (accounts[cuenta_idx].balance < monto) {
        // log_invalid_operation(2, accounts[cuenta_idx].account_number, -1, monto, "Saldo insuficiente para retiro");
    } else {
        accounts[cuenta_idx].balance -= monto;
    }
}

void process_transfer(int cuenta1_idx, int cuenta2_idx, double monto, int thread_id,int line_number) {
    if (!validate_account_exists(cuenta2_idx, accounts[cuenta1_idx].account_number, "Número de cuenta2 no existe",line_number)) {
        return;
    }
    if (accounts[cuenta1_idx].balance < monto) {
        log_invalid_operation(3, accounts[cuenta1_idx].account_number, accounts[cuenta2_idx].account_number, monto, "Saldo insuficiente para transferencia",line_number);
    } else {
        accounts[cuenta1_idx].balance -= monto;
        accounts[cuenta2_idx].balance += monto;
        thread_usage[thread_id]++;
    }
}

void process_transfer_individual(int cuenta1_idx, int cuenta2_idx, double monto) {
    if (!validate_account_exists(cuenta2_idx, accounts[cuenta1_idx].account_number, "Número de cuenta2 no existe",-1)) {
        return;
    }
    if (accounts[cuenta1_idx].balance < monto) {
        // log_invalid_operation(3, accounts[cuenta1_idx].account_number, accounts[cuenta2_idx].account_number, monto, "Saldo insuficiente para transferencia");
    } else {
        accounts[cuenta1_idx].balance -= monto;
        accounts[cuenta2_idx].balance += monto;
    }
}

int find_account_index(int account_number) {
    for (int i = 0; i < account_count; i++) {
        if (accounts[i].account_number == account_number) {
            return i;
        }
    }
    return -1;
}

int validate_account_exists(int idx, int account_number, const char *error_msg, int line_number) {
    if (idx == -1) {
        log_invalid_operation(-1, account_number, -1, 0, error_msg,line_number);
        return 0;
    }
    return 1;
}

void cleanup_mutex_sem() {
    pthread_mutex_unlock(&operation_mutex);
    sem_post(&operation_sem);
}
