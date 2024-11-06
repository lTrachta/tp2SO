#include "expose_metrics.h"
#include <pthread.h> // Para hilos
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // Para EXIT_SUCCESS y EXIT_FAILURE
#include <unistd.h> // Para sleep

#define SLEEP_TIME 1 ///< Tiempo de espera entre actualizaciones de métricas en segundos

/**
 * @brief Función principal que inicializa las métricas, lanza el servidor HTTP y
 * actualiza las métricas periódicamente.
 *
 * Esta función se encarga de inicializar las métricas y el mutex necesario para la
 * sincronización en entornos de múltiples hilos. Luego, crea un hilo que expone las
 * métricas a través de un servidor HTTP y entra en un bucle infinito donde actualiza
 * periódicamente las métricas de uso de CPU, memoria, I/O de disco, estadísticas de
 * red, conteo de procesos y cambios de contexto.
 *
 * @return int EXIT_SUCCESS si todo fue correcto, EXIT_FAILURE en caso de error
 */
int main(void)
{
    // Inicializamos las métricas y el mutex
    init_metrics();

    // Creamos el hilo para exponer las métricas vía HTTP
    pthread_t tid;
    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo del servidor HTTP\n");
        return EXIT_FAILURE;
    }

    while (true)
    {
        // Actualizamos la métrica de uso de CPU
        update_cpu_gauge();

        // Actualizamos la métrica de uso de memoria
        update_memory_gauge();

        /**
         * @brief Actualizamos las métricas de I/O de disco
         *
         * Llamamos a las funciones para obtener las lecturas y escrituras de disco
         * y actualizamos las métricas correspondientes en Prometheus.
         */
        double disk_read = get_disk_io_read();
        prom_gauge_set(disk_io_read_metric, disk_read, NULL);

        double disk_write = get_disk_io_write();
        prom_gauge_set(disk_io_write_metric, disk_write, NULL);

        /**
         * @brief Actualizamos las métricas de red
         *
         * Llamamos a las funciones para obtener los bytes recibidos y transmitidos
         * en la red y actualizamos las métricas correspondientes en Prometheus.
         */
        double net_rx = get_network_rx();
        prom_gauge_set(net_rx_metric, net_rx, NULL);

        double net_tx = get_network_tx();
        prom_gauge_set(net_tx_metric, net_tx, NULL);

        /**
         * @brief Actualizamos la métrica de conteo de procesos
         *
         * Llamamos a la función que obtiene la cantidad de procesos en ejecución
         * y actualizamos la métrica en Prometheus.
         */
        double process_count = get_process_count();
        prom_gauge_set(process_count_metric, process_count, NULL);

        /**
         * @brief Actualizamos la métrica de cambios de contexto
         *
         * Llamamos a la función que obtiene el número de cambios de contexto
         * y actualizamos la métrica en Prometheus.
         */
        double context_switches = get_context_switches();
        prom_gauge_set(context_switch_metric, context_switches, NULL);

        // Esperamos el tiempo definido antes de la siguiente actualización
        sleep(SLEEP_TIME);
    }

    return EXIT_SUCCESS; // Nunca debería llegar aquí
}
