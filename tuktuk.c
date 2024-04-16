#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void error(char *msg){
        fprintf(stderr, "$s: %s\n", msg, strerror(errno));
        exit(1);
}

int main(int argc, char* argv[])
{
        int listener_d = socket(PF_INET, SOCK_STREAM, 0);
        if (listener_d == -1)
                error("Ошибка сокета.");

        struct sockaddr_in name;
        name.sin_family = PF_INET;
        name.sin_port = (in_port_t)htons(30000);
        name.sin_addr.s_addr = htonl(INADDR_ANY);

        int reuse = 1;
        int res = setsockopt(listener_d,
                                        SOL_SOCKET,
                                        SO_REUSEADDR,
                                        (char *)&reuse,
                                        sizeof(int))
        if (res == -1)
                error("Не могу установить для сокета параметр повторного использования.");

        res = bind (listener_d, (struct sockaddr *) &name, sizeof (name));
        if (res == -1)
                error("Не могу привязаться к порту.");

        res = listen(listener_d, 10);
        if (res == -1)
                error("Ошибка прослушивания.");

        puts("Ожидаем подключений...");
        for (;;){
                struct sockaddr_storage client_addr;
                unsigned int address_size = sizeof client_addr;
                int connect_d = accept(listener_d, (struct sockaddr *)&client_addr, &address_size);
                char *msg = "Привет, мир!\r\n";
                res = send(connect_d, msg, strlen(msg), 0);
                if (res == -1)
                        error("Отпрака не удалась.");

                close(connect_d);

        }
        return 0;
}
