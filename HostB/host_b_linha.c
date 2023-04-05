#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define CHAVE_FILA 14 /* chave  da fila */

#define TAMANHO_TEXTO 500 /* tamanho do texto das mensagens  */

#define TAMANHO_MAX 504 

#define TIPO_CLIENTE 1  
#define TIPO_SERVIDOR 2 

typedef struct msgform
{
    long tipoMensagem;
    int pid;
    char texto[TAMANHO_TEXTO];
} mensagem;

void enviaAi(char client_message[])
{

    mensagem msg;
    int filaId; /* id da fila do servidor */

    if ((filaId = msgget(CHAVE_FILA, 0)) == -1) /*  recupera id da fila de mensagens do servidor       */
    {
        perror("Erro para criar fila do servidor");
        exit(1);
    }

    strcpy(msg.texto, client_message);
    printf("Cliente: envio de mensagem: %s\n", msg.texto);
    msg.pid = getpid();
    msg.tipoMensagem = TIPO_SERVIDOR;
    if (msgsnd(filaId, &msg, TAMANHO_MAX, 0) == -1)
    {
        perror("Erro no envio de mensagem");
        exit(1);
    }
    /*      recepcao de mensagem (ack de recepcao)      */
    printf("Cliente: espera de ack do servidor\n");
    if (msgrcv(filaId, &msg, TAMANHO_MAX, TIPO_CLIENTE, 0) == -1)
    {
        perror("Erro na recepcao de mensagem");
        exit(1);
    }
    printf("Cliente %d recebeu um ack do servidor %d\n",
           getpid(), msg.pid);
}

int main(int argc, char *argv[])
{
    int socket_desc;
    struct sockaddr_in server_addr, client_addr;
    char server_message[500], client_message[500];
    int client_struct_length = sizeof(client_addr);
    mensagem msg;

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    if (argc < 4)
    {
        printf("Digite IP e Porta para este servidor\n");
        return -1;
        ;
    }

    // Create UDP socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_desc < 0)
    {
        printf("Erro ao criar o socket\n");
        return -1;
    }
    printf("Socket criado com sucesso!\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("%s: nao pode fazer bind na porta %s \n", argv[0], argv[2]);
        return -1;
    }
    printf("Vinculação concluída\n");

    char tamanhoRecebidoC[10];
    memset(tamanhoRecebidoC, '\0', sizeof(tamanhoRecebidoC));
    printf("negociando tamanho\n");
    if (recvfrom(socket_desc, tamanhoRecebidoC, sizeof(tamanhoRecebidoC), 0,
                 (struct sockaddr *)&client_addr, &client_struct_length) < 0)
    {
        printf("Não foi possível receber a mensagem\n");
        return -1;
    }
    printf("Tamanho recebido: %s\n", tamanhoRecebidoC);

    if (atoi(argv[3]) <= atoi(tamanhoRecebidoC))
    {
        strcpy(tamanhoRecebidoC, argv[3]);
    }
    printf("Tamanho a ser enviado: %s\n", tamanhoRecebidoC);

    if (sendto(socket_desc, tamanhoRecebidoC, strlen(tamanhoRecebidoC), 0, (struct sockaddr *)&client_addr, client_struct_length) < 0)
    {
        printf("Não foi possível enviar a mensagem\n");
        return -1;
    }

    printf("Esperando por mensagens...\n\n");
    while (1)
    {
        // limpar buffers:
        memset(server_message, '\0', sizeof(server_message));
        memset(client_message, '\0', sizeof(client_message));
        // Recebendo mensagem de cliente:
        int i;
        while (1)
        {
            // memset(msg.texto, '\0', sizeof(msg));
            if (recvfrom(socket_desc, (struct mensagem *)&msg, sizeof(msg), 0,
                         (struct sockaddr *)&client_addr, &client_struct_length) < 0)
            {
                printf("Não foi possível receber a mensagem\n");
                return -1;
            }
            if (strcmp(msg.texto, "Fim") == 0)
            {
                break;
            }
            strcat(client_message, msg.texto);
            printf("Quadro de mensagem recebido. Texto da mensagem: %s\n", msg.texto);
        }
        printf("Mensagem recebida do IP: %s e porta: %i\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        enviaAi(client_message);
        // printf("Texto da mensagem: %s\n", client_message);
    }
    // fechar  socket:
    close(socket_desc);
}