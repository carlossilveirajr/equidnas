/*
 * arvore.h
 *
 *  Created on: 13/11/2009
 *      Author: lais
 */

#ifndef _ARVORE_H_
#define _ARVORE_H_

/*includes*/
#include <stdio.h>
#include <malloc.h>
#include "padroes.h"

/*Novos tipos e estruturas*/

/**
 * @brief No da arquivo.
 *
 * Modela um no da arvore. Tendo todos os campos
 *  relevante para esta
 */
typedef struct sArvore {
	tChave chave; /**chave discriminante*/
	tDados dado; /**dados contido na arvore*/

	struct sArvore *direita; /**filho menor*/
	struct sArvore *esquerda; /**filho maior*/
} tNoArvore;

typedef tNoArvore* tArvore; /**tipo para arvore*/

/*prototipos de funcoes*/
void criarArvore(tArvore*);
tControle inserirArvore(tArvore*, tDados);
tControle buscarArvore(tArvore, char*, tDados*);
void destruirArvore(tArvore*);

static tChave _hashA(char*);

/*funcoes e procedimentos*/
/**
 * Cria uma arvore
 *
 * Inicializa a arvore fazendo-a apontar para NULL.
 *
 * @param semente tArvore* : endereco da arvore.
 */
void criarArvore(tArvore *semente) {
	(*semente) = NULL;
}

/**
 * Insere um elemento na arvore
 *
 * Faz a insercao de um elemento na arvore do modo que deve ser inserido.
 *
 * @param bonsai tArvore* : endereco da arvore.
 * @param valor tDados: dados e ser inserido.
 *
 * @return SUCESSO se inserir e ERRO caso contrario.
 */
tControle inserirArvore(tArvore *bonsai, tDados valor) {

	auto tChave key = _hashA(valor.nome);
	auto tArvore *aux = bonsai;

	while (*aux)
		if ((*aux)->chave > key)
			aux = &((*aux)->direita);
		else
			if((*aux)->chave < key)
				aux = &((*aux)->esquerda);
			else
				return ERRO;

	*aux = (tArvore) malloc(sizeof(tNoArvore));
	(*aux)->direita = (*aux)->esquerda = NULL;
	(*aux)->chave   = key;
	(*aux)->dado    = valor;

	return SUCESSO;
}

/**
 * Busca um elemento na arvore.
 *
 * Busca em arvore nao recursiva, sendo que o dado de saida encontrara
 *  no parametro saida. Pode retornar true ou false.
 *
 * @param arvore tArvore : arvore que sera buscada.
 * @param string char* : elemento a ser buscado.
 * @param saida tDados* : dados de saida.
 * @return SUCESSO ou ERRO se conseguiu encontrar ou nao.
 */
tControle buscarArvore(tArvore arvore, char* string, tDados *saida) {

	auto tChave key = _hashA(string);

	while(arvore)
		if (arvore->chave > key)
			arvore = arvore->direita;
		else
			if (arvore->chave < key)
				arvore = arvore->esquerda;
			else {
				*saida = arvore->dado;
				return SUCESSO;
			}

	return ERRO;
}

/**
 * Destroi a arvore.
 *
 * Desaloca todos os no da arvore e atribui zero a raiz.
 *
 * @param condenado tArvore* : endereco da arvore.
 */
void destruirArvore(tArvore *condenado) {

	if (*condenado) {

		destruirArvore(&(*condenado)->direita);
		destruirArvore(&(*condenado)->esquerda);
		free(*condenado);

		*condenado = NULL;
	}
}

/**
 * Encontra o hash de uma palavra
 *
 * Faz a somatoria do valor ASCII de cada caracter da string que recebe multiplicado pela
 *  sua posicao na string. Funcao para uso interno. Retornara um inteiro de 54 bits.
 *
 * @param string char* : palavra de entrada
 * @return tChave : hash calculado
 */
static tChave _hashA(char *string) {

	auto tChave valor = 0;
	register unsigned short int i = 0;

	while(*string)
		valor += (tChave) ((unsigned short int)(*string++) * ++i);

	return valor;
}

#endif /* ARVORE_H_ */
