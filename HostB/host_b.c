#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>

#define CHAVE_FILA 14 /* chave  da fila */

#define TAMANHO_TEXTO 500 

#define TAMANHO_MAX 504 

#define TIPO_CLIENTE 1            
#define TIPO_SERVIDOR 2           
#define PODE_LEITURA_ESCRITA 0600 /* flag para permissao leitura e escrita */

typedef struct msg
{
     long tipoMensagem;
     int pid;
     char texto[TAMANHO_TEXTO];
} mensagem;

int main()
{
     mensagem msg; /* mensagem */
     int filaId;   /* identificador da fila de mensagens */

     /*     criacao da fila de mensagens      */
     if ((filaId = msgget(CHAVE_FILA, PODE_LEITURA_ESCRITA | IPC_CREAT)) == -1)
     {
          perror("Erro na criacao da fila");
          exit(1);
     }

     for (;;)
     {
          /*       recepcao de mensagens       */
          if (msgrcv(filaId, &msg, TAMANHO_MAX, TIPO_SERVIDOR, 0) == -1)
          { // tira mensagem da fila
               perror("Erro na recepcao da mensagem");
               exit(1);
          }
          printf("O servidor %d recebeu uma mensagem do cliente %d\nMensagem: %s\n",
                 getpid(), msg.pid, msg.texto);

          /*        envio de mensagem         */
          msg.tipoMensagem = TIPO_CLIENTE;
          msg.pid = getpid();
          if (msgsnd(filaId, &msg, TAMANHO_MAX, 0) == -1)
          {
               perror("Erro no envio de sua mensagem");
               exit(1);
          }
     }
     exit(0);
}