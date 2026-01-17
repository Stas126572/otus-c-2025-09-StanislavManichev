#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <wordexp.h>
#define NAME "/tmp/echo.socket"

int daemonizate() {
  pid_t pid = fork();
  if (pid < 0)
    return 1;
  if (pid > 0)
    exit(0);
  if (setsid() < 0)
    exit(1);
  chdir("/");
  close(stdin);
  close(stdout);
  close(stderr);
}

int main(int argc, char **argv) {
  char *file_path;
  wordexp_t exp_result, exp_result2;
  wordexp("~/.config/file-size.conf", &exp_result, 0);
  file_path = strdup(exp_result.we_wordv[0]);
  wordfree(&exp_result);
  FILE *file = fopen(file_path, "r");

  if (file == NULL) {
    perror("Can't find file path. The configuration need be in "
           "~/.config/file-size.conf");
    return -1;
  }
  printf("Sucefull opened file %s\n", file_path);
  char *buf_;
  ssize_t size;
  if (!(buf_ = (char *)malloc(256 * sizeof(char)))) {
    perror("Can't allocated memory\n");
  }
  if ((size = fread(buf_, 1, 255, file)) < 0) {
    perror("Can't read file\n");
  }

  if (!(file_path = (char *)malloc(size * sizeof(char)))) {
    perror("Can't allocated memory\n");
  }

  int j = 0;
  int started_file_path = 0;
  for (int i = 0; i < size; i++) {
    if (started_file_path) {
      file_path[j] = buf_[i];
      j++;
    } else {
      if (buf_[i] == '=')
        started_file_path = 1;
    }
  }

  file_path[j] = '\0';
  file_path[strcspn(file_path, "\r\n")] = 0;

  int result = wordexp(file_path, &exp_result2, 0);
  if (result != 0) {
    perror("Error in wordexp. Please check the your format of configuration "
           "file. It need be like this: file_path=<You path to file>");
    return -1;
  }

  if (exp_result2.we_wordc <= 0) {
    perror("Strange file path. Please check it again");
    return -1;
  }

  file_path = strdup(exp_result2.we_wordv[0]);
  wordfree(&exp_result2);
  printf("File configuration is OK.\n Next it may'be daemonizate, to connect "
         "with socket you need to use: %s file\n",
         NAME);

  char buf[100];
  snprintf(buf, sizeof(buf), "%ld", (long)size);

  if (daemonizate()) {
    printf("Can't daemonizate, will work standart");
  }

  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0)
    exit(1);

  struct sockaddr_un server;
  server.sun_family = AF_UNIX;
  strcpy(server.sun_path, NAME);

  unlink(NAME);
  if (bind(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_un))) {
    exit(1);
  }
  listen(sock, 5);

  for (;;) {
    int msgsock = accept(sock, NULL, NULL);
    if (msgsock == -1)
      break;

    struct stat file_stat;
    if (stat(file_path, &file_stat) >= 0) {
      size = file_stat.st_size;
      snprintf(buf, sizeof(buf), "%ld\n", (long)size);
    } else {
      snprintf(buf, sizeof(buf), "%s",
               "Can't get the information. Please check file existence and "
               "file rights.");
    }

    send(msgsock, buf, strlen(buf), 0);

    close(msgsock);
  }
  close(sock);
  unlink(NAME);
  return 0;
}
