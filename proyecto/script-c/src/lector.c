#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>

#define BUFFER_SIZE 2048

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

unsigned long get_total_memory()
{
    FILE *fp;
    char buffer[BUFFER_SIZE];
    unsigned long total_memory = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        exit(1);
    }

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %lu kB", &total_memory) == 1)
        {
            total_memory *= 1024; // Convertir a bytes
            break;
        }
    }

    fclose(fp);
    return total_memory;
}

void format_timestamp(char *input, char *output, size_t output_size)
{
    struct tm tm;
    time_t t;
    char buffer[BUFFER_SIZE];

    // Convertir input a struct tm
    if (strptime(input, "%a %b %d %H:%M:%S %Y", &tm) == NULL)
    {
        fprintf(stderr, "Error al analizar la fecha: %s\n", input);
        exit(1);
    }

    // Convertir struct tm a time_t
    t = mktime(&tm);
    if (t == -1)
    {
        fprintf(stderr, "Error al convertir la fecha a time_t: %s\n", input);
        exit(1);
    }

    // Convertir time_t a la cadena en el formato deseado
    if (strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&t)) == 0)
    {
        fprintf(stderr, "Error al formatear la fecha: %s\n", input);
        exit(1);
    }

    strncpy(output, buffer, output_size);
}

void load_env(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error al abrir el archivo .env");
        exit(1);
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file))
    {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");
        if (key && value)
        {
            setenv(key, value, 1);
        }
    }

    fclose(file);
}

int main()
{
    // Cargar variables de entorno desde el archivo .env
    load_env(".env");

    FILE *fp;
    char buffer[BUFFER_SIZE];
    char query[BUFFER_SIZE];
    char process_name[256]; // Tamaño razonable para el nombre del proceso
    char formatted_time[64]; // Tamaño suficiente para la fecha formateada
    unsigned long total_memory = get_total_memory();

    // Obtener variables de entorno
    const char *db_host = getenv("DB_HOST");
    const char *db_user = getenv("DB_USER");
    const char *db_pass = getenv("DB_PASS");
    const char *db_name = getenv("DB_NAME");

    // Conexión a MySQL
    MYSQL *con = mysql_init(NULL);

    if (con == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }

    if (mysql_real_connect(con, db_host, db_user, db_pass, db_name, 0, NULL, 0) == NULL)
    {
        finish_with_error(con);
    }

    // Ejecutar script de SystemTap y leer su output
    fp = popen("sudo stap memory_monitor.stp", "r");
    if (fp == NULL)
    {
        perror("Error al ejecutar el script de SystemTap");
        exit(1);
    }

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        char call[16], time_str[64];
        int pid, length;

        // Parsear la salida CSV del script de SystemTap
        sscanf(buffer, "%15[^,],%d,%255[^,],%d,%63[^\n]", call, &pid, process_name, &length, time_str);

        double percentage = (double)length / total_memory * 100;

        // Formatear la fecha para la base de datos
        format_timestamp(time_str, formatted_time, sizeof(formatted_time));

        // Imprimir los datos en la consola
        printf("Call: %s\n", call);

        printf("Call: %s\n", call);
        printf("PID: %d\n", pid);
        printf("Process Name: %s\n", process_name);
        printf("Time: %s\n", formatted_time);
        printf("Length: %d\n", length);
        printf("Percentage of Total Memory: %f%%\n\n", percentage);

        snprintf(query, BUFFER_SIZE,
                 "INSERT INTO memory_usage (pid, process_name, my_call, memory_size, timestamp, percentage) VALUES (%d, '%s', '%s', %d, '%s', %f)",
                 pid, process_name, call, length, formatted_time, percentage);

        if (mysql_query(con, query))
        {
            finish_with_error(con);
        }
    }

    // Cerrar archivo y conexión MySQL
    pclose(fp);
    mysql_close(con);

    return 0;
}
