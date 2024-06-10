# SISTEMAS OPERATIVOS 2 - PRACTICA 1

## Integrantes
|NOMBRE|CARNET|
|:----------|:----------:|
|Alexis Marco Tulio López Cacoj|	201908359|
|Juan Sebastian Julajuj Zelada|	201905711|

## Objetivos

* Comprender cómo funcionan las llamadas para la gestión de procesos en Linux.
* Comprender cómo funcionan las llamadas para la gestión de archivos en Linux.
* Aprender a interceptar las llamadas al sistema realizadas por un proceso.


## Descripción General
El objetivo de la práctica es escribir un programa en C que monitoree y registre en un log todas las llamadas al sistema realizadas por los procesos hijos creados por un proceso padre. Este monitoreo se realiza utilizando SystemTap para interceptar dichas llamadas.

## Procesos

#### Proceso Padre (parent.c)

El proceso padre es el encargado de:

* Crear los procesos hijos utilizando la llamada de sistema fork().
* Monitorear las llamadas al sistema de los procesos hijos y escribirlas en un archivo de log syscalls.log.
* Capturar la señal SIGINT (Ctrl + C) y, antes de finalizar, imprimir el número total de llamadas al sistema realizadas por los procesos hijos y el número de llamadas por tipo.

Descripcion de las instrucciones utilizadas en el proceso padre:

* Manejo de señal SIGINT: Define un manejador de señales para capturar SIGINT.
* Creación de archivos: Crea y vacía syscalls.log y practica1.txt.
* Creación de procesos hijo: Utiliza fork() para crear dos procesos hijo y execv() para ejecutar el código del proceso hijo.
* Creación del proceso monitor: Crea un tercer proceso para monitorizar las llamadas al sistema de los procesos hijo usando SystemTap.
* Esperar finalización: Usa waitpid() para esperar la finalización de los procesos hijo y el monitor.
* Imprimir estadísticas: Llama a una función stat() para imprimir estadísticas de las llamadas al sistema realizadas por los procesos hijo.


#### Procesos Hijos (child.c)

Cada uno de los procesos hijos realiza operaciones de manejo de archivos en un archivo llamado practica1.txt. Estas operaciones son seleccionadas de manera aleatoria y pueden ser:
* Open: Abrir el archivo.
* Write: Escribir una línea de texto con 8 caracteres alfanuméricos aleatorios.
* Read: Leer 8 caracteres.

Las operaciones son realizadas con un intervalo aleatorio de tiempo entre 1 y 3 segundos. Los procesos hijos también deben manejar la señal SIGINT para finalizar correctamente.

Descripcion de las instrucciones utilizadas en el proceso hijo:

* Inicialización: Establece una semilla para la generación de números aleatorios basada en el tiempo y el PID del proceso.
* Apertura de archivo: Abre practica1.txt en modo lectura y escritura.
* Manejo de señal SIGINT: Define un manejador de señales para cerrar el archivo y finalizar el proceso al recibir SIGINT.
* Bucle principal: Realiza operaciones aleatorias de lectura, escritura y desplazamiento sobre el archivo en intervalos de 1 a 3 segundos, hasta que se reciba SIGINT.



## Monitor de Syscalls (monitor.c)

El proceso monitor utiliza SystemTap para interceptar y registrar las llamadas al sistema realizadas por los procesos hijos. Esto se logra mediante un script de SystemTap que se ejecuta desde el proceso padre.

El script intercepta todas las llamadas al sistema realizadas por los procesos hijos y las registra en el log syscalls.log.


## Manejo de Señales (signal_handler.c)

El manejo de señales asegura que el programa puede responder a la señal SIGINT (Ctrl + C) para realizar una limpieza adecuada y recopilar estadísticas antes de finalizar.

## Estadísticas (stat.c)

La función stat calcula y muestra el número total de llamadas al sistema realizadas por los procesos hijos y el número de llamadas por tipo.

