#include "get_size_of_file.h"

int64_t getFileSize(const char* file_name){
  int64_t _file_size = 0;
  struct stat _fileStatbuff;
  int fd = open(file_name, O_RDONLY);
  if(fd == -1){
    _file_size = -1;
  }
  else{
    if ((fstat(fd, &_fileStatbuff) != 0) || (!S_ISREG(_fileStatbuff.st_mode))) {
      _file_size = -1;
    }
    else{
      _file_size = _fileStatbuff.st_size;
    }
    close(fd);
  }
  return _file_size;
}

#define ALIGN_UP(addr, size) (((addr) + (size) - 1) & ~((size) - 1))

int64_t* getFilesSize(int64_t* summar_size, int files_count, char** files_names) {
 	long page_size = sysconf(_SC_PAGESIZE);
	*summar_size = 0;
	int64_t* mas = malloc(sizeof(int64_t) * files_count);
 	if (mas == NULL) {
		perror("Malloc: ");
		exit(1);
	}
	for (int i = 0; i < files_count; i++) {
		mas[i] = ALIGN_UP(getFileSize(files_names[i]), page_size);
		*summar_size += mas[i];
		//printf("Sizeof %s is %ld\n", files_names[i], mas[i]);
	}
	return mas;
}

