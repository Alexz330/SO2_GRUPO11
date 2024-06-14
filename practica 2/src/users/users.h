// user.h
#ifndef USERS_H
#define USERS_H

#include <stdbool.h>
#include <json-c/json.h>

#define MAX_ACCOUNTS 1000
#define MAX_ERRORS 100

typedef struct {
    int account_number;
    char name[50];
    double balance;
} USER;

typedef struct {
    int line_number;
    char description[100];
} ERROR;

extern USER accounts[MAX_ACCOUNTS];  // Declaración externa de accounts
extern int account_count;

void load_accounts_from_json(const char *file_path);
void process_account(struct json_object *json_account, int line_number, int thread_id);
void report_thread_usage();
void report_invalid_accounts();
void report_errors();
void generate_user_report();
#endif // USERS_H
