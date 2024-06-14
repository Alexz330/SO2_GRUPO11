#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <json-c/json.h>

#define MAX_ACCOUNTS 1000


typedef struct {
    int line_number;   // Nuevo campo para almacenar el número de línea
    int operation;
    int cuenta1;
    int cuenta2;
    double monto;
    char error_msg[100];  // Ajusta el tamaño según sea necesario
} OPERATION;



extern OPERATION invalid_operations[MAX_ACCOUNTS];
extern int invalid_operation_count;

void process_operation(struct json_object *json_operation, int line_number, int thread_id);
void load_operations_from_json(const char *file_path);
void report_thread_usage_operation();

void log_invalid_operation(int operacion, int cuenta1, int cuenta2, double monto, const char *error_msg, int line_number);

int find_account_index(int account_number);
int validate_account_exists(int idx, int account_number, const char *error_msg,int line_number);
void cleanup_mutex_sem();


void process_deposit(int cuenta_idx, double monto, int thread_id);
void process_withdrawal(int cuenta_idx, double monto, int thread_id,int line_number);
void process_transfer(int cuenta1_idx, int cuenta2_idx, double monto, int thread_id,int line_number);

void process_deposit_individual(int cuenta_idx, double monto);
void process_withdrawal_individual(int cuenta_idx, double monto);
void process_transfer_individual( int cuenta1_idx, int cuenta2_idx, double monto);

#endif // OPERATIONS_H
