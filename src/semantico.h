/**
 * Analisador Semantico
 *
 * Fara a analise semantica de um programa em ALG, para tanto uma lista de erros semanticos
 *  sera respeitada que serao pegos e trados foi disponibilizadas e utilizaas para a confeccao
 *  do analisador.
 * A analise semantica eh administrada a medida que a analise sintatica eh feita, por isso
 *  diversos codigos podem encontrar-se entrelacado e espalhado pelo analisador sintatico.
 *
 * @see sintatico.h
 *
 * @author Carlos Roberto Silveira Junior
 * @author Lais Augusta da Silva Meuchi
 *
 * @version 2009.11.19
 */
#ifndef _SEMANTICO_H_
#define _SEMANTICO_H_

/*includes*/
#include <stdio.h>
#include <malloc.h>
#include "padroes.h"
#include "arvore.h"
#include "pilhaHash.h"
#include "lexico.h"
#include "sintatico.h"
#include "geradorCodigo.h"

/*evitar declaracoes implicitas*/
extern void mensagemErro(const char*);

/*variaveis globais*/
static tArvore escopoGlobal; /**arvore que sera utilizada para o escopo global*/
static tPilhaHash escopoLocal; /**pilha que sera utilizada para os escopos locais (de procedimentos por exemplo)*/
tPilhaHash temporaria; /**pilha que sera utilizada temporariamente para facilitar algumas manipulacoes*/

/**
 * Inicia a analise semantica
 *
 * Da inicio a analise semantica, inicialisa suas estruturas de dados.
 */
void analisadorSemantico(void) {

	criarArvore(&escopoGlobal);
	criarPilha(&escopoLocal);
	criarPilha(&temporaria);

}

/**
 * Verifica declaracao de procedimentos
 *
 * Servira na verificacao do escopo de um procedimento, eh interessante no sentido que os procedimentos sao
 *  identificadores que pertencerao apenas ao escopo local.
 *
 * @param token tToken : token com o nome do procedimento.
 * @return SUCESSO se o procedimento ja foi declarado.
 */
tControle semVerDeclProc(tToken token) {

	auto tDados procedimento;

	if (buscarArvore(escopoGlobal, token.cadeia, &procedimento) == SUCESSO)
		if (procedimento.tipo == PROCEDIMENTO)
			return SUCESSO;

	mensagemErro("Procedimento nao declarado.");
	return ERRO;
}

/**
 * Verifica a declaracao de variaveis
 *
 * Servira para verificar se uma variavel ja foi declarada, a verificacao difere-se da de procedimento pelo
 *  fato de variaveis poderem estar em escopo local.
 *
 * @param token tToken : token com o nome da variavel.
 * @return SUCESSO se a variavel existe.
 */
tControle semVerDeclVar(tToken token) {

	auto tDados variavel;

	if (buscarPilha(&escopoLocal, token.cadeia, &variavel) == SUCESSO)
		return SUCESSO;
	else {
		if (buscarArvore(escopoGlobal, token.cadeia, &variavel) == SUCESSO)
			if (variavel.tipo != PROCEDIMENTO)
				return SUCESSO;
	}
	mensagemErro("Variavel nao declarada.");
	return ERRO;
}

/**
 * Verifica declaracoes redundantes
 *
 * Serve para verificar se uma variavel e/ou procedimento ja foi declarado dentro daquele
 *  escopo, no caso de procedimento, apenas global.
 *
 * @param token tToken : token com o nome do procedimento e/ou funcao.
 * @param ehVariavel char : 1 se for variavel e zero se procedimento.
 * @return SUCESSO se nao foi declarado anteriormente.
 */
tControle semVerDeclRedundante(tToken token, char ehLocal) {

	auto tDados variavel;

	if (ehLocal)
		if (buscarPilha(&escopoLocal, token.cadeia, &variavel) != SUCESSO)
			return SUCESSO;
		else
			;
	else if (buscarArvore(escopoGlobal, token.cadeia, &variavel) != SUCESSO)
		return SUCESSO;

	mensagemErro("Variavel/Procedimento ja declarada.");
	return ERRO;
}

/**
 * Verifica Atribuicao
 *
 * Sera utilizado para verificar se um atribuicao eh valida para uma determinada exprecao
 *
 * @return tControle : SUCESSO se a atribuicao eh valida.
 */
tControle semVerAtribuicao(void) {

	auto signed short int ehReal = 0;
	auto tDados saida;

	while (pop(&temporaria, &saida) == SUCESSO)
		if (saida.tipo == NUMERO_REAL || saida.tipo == REAL)
			ehReal = 1;

	if (ehReal)
		if (saida.tipo == NUMERO_INTEIRO || saida.tipo == INTEIRO) {
			mensagemErro("Numero real atribuido a um inteiro.");
			return ERRO;
		}

	return SUCESSO;
}

/**
 * Verifica os argumentos de le e escreve.
 *
 * Sera utilizado para verificar se os argumentos de le/escreve sao de mesmo tipo, para uma
 *  determinada chamada.
 *
 * @return tControle : SUCESSO se a lista de argumentos eh valida.
 */
tControle semVerLeEscreve(void) {

	auto tDados primeiro, saida;
	auto tToken temp;

	if (pop(&temporaria, &primeiro) != SUCESSO) {

		mensagemErro("Comando le/escreve sem parametros.");
		return ERRO;
	} else {

		strcpy(temp.cadeia, primeiro.nome);
		semVerDeclVar(temp);
	}

	while (pop(&temporaria, &saida) == SUCESSO) {
		strcpy(temp.cadeia, saida.nome);
		if (semVerDeclVar(temp) == SUCESSO)
			if (saida.tipo != primeiro.tipo) {
				mensagemErro(
						"Comando Le/Escreve deve manipular variaveis de mesmo tipo.");
				return ERRO;
			}
	}

	return SUCESSO;
}

/**
 * Verifica se os argumentos de um procedimento.
 *
 * Sera utilizado para verificar se os argumentos de um procedimento seguem as especificacoes.
 *
 * @param nomeProc char* : nome do procedimento a ser avaliado. * @return tControle : SUCESSO se a lista de argumentos eh valida.
 */
tControle semVerParametros(char *nomeProc) {

	int contInteiros = 0, contReal = 0;
	auto tDados saida;

	while (pop(&temporaria, &saida) == SUCESSO) {
		if (saida.tipo == NUMERO_INTEIRO || saida.tipo == INTEIRO)
			contInteiros++;
		else
			contReal++;
	}

	if (buscarArvore(escopoGlobal, nomeProc, &saida) == SUCESSO)
		if (saida.genF == contReal && saida.genI == contInteiros) {
			return SUCESSO;
	}
	else
		return ERRO;

	mensagemErro("Numero ou tipo de parametros invalido.");

	return ERRO;
}

/**
 * Manipulacao da pilha temporaria
 *
 * Empilhar na pilha temporaria, para ser utilizada posteriormente por outra
 *  funcao semantica.
 *
 * @param token tToken : o token a ser empilhado
 */
void semEmpilhaTemp(tToken token) {

	auto tDados entrada, temp;

	strcpy(entrada.nome, token.cadeia);
	entrada.tipo = token.signo;

	if (buscarPilha(&escopoLocal, entrada.nome, &temp) == SUCESSO)
		entrada.tipo = temp.tipo;
	else
		if (buscarArvore(escopoGlobal, entrada.nome, &temp) == SUCESSO)
			entrada.tipo = temp.tipo;

	inserirPilha(&temporaria, &entrada);
}

/**
 * Manipulacao da pilha temporaria
 *
 * Desempilha na pilha temporaria, para ser utilizada posteriormente por outra
 *  funcao semantica.
 *
 * @param saida tToken* : o token a ser desempilhado
 */
void semDesempilhaTemp(tToken *s) {

	auto tDados saida;
	pop(&temporaria, &saida);

	strcpy(s->cadeia, saida.nome);
}

/**
 * Manipulacao da pilha temporaria
 *
 * Desempilha e desaloca o pilha temporaria toda.
 *
 */
void semApagaTemp(void) {
	destruirPilha(&temporaria);
}

/**
 * Faz a declaracao de uma variavel
 *
 * Este procedimento sera utilizado para a insercao de uma variavel
 *  na lista de escopo global ou local. Para tanto, faz uso da pilha
 *  de temporarios
 *
 * @param escopo int : escopo da varivavel
 * @param tipo signed char : tipo da variavel (inteiro ou real)
 */
void semDeclareVariaveis(int escopo, signed char tipo) {

	auto tDados saida;
	auto tToken temp;

	if (escopo)
		while (pop(&temporaria, &saida) == SUCESSO) {

			saida.tipo = temp.signo = tipo;
			strcpy(temp.cadeia, saida.nome);

			if (semVerDeclRedundante(temp, 1) == SUCESSO)
				inserirPilha(&escopoLocal, &saida);
		}
	else
		while (pop(&temporaria, &saida) == SUCESSO) {

			saida.tipo = temp.signo = tipo;
			strcpy(temp.cadeia, saida.nome);

			if (semVerDeclRedundante(temp, 0) == SUCESSO)
				inserirArvore(&escopoGlobal, saida);
		}
}

/**
 * Declara um procedimento
 *
 * Coloca um procedimento e seus atributos na arvore do escopo global, junto
 *  a alguns outros processamentos necessarios.
 *
 * @param nome char* : nome do procedimento.
 */
tControle semDeclareProcedimento(char *nome) {

	auto tDados proc;

	strcpy(proc.nome, nome);
	proc.tipo = PROCEDIMENTO;

	auto signed int inteiros = 0, reais = 0;
	auto tPilhaHash aux = escopoLocal;

	while (aux) {
		if (aux->dado.tipo == NUMERO_INTEIRO || aux->dado.tipo == INTEIRO)
			++inteiros;
		else
			++reais;
		aux = aux->anterior;
	}

	proc.genI = inteiros;
	proc.genF = reais;

	if (inserirArvore(&escopoGlobal, proc) != SUCESSO) {
		mensagemErro("Procedimento ja declarado.");
		return ERRO;
	} else
		return SUCESSO;

}

/**
 * Limpa o escopo local
 *
 * Sempre que um procedimento termina sua analise eh necessario limpar
 *  o seu escopo, para que este possa ser utilizado sem problemas por outro
 *  processo.
 */
void semLimpaLocal(void) {
	destruirPilha(&escopoLocal);
}

/**
 * Encerra a analise semantica
 *
 * Faz o fechamento e a desaloca��o das estrutura utilizadas no analisador semantico.
 */
void fecharSemantico(void) {

	destruirArvore(&escopoGlobal);
	destruirPilha(&escopoLocal);
	destruirPilha(&temporaria);
}

#endif /* SEMANTICO_H_ */
