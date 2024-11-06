/**
 * @file expose_metrics.h
 * @brief Programa para leer el uso de CPU, memoria y otros recursos del sistema y exponerlos como métricas de
 * Prometheus.
 *
 * Este archivo contiene las declaraciones necesarias para la recolección y
 * exposición de métricas del sistema a través de un servidor HTTP.
 */

#include "metrics.h"  // Incluye las definiciones de las métricas
#include <errno.h>    // Para manejar errores
#include <prom.h>     // Para la biblioteca de Prometheus
#include <promhttp.h> // Para la interfaz HTTP de Prometheus
#include <pthread.h>  // Para manejar hilos
#include <stdio.h>    // Para funciones de entrada/salida
#include <stdlib.h>   // Para funciones de utilidad general
#include <string.h>   // Para funciones de manipulación de cadenas
#include <unistd.h>   // Para la función sleep

#define BUFFER_SIZE 256 ///< Tamaño del buffer utilizado para lecturas

// Declaración de las métricas como variables externas
extern prom_gauge_t* disk_io_read_metric;   ///< Métrica para las lecturas de I/O de disco
extern prom_gauge_t* disk_io_write_metric;  ///< Métrica para las escrituras de I/O de disco
extern prom_gauge_t* net_rx_metric;         ///< Métrica para la recepción de datos de red
extern prom_gauge_t* net_tx_metric;         ///< Métrica para la transmisión de datos de red
extern prom_gauge_t* process_count_metric;  ///< Métrica para el conteo de procesos en ejecución
extern prom_gauge_t* context_switch_metric; ///< Métrica para el conteo de cambios de contexto

/**
 * @brief Obtiene el tráfico de red recibido.
 * @return El total de bytes recibidos.
 */
double get_network_rx();

/**
 * @brief Obtiene el tráfico de red transmitido.
 * @return El total de bytes transmitidos.
 */
double get_network_tx();

/**
 * @brief Actualiza la métrica de uso de CPU.
 *
 * Esta función lee el uso actual de la CPU y actualiza la métrica correspondiente.
 */
void update_cpu_gauge();

/**
 * @brief Actualiza la métrica de uso de memoria.
 *
 * Esta función lee el uso actual de la memoria y actualiza la métrica correspondiente.
 */
void update_memory_gauge();

/**
 * @brief Actualiza las métricas de I/O de disco.
 *
 * Esta función obtiene el estado actual de las operaciones de I/O de disco
 * y actualiza las métricas correspondientes.
 */
void update_disk_io_gauge();

/**
 * @brief Actualiza las métricas de red.
 *
 * Esta función obtiene el estado actual de las estadísticas de red
 * y actualiza las métricas correspondientes.
 */
void update_network_stats_gauge();

/**
 * @brief Actualiza la métrica del conteo de procesos.
 *
 * Esta función obtiene el conteo actual de procesos en ejecución
 * y actualiza la métrica correspondiente.
 */
void update_process_count_gauge();

/**
 * @brief Actualiza la métrica de cambios de contexto.
 *
 * Esta función obtiene el conteo actual de cambios de contexto
 * y actualiza la métrica correspondiente.
 */
void update_context_switches_gauge();

/**
 * @brief Función del hilo para exponer las métricas vía HTTP en el puerto 8000.
 *
 * Esta función ejecuta un servidor HTTP que expone las métricas recolectadas.
 * @return NULL
 */
void* expose_metrics(void* arg);

/**
 * @brief Inicializa el mutex y las métricas.
 *
 * Esta función se encarga de inicializar los mutex necesarios para
 * el manejo seguro de las métricas en un entorno multihilo.
 */
void init_metrics();

/**
 * @brief Destructor del mutex.
 *
 * Esta función destruye los mutex inicializados y libera los recursos.
 */
void destroy_mutex();
