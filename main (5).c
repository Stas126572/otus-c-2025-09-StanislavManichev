#include "logger.h"

int main() {
	char * filename = "data.txt";
    	FILE * fp = fopen(filename, "w");
    	if(fp) {	
		LOG_DEBUG(fp, "Hello World!");
		LOG_INFO(fp, "Hello World!");
		LOG_WARNING(fp, "Hello World!");
		LOG_ERROR(fp, "Hello World!");
		fclose(fp);
	}
	return 0;
}
