// user.c
#include "users.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <json-c/json.h>

#define MAX_ACCOUNTS 1000
#define NUM_THREADS 3

USER accounts[MAX_ACCOUNTS];
ERROR errors[MAX_ERRORS];

int account_count = 0;
int error_count = 0;

static int thread_usage[NUM_THREADS] = {0};
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t account_sem;

static USER invalid_accounts[MAX_ACCOUNTS];
static int invalid_account_count = 0;

typedef struct {
    struct json_object *json_accounts;
    int thread_id;
    int start_idx;
    int end_idx;
} ThreadArgs;


void generate_report() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char filename[100];
    strftime(filename, sizeof(filename), "carga_%Y_%m_%d-%H_%M_%S.log", t);

    FILE *report_file = fopen(filename, "w");
    if (!report_file) {
        perror("Cannot open report file");
        return;
    }

    fprintf(report_file, "--- Reporte de Carga Masiva de usuarios---\n");
    int total_processed = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        fprintf(report_file, "Hilo %d procesó %d cuentas\n", i, thread_usage[i]);
        total_processed += thread_usage[i];
    }
    fprintf(report_file, "Total de cuentas procesadas por todos los hilos: %d\n", total_processed);

    fprintf(report_file, "--- Errores encontrados ---\n");
    for (int i = 0; i < error_count; i++) {
        fprintf(report_file, "Línea %d: %s\n", errors[i].line_number, errors[i].description);
    }

    fclose(report_file);
    printf("Reporte de carga masiva generado: %s\n", filename);
}
void* thread_function(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    struct json_object *json_accounts = args->json_accounts;

    for (int i = args->start_idx; i < args->end_idx; i++) {
        struct json_object *json_account = json_object_array_get_idx(json_accounts, i);
        process_account(json_account, i + 1, args->thread_id); // i + 1 to get line number
    }

    free(arg);
    return NULL;
}

void load_accounts_from_json(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Cannot open file");
        return;
    }

    struct json_object *parsed_json;
    char *buffer;
    long file_size;

    // Read file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the buffer
    buffer = malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return;
    }

    // Read the entire file into the buffer
    fread(buffer, file_size, 1, file);
    buffer[file_size] = '\0';
    fclose(file);

    // Parse the JSON
    parsed_json = json_tokener_parse(buffer);
    free(buffer);

    if (json_object_get_type(parsed_json) != json_type_array) {
        fprintf(stderr, "Error: JSON is not an array.\n");
        return;
    }

    sem_init(&account_sem, 0, NUM_THREADS);

    pthread_t threads[NUM_THREADS];
    size_t n_accounts = json_object_array_length(parsed_json);
    size_t chunk_size = (n_accounts + NUM_THREADS - 1) / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->json_accounts = parsed_json;
        args->thread_id = i;
        args->start_idx = i * chunk_size;
        args->end_idx = (i + 1) * chunk_size < n_accounts ? (i + 1) * chunk_size : n_accounts;
        pthread_create(&threads[i], NULL, thread_function, args);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    generate_report();
}

bool is_duplicate_account(int account_number) {
    for (int i = 0; i < account_count; i++) {
        if (accounts[i].account_number == account_number) {
            return true;
        }
    }
    return false;
}

void record_error(int line_number, const char *description) {
    if (error_count < MAX_ERRORS) {
        errors[error_count].line_number = line_number;
        strncpy(errors[error_count].description, description, sizeof(errors[error_count].description));
        error_count++;
    }
}

void process_account(struct json_object *json_account, int line_number, int thread_id) {
    struct json_object *json_account_number, *json_name, *json_balance;
    json_object_object_get_ex(json_account, "no_cuenta", &json_account_number);
    json_object_object_get_ex(json_account, "nombre", &json_name);
    json_object_object_get_ex(json_account, "saldo", &json_balance);

    int account_number = json_object_get_int(json_account_number);
    const char *name = json_object_get_string(json_name);
    double balance = json_object_get_double(json_balance);

    pthread_mutex_lock(&account_mutex);

    if (account_number <= 0) {
        record_error(line_number, "Número de cuenta no válido (debe ser un entero positivo)");
    } else if (is_duplicate_account(account_number)) {
        record_error(line_number, "Número de cuenta duplicado");
    } else if (balance < 0) {
        record_error(line_number, "Saldo negativo");
    } else if (account_count < MAX_ACCOUNTS) {
        // Valid account
        accounts[account_count].account_number = account_number;
        strcpy(accounts[account_count].name, name);
        accounts[account_count].balance = balance;
        account_count++;
        thread_usage[thread_id]++;
    } else {
        record_error(line_number, "Maximum account limit reached");
    }

    pthread_mutex_unlock(&account_mutex);
    sem_post(&account_sem);
}

void report_thread_usage() {
    int total_processed = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Hilo %d procesó %d cuentas\n", i, thread_usage[i]);
        total_processed += thread_usage[i];
    }
    printf("Total de cuentas procesadas por todos los hilos: %d\n", total_processed);
}

void report_invalid_accounts() {
    printf("Cuentas inválidas:\n");
    for (int i = 0; i < invalid_account_count; i++) {
        printf("Número de cuenta: %d, Nombre: %s, Saldo: %.2f\n",
            invalid_accounts[i].account_number,
            invalid_accounts[i].name,
            invalid_accounts[i].balance);
    }
}

void report_errors() {
    printf("Errores encontrados:\n");
    for (int i = 0; i < error_count; i++) {
        printf("Línea %d: %s\n", errors[i].line_number, errors[i].description);
    }
}


void generate_user_report() {
    struct json_object *json_report = json_object_new_object();
    struct json_object *json_accounts = json_object_new_array();

    for (int i = 0; i < account_count; i++) {
        struct json_object *json_account = json_object_new_object();
        json_object_object_add(json_account, "account_number", json_object_new_int(accounts[i].account_number));
        json_object_object_add(json_account, "name", json_object_new_string(accounts[i].name));
        json_object_object_add(json_account, "balance", json_object_new_double(accounts[i].balance));
        json_object_array_add(json_accounts, json_account);
    }

    json_object_object_add(json_report, "accounts", json_accounts);

    const char *report_str = json_object_to_json_string_ext(json_report, JSON_C_TO_STRING_PRETTY);

    FILE *report_file = fopen("user_report.json", "w");
    if (!report_file) {
        perror("Cannot open report file");
        json_object_put(json_report);
        return;
    }

    fprintf(report_file, "%s", report_str);
    fclose(report_file);

    json_object_put(json_report);
}