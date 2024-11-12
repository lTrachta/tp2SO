#include "../include/metrics.h"
#include <stdio.h>
#include <stdlib.h> // Para perror
#include <string.h> // Para strcmp

/**
 * @brief Obtiene el porcentaje de uso de memoria del sistema.
 *
 * Esta función lee el archivo /proc/meminfo para obtener los valores de memoria
 * total y disponible, y calcula el porcentaje de memoria utilizada.
 *
 * @return double Porcentaje de uso de memoria. Retorna -1.0 en caso de error.
 */
double get_memory_usage()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long total_mem = 0, free_mem = 0;

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        return -1.0;
    }

    // Leer los valores de memoria total y disponible
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %llu kB", &total_mem) == 1)
        {
            continue; // MemTotal encontrado
        }
        if (sscanf(buffer, "MemAvailable: %llu kB", &free_mem) == 1)
        {
            break; // MemAvailable encontrado, podemos dejar de leer
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (total_mem == 0 || free_mem == 0)
    {
        fprintf(stderr, "Error al leer la información de memoria desde /proc/meminfo\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de memoria
    double used_mem = total_mem - free_mem;
    double mem_usage_percent = (used_mem / total_mem) * 100.0;

    return mem_usage_percent;
}

/**
 * @brief Obtiene el porcentaje de uso de CPU del sistema.
 *
 * Esta función lee el archivo /proc/stat para obtener los valores de tiempo de CPU
 * y calcula el porcentaje de uso de CPU.
 *
 * @return double Porcentaje de uso de CPU. Retorna -1.0 en caso de error.
 */
double get_cpu_usage()
{
    static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0,
                              prev_irq = 0, prev_softirq = 0, prev_steal = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long totald, idled;
    double cpu_usage_percent;

    // Abrir el archivo /proc/stat
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE * 4];
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error al leer /proc/stat");
        fclose(fp);
        return -1.0;
    }
    fclose(fp);

    // Analizar los valores de tiempo de CPU
    int ret = sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait,
                     &irq, &softirq, &steal);
    if (ret < 8)
    {
        fprintf(stderr, "Error al parsear /proc/stat\n");
        return -1.0;
    }

    // Calcular las diferencias entre las lecturas actuales y anteriores
    unsigned long long prev_idle_total = prev_idle + prev_iowait;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    totald = total - prev_total;
    idled = idle_total - prev_idle_total;

    if (totald == 0)
    {
        fprintf(stderr, "Totald es cero, no se puede calcular el uso de CPU!\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de CPU
    cpu_usage_percent = ((double)(totald - idled) / totald) * 100.0;

    // Actualizar los valores anteriores para la siguiente lectura
    prev_user = user;
    prev_nice = nice;
    prev_system = system;
    prev_idle = idle;
    prev_iowait = iowait;
    prev_irq = irq;
    prev_softirq = softirq;
    prev_steal = steal;

    return cpu_usage_percent;
}

/**
 * @brief Obtiene el número de sectores leídos desde el disco.
 *
 * Esta función lee el archivo /proc/diskstats para obtener el número de
 * sectores leídos del dispositivo de disco especificado (sda).
 *
 * @return double Número de sectores leídos. Retorna -1.0 en caso de error.
 */
double get_disk_io_read()
{
    FILE* fp = fopen("/proc/diskstats", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/diskstats");
        return -1.0;
    }

    unsigned long long read_sectors = 0;
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, sizeof(buffer), fp))
    {
        unsigned int major, minor;
        char device[BUFFER_S32];
        unsigned long long reads;
        sscanf(buffer, "%u %u %s %llu", &major, &minor, device, &reads);
        if (strcmp(device, "sda") == 0)
        {
            read_sectors = reads;
            break;
        }
    }

    fclose(fp);
    return (double)read_sectors;
}

/**
 * @brief Obtiene el número de sectores escritos en el disco.
 *
 * Esta función lee el archivo /proc/diskstats para obtener el número de
 * sectores escritos en el dispositivo de disco.
 *
 * @return double Número de sectores escritos. Retorna -1.0 en caso de error.
 */
double get_disk_io_write()
{
    FILE* file = fopen("/proc/diskstats", "r");
    char buffer[BUFFER_SIZE];
    unsigned int major, minor;
    unsigned long long writes = 0;

    if (file)
    {
        while (fgets(buffer, sizeof(buffer), file))
        {
            sscanf(buffer, "%u %u %llu", &major, &minor, &writes);
        }
        fclose(file);
    }
    return (double)writes;
}

/**
 * @brief Obtiene el número de bytes recibidos en la interfaz de red.
 *
 * Esta función lee el archivo /proc/net/dev para obtener el número de
 * bytes recibidos por la interfaz de red (eth0).
 *
 * @return double Número de bytes recibidos. Retorna -1.0 en caso de error.
 */
double get_network_rx()
{
    FILE* fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        return -1.0;
    }

    char buffer[BUFFER_SIZE];
    unsigned long long rx_bytes = 0;

    while (fgets(buffer, sizeof(buffer), fp))
    {
        if (strstr(buffer, "eth0:") != NULL)
        {
            sscanf(buffer, "%*s %llu", &rx_bytes);
            break;
        }
    }

    fclose(fp);
    return (double)rx_bytes;
}

/**
 * @brief Obtiene el número de bytes transmitidos en la interfaz de red.
 *
 * Esta función lee el archivo /proc/net/dev para obtener el número de
 * bytes transmitidos por la interfaz de red.
 *
 * @return double Número de bytes transmitidos. Retorna -1.0 en caso de error.
 */
double get_network_tx()
{
    FILE* file = fopen("/proc/net/dev", "r");
    char buffer[BUFFER_SIZE];
    unsigned long long tx_bytes = 0;

    if (file)
    {
        while (fgets(buffer, sizeof(buffer), file))
        {
            if (sscanf(buffer, "%*s %llu", &tx_bytes) == 1)
            {
                break;
            }
        }
        fclose(file);
    }
    return (double)tx_bytes;
}

/**
 * @brief Obtiene el número de procesos en ejecución.
 *
 * Esta función lee el archivo /proc/stat para obtener el número de
 * procesos en ejecución en el sistema.
 *
 * @return double Número de procesos en ejecución. Retorna -1.0 en caso de error.
 */
double get_process_count()
{
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE];
    unsigned long long procs_running = 0;

    while (fgets(buffer, sizeof(buffer), fp))
    {
        if (sscanf(buffer, "procs_running %llu", &procs_running) == 1)
        {
            break;
        }
    }

    fclose(fp);
    return (double)procs_running;
}

/**
 * @brief Obtiene el número total de cambios de contexto en el sistema.
 *
 * Esta función lee el archivo /proc/stat para obtener el número total de
 * cambios de contexto realizados por el sistema.
 *
 * @return double Número total de cambios de contexto. Retorna -1.0 en caso de error.
 */
double get_context_switches()
{
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE];
    unsigned long long context_switches = 0;

    while (fgets(buffer, sizeof(buffer), fp))
    {
        if (sscanf(buffer, "ctxt %llu", &context_switches) == 1)
        {
            break;
        }
    }

    fclose(fp);
    return (double)context_switches;
}
