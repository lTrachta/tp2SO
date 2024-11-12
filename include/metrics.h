/**
 * @file metrics.h
 * @brief Funciones para obtener estadísticas del sistema desde el sistema de archivos /proc.
 *
 * Este archivo contiene las declaraciones de funciones que permiten
 * recolectar estadísticas del sistema a partir de información disponible en
 * el sistema de archivos virtual /proc, que proporciona datos sobre el uso
 * de memoria, CPU, I/O de disco, y más.
 */

#include <stdio.h>  // Para funciones de entrada/salida estándar
#include <stdlib.h> // Para funciones de utilidad general
#include <string.h> // Para funciones de manipulación de cadenas
#include <unistd.h> // Para funciones POSIX, como read y write

#define BUFFER_SIZE 256 ///< Tamaño del buffer utilizado para lecturas
#define BUFFER_S32 32   ///< Tamaño del buffer utilizado para lecturas

/**
 * @brief Obtiene el porcentaje de uso de memoria desde /proc/meminfo.
 *
 * Esta función lee los valores de memoria total y disponible desde
 * /proc/meminfo y calcula el porcentaje de uso de memoria.
 *
 * @return Uso de memoria como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_memory_usage(void);

/**
 * @brief Obtiene el porcentaje de uso de CPU desde /proc/stat.
 *
 * Esta función lee los tiempos de CPU desde /proc/stat y calcula
 * el porcentaje de uso de CPU en un intervalo de tiempo.
 *
 * @return Uso de CPU como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_cpu_usage(void);

/**
 * @brief Obtiene el número de bytes leídos desde disco desde /proc/diskstats.
 *
 * Esta función lee el número de bytes leídos desde el dispositivo de
 * almacenamiento y devuelve el valor correspondiente.
 *
 * @return Número de bytes leídos, o -1 en caso de error.
 */
double get_disk_io_read(void);

/**
 * @brief Obtiene el número de bytes escritos en disco desde /proc/diskstats.
 *
 * Esta función lee el número de bytes escritos en el dispositivo de
 * almacenamiento y devuelve el valor correspondiente.
 *
 * @return Número de bytes escritos, o -1 en caso de error.
 */
double get_disk_io_write(void);

/**
 * @brief Obtiene las estadísticas de red desde /proc/net/dev.
 *
 * Esta función lee las estadísticas de red y almacena los bytes
 * recibidos y transmitidos en las direcciones proporcionadas.
 *
 * @param rx_bytes Puntero para almacenar los bytes recibidos.
 * @param tx_bytes Puntero para almacenar los bytes transmitidos.
 * @return 0 en caso de éxito, -1 en caso de error.
 */
int get_network_stats(unsigned long long* rx_bytes, unsigned long long* tx_bytes);

/**
 * @brief Obtiene el número de procesos en ejecución desde /proc/stat.
 *
 * Esta función cuenta el número actual de procesos en ejecución
 * en el sistema y lo devuelve.
 *
 * @return Número de procesos en ejecución, o -1 en caso de error.
 */
double get_process_count(void);

/**
 * @brief Obtiene el número de cambios de contexto desde /proc/stat.
 *
 * Esta función lee el número de cambios de contexto registrados
 * en el sistema y devuelve el valor correspondiente.
 *
 * @return Número de cambios de contexto, o -1 en caso de error.
 */
double get_context_switches(void);
