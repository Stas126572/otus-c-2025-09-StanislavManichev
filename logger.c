#include "logger.h"



void logger_function(FILE* fp, const char* level_string, const char* message, const char* file_name, int line) {
	
	if (fp == NULL)
	{
		perror("No opened file");
		exit(EXIT_FAILURE);
	}
	
	if (fprintf(fp, "%s(%s, %d): %s\n", level_string, file_name, line, message) < 0)
	{
		perror("Can't write into the file");
		exit(EXIT_FAILURE);
	}

	if (level_string[0] == 'E')
	{
		int nptrs;
		void *buffer[BUF_SIZE];
		char **strings;

		nptrs = backtrace(buffer, BUF_SIZE);	

		if (fprintf(fp, "Backlog of %d function\n", nptrs) < 0)
		{
			perror("Can't write into the file");
			exit(EXIT_FAILURE);
		}

		strings = backtrace_symbols(buffer, nptrs);
 		if (strings == NULL) {
 			perror("backtrace_symbols");
 			exit(EXIT_FAILURE);
 		}

 		for (int j = 0; j < nptrs; j++) {
 			if (fprintf(fp, "%s\n", strings[j])< 0)
        		{
                		perror("Can't write into the file");
               			 exit(EXIT_FAILURE);
        		}
		}
 		free(strings);
	}
}

