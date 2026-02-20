//get_directory_files_name.h
#pragma once

#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <limits.h>
#endif

//#define PRINTS_LOGS

#ifdef PRINTS_LOGS
#define LOG(...) printf(__VA_ARGS__);
#endif
#ifndef PRINTS_LOGS
#define LOG(...) ((void)0);
#endif


#ifdef _WIN32
#define get_full_path(rel_path, abs_path) _fullpath(abs_path, rel_path, MAX_PATH_LENGHT)
#define MAX_PATH_LENGHT _MAX_PATH
#else
#define get_full_path(rel_path, abs_path) realpath(rel_path, abs_path)
#define MAX_PATH_LENGHT PATH_MAX
#endif
size_t get_names(char*** mas, char* name);
