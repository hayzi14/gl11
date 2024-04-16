#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
int listener_d = 0;
void error(char *msg){
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
}

int open_listener_socket(void){
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1)
        error("Can't open listener socket.");
    return s;
}

int open_client_socket(){
    static struct sockaddr client_address[1];
    static unsigned int address_size = sizeof(client_address);
    int s = accept(listener_d, client_address, &address_size);
    if (s == -1)
        error("Can't open client socket");
    return s;
}

void bind_to_port(int socket, int port){
    struct sockaddr_in name;
    name.sin_family = PF_INET;
    name.sin_port = (in_port_t)htons(port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    int reuse = 1;
    int res = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR,
                        (char *)&reuse, sizeof(int));
    if (res == -1)
        error("Can't set 'reuse' option on the socket");
    res = bind(socket, (struct sockaddr *)&name, sizeof(name));
    if (res == -1)
        error("Can't bind to socket");
}

int say(int socket, char *s){
    int res = send(socket, s, strlen(s), 0);
    if (res == -1)
        error("Error talking to the client");
    return res;
}

int read_in(int socket, char *buf, int len){
    FILE *fp = fdopen(socket, "r");
    int i = 0, ch;
    while (isspace(ch = fgetc(fp)) && ch != EOF){

    }
    if (ferror(fp))
        error("fgetc");
    while (ch != '\n' && ch != EOF){
        if (i < len)
        buf[i++] = ch;
        ch = fgetc(fp);
    }
    if (ferror(fp))
        error("fgetc");
    while (isspace(buf[--i])){
        buf[i] = '\0';
    }
    return strlen(buf);
}

int main(int argc, char* argv[])
{
	char buf[255];
    int port = 30000;
    listener_d = open_listener_socket();
    bind_to_port(listener_d, port);

    if (listen(listener_d, 10) == -1)
        error("can't listen");
    while (1){
        printf("Ожидается подключения к порту %d\n", port);
        int connect_d = open_client_socket();

        say(connect_d, "Вход на секретный сервер. Скажи пароль\r\n");

        read_in(connect_d, buf, sizeof(buf));

        if (strncasecmp("qwerty", buf, 6)){
            say(connect_d, "Неверный пароль. Досвидания!\r\n");
        }
        else{
            say(connect_d,"Добро пожаловать!\r\n");
        }
        close(connect_d);
    }
    return 0;
}
