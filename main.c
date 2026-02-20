#include "look_directory/get_directory_files_name.h"
#include "get_size_of_file/get_size_of_file.h"
#include "get_files_contains/get_files_contains.h"

typedef struct {
   
} thread_info;

int main(int argc, char** argv)
{
         if (argc < 3) {
                printf("Please get the directory and thread number.\n");
                return 1;

        }
	int thread_num = atoi(argv[2]);
        char* dir_name = argv[1];
        char** names_mas;
        size_t size_of;
        if (!(size_of = get_names(&names_mas, dir_name)))
                {
                        perror("Can't read files");
                        return -1;
                }
        for (size_t i = 0; i < size_of; i++) {
                printf("The file %ld is %s\n", i, names_mas[i]);
        }
        printf("All number of files is %ld\n", size_of);
        int64_t sum_files_size;
        int64_t* sizes_mas = getFilesSize(&sum_files_size, size_of, names_mas);
        if (sizes_mas == NULL) {
                perror("Can't get the int\'s mas");
                return -1;
        }
        for (size_t i = 0; i < size_of; i++) {
                printf("Sizeof %s is %"PRId64"\n", names_mas[i], sizes_mas[i]);
        }
        printf("Summar size if %ld\n", sum_files_size);
	
	char* contain_mas = files_contains(size_of, names_mas, sizes_mas, sum_files_size);
       	size_t chunk_size = sum_files_size / thread_num;
	printf("Size of chunk: %ld\n", chunk_size);	
	for (size_t i = 0;  i < size_of; i++) {
		free(names_mas[i]);
	}
	free(names_mas);
	free(sizes_mas);
	if (munmap(contain_mas, sum_files_size) == -1) {
		perror("Munmap: ");
	}
}
