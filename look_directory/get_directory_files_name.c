//get_directory_files_name.c
#define _DEFAULT_SOURCE
#include "get_directory_files_name.h"

size_t get_names(char*** mas, char* dir_name) {
	*mas = NULL;
	DIR* dir;
	struct dirent* entry;
	size_t file_count = 0;

	char abs_path[MAX_PATH_LENGHT];
	char base_path[MAX_PATH_LENGHT];
	LOG("Name of opened directory: %s\n", dir_name);
	dir = opendir(dir_name);
	if (!dir) 	
		return 0; 

	size_t pos = strlen(dir_name);
	strcpy(base_path, dir_name);
	base_path[pos] =  '/';
        base_path[pos + 1] = '\0';
	pos++;
	LOG("Dir path: %s", base_path);
	while ((entry = readdir(dir)) != NULL)
	{
		
                LOG("In the while\n");
		if (entry->d_type == DT_REG) {
			strcpy(base_path + pos, entry->d_name);
			
			


			if (get_full_path(base_path, abs_path)) {
			*mas = (char**)realloc((void*)(*mas), sizeof(char*) * (++file_count));
                        if (!(*mas))
                               return 0;    
			
			LOG("Full path: %s\n", abs_path);
			(*mas)[file_count - 1] = strdup(abs_path);
			}
		}
	}
	LOG("Numbers of files: %ld\n", file_count);
	closedir(dir);
	
	return file_count;     
}	
