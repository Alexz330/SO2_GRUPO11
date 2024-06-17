# SISTEMAS OPERATIVOS 2 - PRACTICA 2

## Integrantes
|NOMBRE|CARNET|
|:----------|:----------:|
|Alexis Marco Tulio López Cacoj|	201908359|
|Juan Sebastian Julajuj Zelada|	201905711|

## Objetivos
* Comprender como funcionan los hilos en Linux.
* Comprender los conceptos de concurrencia y paralelismo.
* Aplicar conceptos de sincronización de procesos.

## Descripción
Las corporaciones utilizan computadoras de alto rendimiento llamados mainframes para aplicaciones que dependen de la escalabilidad y la confiabilidad. Por ejemplo, una institución bancaria podría utilizar un mainframe para albergar la base de datos de las cuentas de sus clientes, para las cuales se pueden enviar transacciones desde cualquiera de los miles de cajeros automáticos en todo el mundo. La práctica consiste en realizar una aplicación en consola en C que permita almacenar los datos de usuario de un banco, así como poder realizar operaciones monetarias como depósitos, retiros y transacciones.

## Procesos

### Operaciones

Este código en C realiza operaciones bancarias masivas utilizando múltiples hilos para procesar transacciones desde un archivo JSON.


#### Flujo de las operaciones
* Carga de Operaciones: load_operations_from_json carga y distribuye operaciones entre hilos.
* Procesamiento de Operaciones: Cada hilo ejecuta thread_function_operation para procesar su porción de operaciones.
* Registro de Errores: Operaciones inválidas se registran mediante log_invalid_operation.
* Generación de Reporte: Al finalizar, se genera un reporte detallado con generate_operation_report.

#### Definiciones de Constantes y Variables Globales

    #define NUM_THREADS 4
    #define MAX_ACCOUNTS 1000
    
    int thread_usage[NUM_THREADS] = {0};
    OPERATION invalid_operations[MAX_ACCOUNTS];
    int invalid_operation_count = 0;
    
    pthread_mutex_t operation_mutex = PTHREAD_MUTEX_INITIALIZER;
    sem_t operation_sem;

* NUM_THREADS: Número de hilos a utilizar.
* MAX_ACCOUNTS: Máximo número de cuentas.
* thread_usage: Arreglo para rastrear el número de operaciones procesadas por cada hilo.
* invalid_operations: Arreglo para almacenar operaciones inválidas.
* invalid_operation_count: Contador de operaciones inválidas.
* operation_mutex: Mutex para sincronizar el acceso a operaciones.
* operation_sem: Semáforo para controlar la ejecución de los hilos.

#### Definición de la Estructura ThreadArgs

    typedef struct {
        struct json_object *json_operations;
        int thread_id;
        int start_idx;
        int end_idx;
    } ThreadArgs;
    
Esta estructura se utiliza para pasar argumentos a los hilos.

#### Función thread_function_operation

    void* thread_function_operation(void *arg) {
        // ...
    }
    
Función que cada hilo ejecuta, procesando una porción de las operaciones.


#### Función generate_operation_report

    void generate_operation_report() {
        // ...
    }
Genera un reporte de las operaciones procesadas, incluyendo operaciones válidas e inválidas.

#### Función load_operations_from_json

    void load_operations_from_json(const char *file_path) {
        // ...
    }
Carga las operaciones desde un archivo JSON, distribuye el trabajo entre hilos y genera un reporte al finalizar.


#### Función log_invalid_operation

    void log_invalid_operation(int operacion, int cuenta1, int cuenta2, double monto, const char *error_msg, int line_number) {
        // ...
    }
Registra una operación inválida con detalles específicos.

#### Función process_operation

    void process_operation(struct json_object *json_operation, int line_number, int thread_id) {
        // ...
    }

Procesa una operación individual, incluyendo depósitos, retiros y transferencias. Valida cuentas y registra operaciones inválidas cuando es necesario.

#### Funciones para Procesar Operaciones

    void process_deposit(int cuenta_idx, double monto, int thread_id) {
        // ...
    }
    
    void process_withdrawal(int cuenta_idx, double monto, int thread_id, int line_number) {
        // ...
    }
    
    void process_transfer(int cuenta1_idx, int cuenta2_idx, double monto, int thread_id, int line_number) {
        // ...
    }

Cada función procesa un tipo de operación específico (depósito, retiro, transferencia), actualizando el saldo de las cuentas y registrando las operaciones procesadas.

#### Función find_account_index

    int find_account_index(int account_number) {
        // ...
    }
Encuentra el índice de una cuenta dado su número.

#### Función validate_account_exists

    int validate_account_exists(int idx, int account_number, const char *error_msg, int line_number) {
        // ...
    }
    
Valida si una cuenta existe, registrando un error si no es así.


#### Función cleanup_mutex_sem

    void cleanup_mutex_sem() {
        pthread_mutex_unlock(&operation_mutex);
        sem_post(&operation_sem);
    }
    
Limpia y libera los recursos del mutex y semáforo después de procesar una operación.

### Usuarios

Este código en C presenta un menú de operaciones bancarias para el usuario. Permite realizar varias acciones como cargar usuarios, cargar transacciones, realizar depósitos, retiros, transferencias, consultar cuentas, y generar reportes.

#### Función main

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
    
El main es el punto de entrada del programa. En un bucle infinito, muestra el menú, solicita una opción del usuario y ejecuta la función correspondiente según la opción seleccionada. Si la entrada no es válida, se limpia el buffer y se vuelve a solicitar la opción.


#### Función display_menu

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
Esta función muestra el menú de opciones al usuario.

#### Función handle_massive_user_load

    void handle_massive_user_load() {
        char file_path[100];
        printf("Ingrese la ruta del archivo JSON de usuarios: ");
        scanf("%s", file_path);
        load_accounts_from_json(file_path);
        printf("Carga masiva de usuarios completada.\n");
    }
Solicita al usuario la ruta de un archivo JSON que contiene los datos de los usuarios y llama a load_accounts_from_json para cargar los usuarios en el sistema.

#### Función handle_massive_transaction_load

    void handle_massive_transaction_load() {
        char file_path[100];
        printf("Ingrese la ruta del archivo JSON de transacciones: ");
        scanf("%s", file_path);
        load_operations_from_json(file_path);
        printf("Carga masiva de transacciones completada.\n");
    }
Solicita al usuario la ruta de un archivo JSON que contiene transacciones y llama a load_operations_from_json para cargarlas en el sistema.

#### Función handle_deposit

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
Solicita al usuario el número de cuenta y el monto a depositar, valida la información y procesa el depósito usando process_deposit_individual.

#### Función handle_withdrawal
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
Solicita al usuario el número de cuenta y el monto a retirar, valida la información y procesa el retiro usando process_withdrawal_individual.


#### Función handle_transfer

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
Solicita al usuario el número de cuenta de origen y destino, y el monto a transferir, valida la información y procesa la transferencia usando process_transfer_individual.


#### Función handle_query

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
Solicita al usuario el número de cuenta, valida la información y muestra los detalles de la cuenta si existe.

#### Función handle_account_report

    void handle_account_report() {
        generate_user_report();
    }
Genera un reporte en JSON de las cuentas utilizando generate_user_report.

#### Función handle_user_errors_report

    void handle_user_errors_report() {
        printf("\n--- Reporte de Errores de Usuarios ---\n");
        report_invalid_accounts();
    }

Genera y muestra un reporte de errores de usuarios utilizando report_invalid_accounts.


