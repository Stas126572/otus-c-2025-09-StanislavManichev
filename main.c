#define _CRT_SECURE_NO_WARNINGS
#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <string.h>


#ifdef _MSC_VER
	#define snprintf _snprintf_p
#endif

#define REPEATE_2(x) (x), (x)
#define REPEATE_4(x) REPEATE_2(x), REPEATE_2(x)
#define REPEATE_8(x) REPEATE_4(x), REPEATE_4(x)


typedef enum Error_Type
{
	NONE,
	NOT_INTEGER,
	SUCCESS
} Error_Type;

static int count_values(void* unused __attribute__((unused)), int argc, char** argv, char** col_name __attribute__((unused)))
{
	if (argc != 5) return -1;
	printf("AVG: %d\nMAX: %d\nMIN: %d\nSUM: %d\nVAR: %d\n", atoi(argv[0]), atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
	return 0;
}

static int check_type(void* error_type, int argc, char** argv, char** col_name __attribute__((unused)))
{
	if (argc != 1) return -1;
	if (strcmp(argv[0], "null") == 0) {
		*((Error_Type*)error_type) = NONE;
		return EXIT_FAILURE;
	}
	if (strcmp(argv[0], "integer") != 0) {
		*((Error_Type*)error_type) = NOT_INTEGER;
		return EXIT_FAILURE;
	}
	*((Error_Type*)error_type) = SUCCESS;
	return EXIT_SUCCESS;
}



int main(int argc, char** argv)
{
	if (argc < 4)
	{
		fprintf(stderr, "Incorrect format.");
		return EXIT_FAILURE;
	}

	sqlite3* db;
	if (sqlite3_open(argv[1], &db)) {
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return EXIT_FAILURE;
	}

	char query[200];
        char* column = argv[2];
	char* table = argv[3];
	char* err_message = NULL;
	
	Error_Type error_type;
	snprintf(query, 200, "SELECT typeof(%s) FROM %s LIMIT 1;",  column, table);

        
        if ((sqlite3_exec(db, query, check_type, (void*)&error_type, &err_message)) != SQLITE_OK) {
                switch (error_type) {
			case NONE:
				printf("Can't find this column.");
				break;
			case NOT_INTEGER:
				printf("The column isn't integer type.");
				break;
			default:
				printf("SQL Error: %s\n", err_message);
		}
		sqlite3_free(err_message);
		return EXIT_FAILURE;
        }


	snprintf(query, 200, "SELECT AVG(%s) AS average, MAX(%s) AS max, MIN(%s) AS min, SUM(%s) AS sum, AVG(%s * %s) - AVG(%s) * AVG(%s) AS variable FROM %s", REPEATE_8(column), table);
	if ((sqlite3_exec(db, query, count_values, 0, &err_message)) != SQLITE_OK) {
		printf("SQL Error: %s\n", err_message);
		sqlite3_free(err_message);
		return -1;
	}
	
}
