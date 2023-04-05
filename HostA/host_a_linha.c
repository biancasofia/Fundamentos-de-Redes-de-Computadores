#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define CHAVE_FILA 13 /* chave  da fila */

#define TAMANHO_TEXTO 500 

#define TAMANHO_MAX 504
/* tamanho total das mensagens */

#define TIPO_CLIENTE 1            
#define TIPO_SERVIDOR 2           
#define PODE_LEITURA_ESCRITA 0600 /* flag permissao leitura e escrita */

typedef struct msg
{
    long tipoMensagem;
    int pid;
    char texto[TAMANHO_TEXTO];
} memsagem;

memsagem falaALinha()
{
    memsagem msg; /* mensagem */
    int filaId;   /* identificador da fila de mensagens */

    /*     criacao da fila de mensagens      */
    if ((filaId = msgget(CHAVE_FILA, PODE_LEITURA_ESCRITA | IPC_CREAT)) == -1)
    {
        perror("Erro na criacao da fila");
        exit(1);
    }

    /*       recepcao de mensagens       */
    if (msgrcv(filaId, &msg, TAMANHO_MAX, TIPO_SERVIDOR, 0) == -1)
    {
        perror("Erro na recepcao de sua mensagem");
        exit(1);
    }
    printf("O servidor %d recebeu uma mensagem do cliente %d\nMensagem: %s\n",
           getpid(), msg.pid, msg.texto);

    /*        envio de mensagem         */
    msg.tipoMensagem = TIPO_CLIENTE;
    msg.pid = getpid();
    if (msgsnd(filaId, &msg, TAMANHO_MAX, 0) == -1) // msgsnd permite a inserção de uma mensagem na fila
    {
        perror("Erro no envio da mensagem");

        exit(1);
    }

    return msg;
}

int main(int argc, char *argv[])
{
    memsagem msg;

    // printf("retorno %s ", msg.texto);
    int socket_desc;
    struct sockaddr_in server_addr;
    char server_message[100], client_message[100];
    int server_struct_length = sizeof(server_addr);

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);

    /* confere o numero de argumentos passados para o programa */
    if (argc < 4)
    {
        printf("uso correto: %s <ip_do_servidor> <porta_do_servidor> <dado1> ... <dadoN> \n", argv[0]);
        return -1;
    }

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

    printf("Negociando tamanho das mensagens...\n");

    char tamanhoNegociado[100];
    memset(tamanhoNegociado, '\0', sizeof(tamanhoNegociado));
    strcpy(tamanhoNegociado, argv[3]);

    if (sendto(socket_desc, tamanhoNegociado, strlen(tamanhoNegociado), 0, (struct sockaddr *)&server_addr, server_struct_length) < 0)
    {
        printf("Não foi possível enviar a mensagem\n");
        return -1;
    }

    char tamanhoRecebido[100];
    memset(tamanhoRecebido, '\0', sizeof(tamanhoRecebido));
    if (recvfrom(socket_desc, tamanhoRecebido, sizeof(tamanhoRecebido), 0, (struct sockaddr *)&server_addr, &server_struct_length) < 0)
    {
        printf("Não foi possível receber a mensagem\n");
        return -1;
    }

    printf("Tamanho negociado = %s\n", tamanhoRecebido);

    while (1)
    {
        memset(server_message, '\0', sizeof(server_message));
        memset(client_message, '\0', sizeof(client_message));

        msg = falaALinha();

        // Mandar mensagem pro servidor:
        int i, j;
        char mensagemCortada[10000];

        // int tamanhoRecebido = 4;
        // printf("tamanho %d\n", strlen(msg.texto));
        for (j = 0; j < strlen(msg.texto); j = j + atoi(tamanhoRecebido))
        {
            memsagem msgCortada;
            int aux = 0;
            // printf("j %d\n", j);
            memset(mensagemCortada, '\0', sizeof(mensagemCortada));
            for (i = j; aux < atoi(tamanhoRecebido); i++)
            {
                //   printf("i e j %d %d\n", i, j);

                msgCortada.texto[aux] = msg.texto[i];
                // printf("cortando %c\n", msgCortada.texto[aux]);
                aux++;
            }
            printf("Enviando quadros contendo texto: %s\n", msgCortada.texto);
            if (sendto(socket_desc, (struct mensagem *)&msgCortada, sizeof(msgCortada), 0, (struct sockaddr *)&server_addr, server_struct_length) < 0)
            {
                printf("Não foi possível enviar a mensagem\n");
                return -1;
            }
        }
        // para indicar que acabou
        memsagem msgFim;
        strcpy(msgFim.texto, "Fim");
        if (sendto(socket_desc, (struct mensagem *)&msgFim, sizeof(msgFim), 0, (struct sockaddr *)&server_addr, server_struct_length) < 0)
        {
            printf("Não foi possível enviar a mensagem\n");
            return -1;
        }

        /// indicar que acabou
        /*  if (sendto(socket_desc, "*", sizeof("*"), 0, (struct sockaddr *)&server_addr, server_struct_length) < 0)
              {
                  printf("Não foi possível enviar a mensagem\n");
                  return -1;
              }*/
    }
    // Close the socket:f
    close(socket_desc);

    return 0;
}
