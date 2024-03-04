#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        printf("Error opening socket\n");
        exit(1);
    }

    struct sockaddr_in addr =
        {
            AF_INET,
            // this is the port number 8080 written in hex and in little endian
            htons(8080),
            INADDR_ANY};

    if (bind(sock_fd, &addr, sizeof(addr)) < 0)
    {
        printf("Error binding socket\n");
        exit(1);
    }

    if (listen(sock_fd, 5) < 0)
    {
        printf("Error listening\n");
        exit(1);
    }

    int client_fd;
    if ((client_fd = accept(sock_fd, 0, 0)) < 0)
    {
        printf("Error accepting connection\n");
        exit(1);
    }

    char buffer[256] = {0};
    recv(client_fd, buffer, sizeof(buffer), 0);

    // GET /file.html .....
    char *file = buffer + 5;
    *strchr(file, ' ') = 0;

    FILE *f = fopen(file, "r");
    if (f == 0)
    {
        printf("Error opening file\n");
        exit(1);
    }

    off_t len = 256;
    off_t *len_ptr = &len;
    if (sendfile(client_fd, fileno(f), 0, &len, 0, 0) > 0)
    {
        printf("Error sending file\n");
        exit(1);
    }

    fclose(f);
    close(client_fd);
    close(sock_fd);
    exit(0);
}