#include "../include/getinfojson.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_config(int* sampling_interval, char* metrics_to_collect[], int* num_metrics)
{
    FILE* file = fopen("../config.json", "r");
    if (!file)
    {
        perror("Error al abrir config.json");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* json_data = malloc(file_size + 1);
    fread(json_data, 1, file_size, file);
    fclose(file);

    cJSON* json = cJSON_Parse(json_data);
    if (!json)
    {
        fprintf(stderr, "Error al parsear config.json\n");
        free(json_data);
        exit(EXIT_FAILURE);
    }

    // Leer el intervalo de muestreo
    cJSON* interval = cJSON_GetObjectItem(json, "sampling_interval");
    if (cJSON_IsNumber(interval))
    {
        *sampling_interval = interval->valueint;
    }
    else
    {
        fprintf(stderr, "sampling_interval inválido\n");
    }

    // Leer las métricas a recolectar
    cJSON* metrics_array = cJSON_GetObjectItem(json, "metrics_to_collect");
    if (cJSON_IsArray(metrics_array))
    {
        int index = 0;
        cJSON* metric = NULL;
        cJSON_ArrayForEach(metric, metrics_array)
        {
            if (cJSON_IsString(metric))
            {
                metrics_to_collect[index] = strdup(metric->valuestring);
                index++;
            }
        }
        *num_metrics = index;
    }

    cJSON_Delete(json);
    free(json_data);
}
