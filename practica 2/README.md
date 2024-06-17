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


