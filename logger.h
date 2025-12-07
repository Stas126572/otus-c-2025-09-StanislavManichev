// logger.h должен содержать объявления и макросы
#ifndef LOGGER_HD
#define LOGGER_HD
#include <stdio.h>
#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#define BUF_SIZE 100

// Прототип функции (объявление)
void logger_function(FILE* fp, const char* level_string, const char* message, const char* file_name, int line);

// Макросы
#define LOGGER(file, level, string) logger_function(file, level, string, __FILE__, __LINE__)

#define LOG_DEBUG(file, string) LOGGER(file, "DEBUG", string)
#define LOG_INFO(file, string) LOGGER(file, "INFO", string)
#define LOG_WARNING(file, string) LOGGER(file, "WARNING", string)
#define LOG_ERROR(file, string) LOGGER(file, "ERROR", string)
#endif
