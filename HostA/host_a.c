#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define TIPO_CLIENTE 1  

#define TIPO_SERVIDOR 2 

#define CHAVE_FILA 13 /* chave  da fila */

#define TAMANHO_TEXTO 500 /* tamanho do texto das mensagens  */

#define TAMANHO_MAX 504 

typedef struct msg
{
     long tipoMensagem;
     int pid;
     char texto[TAMANHO_TEXTO];
} mensagem;

int main(int argc, char *argv[])
{
     char texto[500];

     mensagem msg;

     int filaId; /* id da fila do servidor */

     /* if (argc > 1)   /* default se o argumento vier vazio
            strcpy(texto, argv[1]);
       else
            strcpy(texto, "Texto da mensagem");*/

     printf("Digite sua mensagem: \n");

     scanf(" %[^\n]", texto);

     if ((filaId = msgget(CHAVE_FILA, 0)) == -1) // msgget é utilizada para criar uma nova fila de mensagens, ou para obter o identificador da fila msqid de uma fila de mensagens existente no sistema.
     {
          perror("Erro na criacao da fila do servidor");
          exit(1);
     }
     /*      envio de mensagens       */
     strcpy(msg.texto, texto);
     printf("Cliente N: mensagem enviada: %s\n", msg.texto);
     msg.pid = getpid();
     msg.tipoMensagem = TIPO_SERVIDOR;
     if (msgsnd(filaId, &msg, TAMANHO_MAX, 0) == -1) // msgsnd permite a inserção de uma mensagem na fila, -1 em caso de erro
     {
          perror("Erro no envio de sua mensagem");
          exit(1);
     }
     /*      recepcao de mensagem (ack de recepcao)      */
     printf("Cliente: espera de ack do servidor\n");               // As mensagens são buscadas em uma fila por msgrcv()
     if (msgrcv(filaId, &msg, TAMANHO_MAX, TIPO_CLIENTE, 0) == -1) // so depois q servidor sobe que ele printa que recebeu
     {
          perror("Erro na recepcao de sua mensagem"); // A função msgrcv() lê uma mensagem da fila associada ao identificador de fila de mensagens especificado por msqid
          exit(1);                                    // e a coloca no buffer definido pelo usuário apontado por msgp.
     }
     printf("Cliente N %d recebeu um ack da camada N-1 %d\n", getpid(), msg.pid);
     exit(0);
}