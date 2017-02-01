/**
 * @mainpage Compilador Equidnas (ALG para C)
 *
 * @image html equidnas.png
 *
 * <h3>Documentacao do compilador Equidnas. </h3>
 *
 * Compilador para a linguagem ALG, cujo objetivo eh a traducao da linguagem
 *  ALG para a linguagem C, de tal modo que esta possa ser utilizada como entrada para
 *  outro compilador (gcc, por exemplo) e seja compilado corretamente para linguagem
 *  de maquina gerando um executavel. Serao implementadas algumas
 *  etapas do processo de compilacao: analise lexica, analise sintatica,
 *  analise semantica e geracao de codigo. O metodo de tratamente de erro proposto
 *  eh modo panico e as mensagens de erros serao exibida na saida padrao.
 *  Topicos como geracao de codigo intermediario e otimizacao do mesmo e do codigo
 *  alvo nao serao implementados.
 *
 * A saida esperada para o Equidnas eh um arquivo com a extencao .c com o codigo
 *  em C equivalente ao codigo em ALG do arquivo de origem. Ou, exibir os erros
 *  encontrados pelas analises na saida padrao.
 *
 * <h5>Construcao de compiladores dois <br>
 *
 * Helena de Medeiros Caseli</h5>
 *
 * @author Lais Augusta da Silva Meuchi
 * @author Carlos Roberto Silveira Junior
 *
 * @version 2009.11.28
 */

/*includes*/
#include <stdio.h>
#include <string.h>
#include "padroes.h"
#include "lexico.h"
#include "sintatico.h"
#include "semantico.h"
#include "geradorCodigo.h"

/*programa principal*/
/**
 * Funcao principal, ponto de partida.
 *
 * Ponto inicial da compilacao do Equidnas. Esta funcao recebera dois paramentros sendo
 *  que um deles eh optativo; por linha de comando.
 * Inicialmente verificara se os parametros passados estao certo. Depois inicializara os
 *  analisadores, o ultimo a ser inicialisado eh o semantico.
 * Ao terminar a analise sintatica todos os outros analisadores sao terminados
 *  e o programa devolve o controle ao sistema operacional.
 */
int main(int argc, char *argv[]) {

      auto char arqDestino[51];

      /*numero errado de argumentos
       * se argc = 2 entao arqDestino = arqOrigem + ".c"
       * se argc = 3 entao arqDestino = argv[2]
       */
      if(argc != 2 && argc != 3) {

             printf("\n\nCompilador Equidnas: ALG para C\n\n");
             printf("Use: %s arqOrigem arqDestino\n", argv[0]);
             printf("arqOrigem: codigo fonte, parametro obrigatorio\n");
             printf("arqDestino: arquivo de saida, parametro optativo\n\n");
             printf("Lais Augusta da Silva Meuchi\n");
             printf("Carlos Roberto Silveira Junior\n\t\t\t\t2009\n\n");

             return 1;
      }

      /*inicializando analisador lexico com arquivo origem*/
      if(iniciarAnalisadorLexico(argv[1]) == ERRO_AO_ABRIR_ORIGEM) {

             printf("Erro ao abrir o arquivo com o codigo fonte\n");
             return 2;
      }

      /*tratando nome do arquivo de saida*/
      if(argc == 2) {

             /*se o nome nao foi especificado entao usa-se o nome do arquivo de
              * entrada porem com a extencao .c*/
             register unsigned short int i = 0;

             /*copiando arquivo de entrada ate a extencao, final do nome ou
              * limite*/
             while(argv[1][i] != '\0' && argv[1][i] != '.' && i < 48)
                    arqDestino[i] = argv[1][i++];

             arqDestino[i] = '\0';
             /*concatenando nova extencao*/
             strcat(arqDestino,".c");
      }
      else
             /*nome do arquivo especificado*/
             strcpy(arqDestino, argv[2]);

      /*iniciando a analise semantica*/
      analisadorSemantico();

      /*iniciando o gerador de codigo*/
      gerador(arqDestino);

      /*******************************************
       *analise sintatica  desencadeia as  outras*
       *******************************************/
      analiseSintatica();/************************
       *******************************************
       *terminada a analise sintatica e as outras*
       *******************************************/

      /*terminando o analisador lexico*/
      fecharAnalisadorLexico();

      /*terminando o analisador semantico*/
      fecharSemantico();

      /*terminando o gerador de codigo*/
      fecharGerador();

      /*sucesso*/
      return 0;
}
