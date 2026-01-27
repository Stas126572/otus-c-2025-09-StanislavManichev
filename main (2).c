#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <time.h>

#define PORT "23"
#define BUF_SIZE 16384

typedef enum { STATE_CONNECTING, STATE_COMMUNICATING, STATE_DONE } conn_state_t;

struct connection {
    int fd;
    struct sockaddr_storage addr;
    socklen_t addr_len;
    conn_state_t state;
    char buffer[BUF_SIZE];
    size_t total;
};

void filter_telnet(char *buf, size_t *len) {
    size_t j = 0;
    for (size_t i = 0; i < *len; i++) {
        if ((unsigned char)buf[i] == 255) { 
            i += 2; 
            continue;
        }
        buf[j++] = buf[i];
    }
    *len = j;
    buf[j] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <font> <text>\n", argv[0]);
        return 1;
    }

    char cmd[512];

	snprintf(cmd, sizeof(cmd), "figlet /%s", argv[1]);

  for (int i = 2; i < argc; i++) {
    strcat(cmd, " ");
    strcat(cmd, argv[i]);
  }
  strcat(cmd, "\r\n");
	
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    while (getaddrinfo("telehack.com", PORT, &hints, &res) != 0)
		{
			poll(NULL, 0, 500);
		}

    int num_ips = 0;
    for (p = res; p != NULL; p = p->ai_next) num_ips++;

    struct connection *conns = calloc((size_t)num_ips, sizeof(struct connection));
    struct pollfd *fds = calloc((size_t)num_ips, sizeof(struct pollfd));
    
    p = res;
    for (int i = 0; i < num_ips; i++) {
        conns[i].fd = socket(AF_INET, SOCK_STREAM, 0);
        if (conns[i].fd >= 0) {
            fcntl(conns[i].fd, F_SETFL, O_NONBLOCK);
            memcpy(&conns[i].addr, p->ai_addr, p->ai_addrlen);
            conns[i].addr_len = p->ai_addrlen;
            connect(conns[i].fd, (struct sockaddr *)&conns[i].addr, conns[i].addr_len);
        }
        conns[i].state = STATE_CONNECTING;
        p = p->ai_next;
    }
    freeaddrinfo(res);

    int running = 1;
    while (running) {
        for (int i = 0; i < num_ips; i++) {
            fds[i].fd = conns[i].fd;
            fds[i].events = (conns[i].fd >= 0) ? POLLIN | POLLOUT : 0;
            fds[i].revents = 0;
        }

        if (poll(fds, (nfds_t)num_ips, 1000) <= 0) continue;
	for (int i = 0; i < num_ips; i++) {
            if (conns[i].fd < 0 ||  !(fds[i].revents & (POLLIN | POLLOUT))) continue;

            int error = 0;
            socklen_t err_len = sizeof(error);
            getsockopt(conns[i].fd, SOL_SOCKET, SO_ERROR, &error, &err_len);
            if (error != 0) {
                close(conns[i].fd); conns[i].fd = -1;
                continue;
            }

            ssize_t n = recv(conns[i].fd, conns[i].buffer + conns[i].total, BUF_SIZE - 1 - conns[i].total, 0);
            if (n > 0) {
                conns[i].total += n;
                conns[i].buffer[conns[i].total] = '\0';

                filter_telnet(conns[i].buffer, &conns[i].total);

                if (conns[i].state == STATE_CONNECTING) {
                    if (strstr(conns[i].buffer, "Command,") ||  strstr(conns[i].buffer, ".")) {
                        send(conns[i].fd, cmd, strlen(cmd), 0);
                        conns[i].state = STATE_COMMUNICATING;
                        conns[i].total = 0;
                    }
                } else if (conns[i].state == STATE_COMMUNICATING) {
                    if (strstr(conns[i].buffer, "\r\n.") ||  strstr(conns[i].buffer, "\n.")) {
                        
			char *cmd_start = strstr(conns[i].buffer, "figlet /");
    
    if (cmd_start) {
        char *art_start = strchr(cmd_start, '\n');
        
        if (art_start) {
            art_start++;
            
            char *end_ptr = strstr(art_start, "\r\n.");
            if (end_ptr) {
                *end_ptr = '\0'; 
            }

            printf("%s\n", art_start);

        }
    }    
			conns[i].state = STATE_DONE;
                        running = 0; 
                        break;
                    }
                }
            } else if (n == 0 ||  (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
                close(conns[i].fd); conns[i].fd = -1;
            }
        }
    }

    for (int i = 0; i < num_ips; i++) if (conns[i].fd != -1) close(conns[i].fd);
    free(conns); free(fds);
    return 0;
}
