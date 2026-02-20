#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // для int64_t
#include <inttypes.h> // для правильного вывода int64_t в printf

#ifdef linux
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#pragma once
int64_t* getFilesSize(int64_t* sum_files_size, int files_count, char** files_names);

