/**
 * lexico.h
 *
 * Biblioteca que compreende funcoes referentes ao analisador lexico
 *  para a linguagem ALG.
 *
 * Construcao de compiladores dois
 * Helena de Medeiros Caseli
 *
 * @author Lais Augusta da Silva Meuchi
 * @author Carlos Roberto Silveira Junior
 *
 * @version 2009.10.17
 */

/*diretivas de bibliotecas*/
#ifndef _LEXICO_H_
      #define _LEXICO_H_

/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "padroes.h"
#include "sintatico.h"

/*define*/
/**
 * Macro que retrocede o "cabecote de leitura" um byte (char), utilizada para
 *  recomecar o processamento de um item. Usada apenas neste bloco (.h)
 */
#define RETROCEDER()    fseek(arqOrigem,-1,SEEK_CUR);

/*variavel global estatica (vista apenas neste bloco .h)*/
static FILE *arqOrigem; /**para abrir o codigo-fonte de origem*/
static unsigned int linhaAtual; /**contagem de linhas*/

/*funcoes e procedimentos*/

/**
 * tCodigoTokeb buscarPalavraReservada(const char*);
 *
 * Faz uma busca na tabela de palavras reservadas usando o principio de dividir
 *  e conquistar, assim como em uma avl. A busca eh feita verificado se o
 *  elemento do meio eh maior ou nao que o elemento buscado e assim orienta a
 *  proxima etapa de busca. Sendo para tanto sua complexidade algoritmica O(lg n).
 *
 * @param palavra: const char* palavra a ser buscada.
 * @return zero se nao encontrou.
 */
tCodigoToken buscarPalavraReservada(const char *palavra) {

      auto short unsigned int superior = TOTAL_DE_RESERVADAS, inferior = 0; /*limites da busca*/
      auto short signed int meio, comparacao, meioAntigo = !meio;

      while(meioAntigo != meio) {

             meioAntigo = meio;
             /*encontrando o meio levando em consideracao os limites atuais*/
             meio = (short int)(0.5 * (float)superior + 0.5 * (float) inferior);

             comparacao = strcmp(palavra, tabelaPalavrasReservadas[meio]);

             /*palavra mais a direita*/
             if(comparacao < 0)
                    superior = meio;
             else
                    if(comparacao) /*palavra mais a esquerda*/
                           inferior = meio;
                    else
                           return meio + INICIO_PALAVRAS_RESERVADAS;
      }

      return 0;
}

/**
 * tControle iniciarAnalisadorLexico(const char*);
 *
 * Funcao tem por finalidade abrir o arquivo que o analisador lexico utilizara,
 *  o nome do proposto arquivo eh recebido como parametro, se nao houve erros
 *  eh retornado SUCESSO, caso contrario ERRO_AO_ABRIR_ORIGEM (caso o arquivo
 *  nao exista ou falhe ao abrir).
 *
 * @param nomeArquivo: char* recebe o nome do arquivo origem
 * @return tControle SUCESSO ou ERRO_AO_ABRI_ORIGEM
 */
tControle iniciarAnalisadorLexico(const char *nomeArquivo) {

       linhaAtual = 1;
       return !(arqOrigem = fopen(nomeArquivo, "rt")) ? ERRO_AO_ABRIR_ORIGEM : SUCESSO;
}

/**
 * void fecharAnalisadorLexico(void);
 *
 * Funcao tem por finalidade fechar o arquivo que o analisador lexico utilizou.
 */
void fecharAnalisadorLexico(void) {

      fclose(arqOrigem);
}

/**
 * unsigned int getLinhaAtual(void);
 *
 * Funcao retorna o numero da linha que se encontra o token atual.
 *
 * @return unsigned int, linha do ultimo token retornado pelo getToken().
 */
unsigned int getLinhaAtual(void) {

      return linhaAtual;
}

/**
 * tControle ignoraComentarios(tToken*);
 *
 * Funcao chamada quando encontrou-se um '{' serve para ignorar tudo ate
 *  encontrar um '}'. Porem considerendo que o '{' ja foi consumido. Fazendo
 *  para tanto a atualizacao das linhas quando necessario.
 *
 * @param saida tToken* : retorna a linha e o codigo caso de erro.
 *
 * @return retorna SUCESSO se ignorou o comentario inteiro e FIM_DE_ARQUIVO
 *  caso tenha atingido o final do arquivo antes de fechar o comentario.
 */
tControle ignoraComentarios(tToken *saida) {

      /*automato de comentario segundo estado*/
      auto char lido;

      /*descobrir inicio do comentario*/
      saida->signo = COMENTARIO_ERRO;

	/*consome o '{'*/
	fread(&lido, sizeof(char), 1, arqOrigem);

      /*consome tudo que o programa apresenta ate encontro um '}'*/
      while(fread(&lido, sizeof(char), 1, arqOrigem) && lido != '}')
             /*verifica se houve mudanca de linha*/
             if(lido == NOVA_LINHA)
                    ++linhaAtual;

      return lido == '}' ? SUCESSO : FIM_DE_ARQUIVO;
}

/**
 * void automatoDigitos(tToken*);
 *
 * Procedimento do automato que reconhece numeros inteiros e reais.
 *
 * @param saida tToken* : retorna o token com o numero.
 */
void automatoDigitos(tToken *saida) {

      auto char digito;
      auto unsigned short i = 0;

      /*q1 - le os digitos da parte inteira*/
      while(fread(&digito, sizeof(char), 1, arqOrigem) && digito >= '0' && digito <= '9')
                    saida->cadeia[i++] = digito;

      if(digito != ',') /*q numero_inteiro*/
             saida->signo = NUMERO_INTEIRO;
      else { /*q2*/

             /*concatena virugla a cadeia*/
             saida->cadeia[i++] = ',';

             /*continua a leitura apos a virgula*/
             while(fread(&digito, sizeof(char), 1, arqOrigem) &&
                   digito >= '0' && digito <= '9')
                    saida->cadeia[i++] = digito;

             /*verifica se apos a virgula havia outros numeros*/
             if(saida->cadeia[--i] == ',') {
                    /*nao houve outro numero apos a virgula; exemplo: "1,a"*/
                    RETROCEDER()
                    saida->signo = NUMERO_INTEIRO;
             }
             else {
                    /*houve numeros apos a virgula; exemplo "2,1"*/
                    ++i; /*para nao perder digito ao colocar '/0'*/
                    saida->signo = NUMERO_REAL;
             }
      }

      /*colocando o terminador e acertando cabecote de leitura*/
      saida->cadeia[i] = '\0'; /*i esta sempre um a frente do indice*/
      RETROCEDER()
}

/**
 * void automatoMenor(tToken*);
 *
 * Reconhece cadeias iniciadas com menor ( <, <=, <>)
 *
 * @param saida : tToken* retorna o token
 */
void automatoMenor(tToken *saida) {

      auto char proximo;

      /*le o menor*/
      fread(&saida->cadeia[0], sizeof(char), 1, arqOrigem);

      /*lendo o proximo digito, '>' ou '=' ou ?*/
      fread(&proximo, sizeof(char), 1, arqOrigem);

      if(proximo == '>' || proximo == '=') {
             saida->cadeia[1] = proximo;
             saida->cadeia[2] = '\0';
      }
      else {
             saida->cadeia[1] = '\0';
             RETROCEDER()
      }

      saida->signo = buscarPalavraReservada(saida->cadeia);
}

/**
 * void automatoMaior(tToken*);
 *
 * Reconhece cadeia comecada com maior: >= ou >
 *
 * @param saida tToken* : retorna o token reconhecido
 */
void automatoMaior(tToken *saida) {

      auto char proximo;

      /*le o maior*/
      fread(&saida->cadeia[0], sizeof(char), 1, arqOrigem);

      /*lendo o proximo digito, '=' ou ?*/
      fread(&proximo, sizeof(char), 1, arqOrigem);

      if(proximo == '=') {
             saida->cadeia[1] = '=';
             saida->cadeia[2] = '\0';
      }
      else {
             saida->cadeia[1] = '\0';
             RETROCEDER()
      }

      saida->signo = buscarPalavraReservada(saida->cadeia);
}

/**
 * void automatoDoisPontos(tToken*);
 *
 * Reconhece cadeia comecada com dois pontos: := ou :
 *
 * @param saida tToken* : retorna o token reconhecido
 */
void automatoDoisPontos(tToken *saida) {

      auto char proximo;

      /*le o maior*/
      fread(&saida->cadeia[0], sizeof(char), 1, arqOrigem);

      /*lendo o proximo digito, '=' ou ?*/
      fread(&proximo, sizeof(char), 1, arqOrigem);

      if(proximo == '=') {
             saida->cadeia[1] = '=';
             saida->cadeia[2] = '\0';
      }
      else {
             saida->cadeia[1] = '\0';
             RETROCEDER()
      }

      saida->signo = buscarPalavraReservada(saida->cadeia);
}

/**
 * void automatoOutros(tToken*);
 *
 * Reconhece outros simbulos, simples da linguagem, assim como '(', ')' ...
 *
 * @param saida tToken* : retorna o token reconhecido
 */
void automatoOutros(tToken *saida) {

      /*le um simbulo e coloca o terminador*/
      fread(&saida->cadeia[0], sizeof(char), 1, arqOrigem);
      saida->cadeia[1] = '\0';

      /*verifica se o simbulo eh reservado, senao erro.*/
      if(!(saida->signo = buscarPalavraReservada(saida->cadeia)))
             saida->signo = TOKEN_ERRO;
}

/**
 * void automatoIdent(tToken*)
 *
 * Reconhece identificadore e palavras reservadas.
 *
 * @param saida tToken* : retorna o token reconhecido
 */
void automatoIdent(tToken *saida) {

       auto char ehId = 0; /*decidir se consulta tabela reservada*/
       auto unsigned short int i = 0;

       /*ledo do arquivo a cadeia e testando-a*/
       while(fread(&saida->cadeia[i], sizeof(char), 1, arqOrigem) &&
             ((saida->cadeia[i] >= 'a' && saida->cadeia[i] <= 'z') ||
              (saida->cadeia[i] >= 'A' && saida->cadeia[i] <= 'Z') ||
              (saida->cadeia[i] >= '0' && saida->cadeia[i] <= '9')))
              if(saida->cadeia[i++] < 'a' || saida->cadeia[i - 1] > 'z')
                     ehId = 1;

       /*retrocede o outro*/
       RETROCEDER()

       /*finalizando cadeia*/
       saida->cadeia[i] = '\0';

       /*se a cadeia tiver A-Z ou 0-9 cai direto em identificador*/
       if(ehId)
              saida->signo = IDENTIFICADOR;
       else {
              saida->signo = buscarPalavraReservada(saida->cadeia);

              if(!saida->signo) /*se cadeina nao eh reservada*/
                     saida->signo = IDENTIFICADOR;
       }
}

/**
 * tControle getToken(tToken*);
 *
 * Funcao que retorna atravez de seu parametro o token lido e identificado pelo
 *  analizador lexico.
 *
 * @param saida tToken* : tokens lido e identificado
 *
 * @return tControle: retorna SUCESSO se o token foi lido e FIM_DE_ARQUIVO caso
 *                     o arquivo tenha terminado
 */
tControle getToken(tToken *saida) {

      auto char lido;

      /*ignorando espacos, tabs e novas linhas*/
      do {
             if(!fread(&lido, sizeof(char), 1, arqOrigem))
                    return FIM_DE_ARQUIVO;
             else
                    if(lido == NOVA_LINHA)
                           ++linhaAtual;
      } while(lido == ' ' || lido == NOVA_LINHA || lido == TAB);

      /*retrocedendo para entrar no automado a partir do primeiro simbulo*/
      RETROCEDER()

      /*linha do proximo token*/
      saida->linha = getLinhaAtual();

      /*escolhendo automato a partir dos primeiros simbulos*/
      if(lido == '{') {
             /*automato que ignora comentarios*/
             if(ignoraComentarios(saida) == FIM_DE_ARQUIVO)
                    return SUCESSO;
             else
                    return getToken(saida);
      }
      else
             if(lido >= '0' && lido <= '9')
                    /*automato que reconhece numeros reais e inteiros*/
                    automatoDigitos(saida);
             else
                     if(lido == '<')
                            /*automato que reconhece os menor*/
                            automatoMenor(saida);
                     else
                            if(lido == '>')
                                   /*automato que reconhece o maior*/
                                   automatoMaior(saida);
                            else
                                   if(lido == ':')
                                          /*automato que reconhece dois pontos*/
                                          automatoDoisPontos(saida);
                                   else
                                          if((lido >= 'a' && lido <= 'z') ||
                                             (lido >= 'A' && lido <= 'Z'))
                                                 /*automato dos identificadores*/
                                                 automatoIdent(saida);
                                          else
                                                 /*automato dos outros simbulos
                                                  * unicos*/
                                                 automatoOutros(saida);

      /*relata erros lexico e passa o próximo token*/
      if (saida->signo == TOKEN_ERRO) {
    	  mensagemErro("Token invalido.");
    	  return getToken(saida);
      }

      return SUCESSO;
}

#endif
