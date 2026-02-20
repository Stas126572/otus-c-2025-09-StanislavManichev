#include <stdint.h> // для int64_t
#include <inttypes.h> // для правильного вывода int64_t в print
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#pragma once

char* files_contains(int64_t files_count, char** files_name, int64_t* files_size, int64_t summar_size);
void read_files_contains(char* buffer, int64_t summar_size);
