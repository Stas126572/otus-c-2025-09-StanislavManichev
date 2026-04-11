#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REPEATE_2(x) (x), (x)
#define REPEATE_4(x) REPEATE_2(x), REPEATE_2(x)
#define REPEATE_8(x) REPEATE_4(x), REPEATE_4(x)

typedef enum Error_Type { NULL_ERROR, INCORRECT_ARGC, NOT_INTEGER, SUCCESS } Error_Type;

static int count_values(void *unused __attribute__((unused)), int argc,
                        char **argv, char **col_name __attribute__((unused))) {
  if (argc != 5)
    return 1;
  printf("AVG: %d\nMAX: %d\nMIN: %d\nSUM: %d\nVAR: %d\n", atoi(argv[0]),
         atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
  return 0;
}

static int check_type(void *error_type, int argc, char **argv,
                      char **col_name __attribute__((unused))) {
  if (argc != 1) {
          *((Error_Type *)error_type) = INCORRECT_ARGC;
          return 1;
  }
  if (strcmp(argv[0], "null") == 0) {
    *((Error_Type *)error_type) = NONE;
    return 1;
  }
  if (strcmp(argv[0], "integer") != 0) {
    *((Error_Type *)error_type) = NOT_INTEGER;
    return 1;
  }
  *((Error_Type *)error_type) = SUCCESS;
  return 0;
}

int main(int argc, char **argv) {
  int code = EXIT_SUCCESS;

  if (argc < 4) {
    fprintf(stderr, "Incorrect format.");
    code = EXIT_FAILURE;
    goto cleanup;
  }

  sqlite3 *db = NULL;
  if (sqlite3_open(argv[1], &db)) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    code = EXIT_FAILURE;
    goto cleanup;
  }

  char query[200];
  char *column = argv[2];
  char *table = argv[3];
  char *err_message = NULL;

  Error_Type error_type;
  snprintf(query, 200, "SELECT typeof(%s) FROM %s LIMIT 1;", column, table);

  if ((sqlite3_exec(db, query, check_type, (void *)&error_type,
                    &err_message)) != SQLITE_OK) {
    switch (error_type) {
    case NONE:
      fprintf(stderr, "Can't find this column.");
      break;
    case NOT_INTEGER:
      fprintf(stderr, "The column isn't integer type.");
      break;
    case INCORRECT_ARGC:
      fprintf(stderr, "Incorrect column of argv.");
      break;
    default:
      fprintf(stderr, "SQL Error: %s\n", err_message);
    }
    code = EXIT_FAILURE;
    goto cleanup;
  }

  snprintf(query, 200,
           "SELECT AVG(%s) AS average, MAX(%s) AS max, MIN(%s) AS min, SUM(%s) "
           "AS sum, AVG(%s * %s) - AVG(%s) * AVG(%s) AS variable FROM %s",
           REPEATE_8(column), table);
  if ((sqlite3_exec(db, query, count_values, 0, &err_message)) != SQLITE_OK) {
    fprintf(stderr, "SQL Error: %s\n", err_message);
    code = EXIT_FAILURE;
    goto cleanup;
  }

cleanup:
  if (db != NULL)
    sqlite3_close(db);
  if (err_message != NULL)
    sqlite3_free(err_message);
exit:
  return code;
}
