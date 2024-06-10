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

## Instrucciones de Ejecución

#### Instalación de SystemTap
SystemTap es una herramienta utilizada para monitorear el funcionamiento del kernel y recoger información sobre el sistema. 
Para instalar SystemTap y las cabeceras del kernel necesarias, ejecuta el siguiente comando en tu terminal:

    apt install systemtap linux-headers-$(uname -r)

* apt install systemtap: Instala el paquete SystemTap.
* linux-headers-$(uname -r): Instala las cabeceras del kernel para la versión del kernel que estás ejecutando actualmente. uname -r retorna la versión del kernel, y el comando en conjunto asegura que se instalan las cabeceras correspondientes a esa versión.

#### Compilación del Código Fuente
Para compilar el código fuente, necesitarás el compilador gcc. Aquí hay dos pasos separados para compilar el código del proceso padre y el del proceso hijo.

#### Compilación del Módulo del Proceso Padre
El proceso padre se compone de múltiples archivos de código fuente: main.c, monitor.c, stat.c y signal_handler.c. Estos archivos se deben compilar juntos para crear un ejecutable llamado main.

Ejecuta el siguiente comando para compilar el proceso padre:

    gcc main.c monitor.c stat.c signal_handler.c -o main -lpthread

* main.c: Archivo principal que contiene la lógica principal del programa.
* monitor.c: Archivo que contiene la lógica para monitorear las llamadas al sistema de los procesos hijo.
* stat.c: Archivo que contiene la lógica para calcular y mostrar las estadísticas de las llamadas al sistema.
* signal_handler.c: Archivo que contiene la lógica para manejar las señales.
* -o main: Especifica el nombre del archivo ejecutable de salida, en este caso main.
* -lpthread: Linkea con la librería de pthread, que es necesaria para el manejo de hilos.

#### Compilación del Módulo del Proceso Hijo
El proceso hijo tiene su propio archivo de código fuente main.c (el nombre del archivo puede ser el mismo, pero está en un contexto diferente). Este archivo se compila para crear un ejecutable llamado main.bin.

Ejecuta el siguiente comando para compilar el proceso hijo:

    gcc main.c -o main.bin

* main.c: Archivo que contiene la lógica del proceso hijo.
* -o main.bin: Especifica el nombre del archivo ejecutable de salida, en este caso main.bin.

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

