# Specifica la versione minima di CMake
cmake_minimum_required(VERSION 3.10)

# Nome del progetto
project(rabbitmq_project)

# Libreria rabbitmq-c
find_library(RABBITMQ_LIB rabbitmq REQUIRED)

# File sorgente
add_executable(publisher src/publisher.cpp)
add_executable(subscriber src/subscriber.cpp)

# Linko la libreria rabbitmq-c ai due eseguibili
target_link_libraries(publisher ${RABBITMQ_LIB})
target_link_libraries(subscriber ${RABBITMQ_LIB})

