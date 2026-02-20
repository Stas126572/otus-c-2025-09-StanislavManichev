#include "get_files_contains.h"

char* files_contains(int64_t files_count, char** files_name, int64_t* files_size, int64_t summar_size)
{
	

         char *total_buffer = (char*)mmap(NULL, summar_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        size_t size_have_readen = 0;
	for (int64_t i = 0; i < files_count; i++)
	{	
        printf("%s\n", files_name[i]);
	int fd = open(files_name[i], O_RDONLY);
	if (fd == -1) {
		perror("Can't open file");
		exit(-1);
	}
	void* res = mmap(total_buffer + size_have_readen, files_size[i], PROT_READ, MAP_PRIVATE | MAP_FIXED, fd, 0);
	if (res == MAP_FAILED) {
    fprintf(stderr, "Ошибка mmap для файла %s на смещении %ld: %m\n", files_name[i], size_have_readen);
}
	size_have_readen += files_size[i];
	close(fd);
	}

	return total_buffer;;
}

void read_files_contains(char* buffer, int64_t summar_size) {
	for (int i = 0; i < summar_size; i++)
	{

		if (buffer[i] =='\0')
		{
			continue;
		}
		putc(buffer[i], stdout);
	}
}
