cmake_minimum_required(VERSION 3.10)
project(monitoring_project)

# Incluir directorios de encabezados
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/include /usr/include/cjson)

# Añadir el ejecutable
add_executable(monitoring_project
    src/main.c
    src/metrics.c
    src/expose_metrics.c
    src/getinfojson.c
)

# Añadir el directorio donde se instalan las librerías compartidas (instaladas con sudo make install)
link_directories(/usr/local/lib)
link_directories(/usr/local/lib /lib/x86_64-linux-gnu)

# Vincular las librerías libprom y libpromhttp desde /usr/local/lib
target_link_libraries(monitoring_project
    /usr/local/lib/libprom.so
    /usr/local/lib/libpromhttp.so
)
