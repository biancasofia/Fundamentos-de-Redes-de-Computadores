# Fundamentos-de-Redes-de-Computadores

Simulador de camada de enlace para a disciplina de Fundamentos de Redes de Computadores 2022/UNB


Fundamentos de Redes de Computadores

**Protótipo de camada de enlace de dados**

Brasília, DF 2022

1\. Introdução

Segundo (Tanenbaum, 2011), a missão da camada de enlace é fornecer interface de serviço para a camada de rede e transmitir bits à máquina destino, entregando-os até a camada de rede da mesma. Nesse contexto, a divisão do fluxo de bits advindos da camada física em quadros ocorre por meio de algumas técnicas, como:

1. contagem de caracteres;
1. bytes de flag com inserção de bytes;
1. flags iniciais e finais, com inserção de bits; e
1. violações de codificação da camada física

No enquadramento através da contagem de caracteres, um campo com um número de caracteres é adicionado ao cabeçalho do quadro. Assim, a camada de enlace é responsável, também, por detectar erros e corrigi-los, quando necessário . Esta camada também oferece algumas possibilidades de serviços, podendo ser serviço não orientado a conexões sem confirmação, serviço não orientado a conexões com confirmação e serviço orientado a conexões com confirmação.

Nesse sentido, o objetivo deste trabalho é implementar um protótipo que simula a camada de enlace, usando o serviço não orientado a conexões, com o diálogo ponto-a-ponto.

Figura 1: diálogo entre hosts. (Fonte Notas de aula)

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.001.png)

2. Diário de atividades

20/11 - 23/11-> Para realização do desenvolvimento do projeto, antes de começar a prática, cada integrante do grupo realizou um estudo teórico do conteúdo através dos vídeos e apostilas oferecidos pelo professor.

24/11 - 25/11 -> Em seguida, foi feito um estudo do código também disponibilizado pelo docente para um melhor entendimento de como funcionava a comunicação entre os hosts via socket.

26/11 -> Feito isso, evolui-se com o estudo, não apenas só os recomendados, para saber como realizar a comunicação por meio de fila de mensagens entre a camada N e a camada N-1 (conforme a Fig. 1).

27/11 -> Prontamente, foi dirigida para a parte de codificação, em que foi feita parte da implementação da fila de mensagem e alteração do código original propiciado pelo professor para utilização da turma.

28/11 - 29/11 -> Foi terminada a implementação da fila de mensagem. E estava funcionando de forma básica a comunicação entre os hosts.

30/11 - 31/11 -> Estudo de como implementar a negociação de tamanho das strings. 01/11 -> Tentativa frustrada de implementação da negociação.

03/11 -> Aula de dúvida com o professor e entendimento.

03/12- 04/12 -> Desenvolvimento do relatório e implementação da negociação de tamanho das strings.

Durante o percurso do desenvolver do projeto, foi enfrentado alguns desafios já que foi a primeira vez em que os integrantes do grupo se envolviam com o referido tema, entretanto, foi resolvido com bastante esforço e pesquisa em que resultou em sucesso entre a comunicação entre os hosts por meio da comunicação simplex. Todas as etapas de codificação foram feitas em trio.

3. Descrevendo a aplicação
1. Simplex

A aplicação funciona com dois hosts, sendo que cada host possui duas camadas: N e N-1. Dessa forma, o primeiro Host A envia uma mensagem para o outro Host B, sendo que esse último apenas escuta (modo simplex).

Assim, o primeiro passo é captar a mensagem que o lado A envia através da primeira camada, sendo ela recebida e enviada para uma fila de mensagens (será criada) que possui uma chave específica.

Figura 2: Trecho de uso de fila de mensagem. (Fonte: Elaboração própria).![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.002.jpeg)

Após enviar a mensagem para a fila, a aplicação espera que o outro lado receba a mensagem e confirme.

Figura 3: Trecho de código da confirmação de recebimento. (Fonte: Elaboração própria).

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.003.png)

Nesse contexto, a camada mais abaixo busca a mensagem enviada, usando a mesma chave da fila e especificando o tipo de mensagem( pode ser do tipo cliente ou servidor).

Figura 4: Busca por mensagens. (Fonte: Elaboração própria).

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.004.jpeg)

Assim, a mensagem será passada para a camada mais abaixo do host B, sendo que essa comunicação acontece via socket. Como será explicado no item (3.2) sobre negociação, a camada mais abaixo do host A divide a mensagem em pequenas porções e envia para o host B via socket.

Figura 5: Envio via socket. (Fonte: Elaboração própria).

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.005.jpeg)

Após a divisão das mensagens em pequenas porções, a camada abaixo do host A envia uma mensagem via socket informando que a mensagem foi enviada completamente.

Figura 6: Envio via socket fim da mensagem. (Fonte: Elaboração própria).

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.006.png)

Ao chegar no lado B, as funções de recebimento via socket são usadas e as mensagens são concatenadas e até o código de fim de mensagem.

Figura 7: Recebimento via socket. (Fonte: Elaboração própria).

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.007.jpeg)

Nesse ponto, resta apenas que o Host B passe a mensagem para a camada mais acima, ocorrendo o mesmo processo descrito para o Host anterior, ou seja, via fila de mensagens. Assim, a camada N-1 mais abaixo cria a fila, passando a mensagem recebida pelo socket e depois espera a confirmação da última camada.

Figura 8: Envio de mensagem via fila de mensagem. (Fonte: Elaboração própria).

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.008.jpeg)

Figura 9: Recepção de mensagem e confirmação. (Fonte: Elaboração própria)..

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.009.jpeg)

Limitações: Apesar de fazer a detecção de erros, a aplicação não trata a questão de enviar novamente a mensagem quando isso ocorre, nem elabora bons tratamentos para os erros.

2. Negociação entre os lados

A implementação da negociação foi realizada logo no início da execução dos processos das camadas inferiores do lado A e lado B. Após a criação do socket e da vinculação do do socket com a porta. O lado A envia uma mensagem com o tamanho máximo que ele pode enviar e o Lado B recebe essa mensagem, verifica se ele consegue enviar um tamanho maior ou menor e retorna o MENOR valor. O tamanho em que cada lado pode receber e enviar é informado no argumento da execução do processo no terminal.

Figura 10: Negociação do tamanho da mensagem. (Fonte: Elaboração própria)..

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.010.jpeg)

Assim, a cada mensagem que for enviada, o conteúdo dela será dividido no tamanho definido até que se encerre os processos e uma nova negociação só é realizada quando os processos forem executados novamente.

3. Imagens dos Terminais
1. Host A (envia a mensagem)

Figura 11:Terminal do host A camada N. (Fonte: Elaboração própria)..

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.011.png)

Figura 12:Terminal do host A camada N-1. (Fonte: Elaboração própria)..

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.012.png)

2. Host B (recebe mensagem)

Figura 13:Terminal do host B camada N-1. (Fonte: Elaboração própria)

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.013.jpeg)

Figura 14:Terminal do host B camada N. (Fonte: Elaboração própria)

![](img/Aspose.Words.d93ca2fb-e8f8-4929-86be-27cba118c970.014.png)

4. Conclusão

Assim, foi realizado um protótipo que simula a camada de enlace, fazendo a comunicação como mostrado na Figura 1. Nem todos os protocolos da camada foram implementados, mas os principais, como negociar o tamanho da mensagem e o papel básico de passar a mensagem para o outro lado foram feitos com sucesso. 


5. Referências

WETHERALL, J.; TANENBAUM, A. S. Redes de Computadores. 5ª edição. Rio de Janeiro: Editora Campus, v. 5, p. 121, 2011.
