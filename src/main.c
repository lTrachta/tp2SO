#include "../include/expose_metrics.h"
#include "../include/getinfojson.h"
#include "../include/metrics.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int sampling_interval = 1;    // Intervalo de muestreo predeterminado
    char* metrics_to_collect[10]; // Puedes ajustar el tamaño según sea necesario
    int num_metrics = 0;
    double disk_read;
    double disk_write;
    double net_rx;
    double net_tx;
    double process_count;
    double context_switches;

    // Cargar la configuración desde config.json
    load_config(&sampling_interval, metrics_to_collect, &num_metrics);

    // Inicializar las métricas y el mutex
    init_metrics();

    // Crear un hilo para exponer las métricas
    pthread_t expose_thread;
    if (pthread_create(&expose_thread, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo para exponer métricas\n");
        exit(EXIT_FAILURE);
    }

    // Bucle para actualizar las métricas de acuerdo al intervalo de muestreo
    while (1)
    {
        // Actualizamos las métricas según las seleccionadas en el config.json
        for (int i = 0; i < num_metrics; i++)
        {
            if (strcmp(metrics_to_collect[i], "cpu_usage") == 0)
            {
                update_cpu_gauge();
            }
            else if (strcmp(metrics_to_collect[i], "memory_usage") == 0)
            {
                update_memory_gauge();
            }
            else if (strcmp(metrics_to_collect[i], "diskstats_usage") == 0)
            {
                disk_read = get_disk_io_read();
                prom_gauge_set(disk_io_read_metric, disk_read, NULL);
                disk_write = get_disk_io_write();
                prom_gauge_set(disk_io_write_metric, disk_write, NULL);
            }
            else if (strcmp(metrics_to_collect[i], "net_usage") == 0)
            {
                net_rx = get_network_rx();
                prom_gauge_set(net_rx_metric, net_rx, NULL);
                net_tx = get_network_tx();
                prom_gauge_set(net_tx_metric, net_tx, NULL);
            }
            else if (strcmp(metrics_to_collect[i], "process_usage") == 0)
            {
                process_count = get_process_count();
                prom_gauge_set(process_count_metric, process_count, NULL);
            }
            else if (strcmp(metrics_to_collect[i], "context_switches_usage") == 0)
            {
                context_switches = get_context_switches();
                prom_gauge_set(context_switch_metric, context_switches, NULL);
            }
        }

        // Esperar el intervalo de muestreo
        sleep(sampling_interval);
    }

    // Esperar a que termine el hilo de exposición de métricas (aunque no debería terminar nunca)
    pthread_join(expose_thread, NULL);

    // Liberar recursos y destruir el mutex
    destroy_mutex();

    return 0;
}
