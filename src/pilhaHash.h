/**
 * Pilha de dados utilizada para escopo local.
 *
 * Para escopo local e listas temporarias sera utilizada esta pilha.
 *  Basicamente trata-se de uma pilha encadeada, sendo que cada no representa
 *  um argumento.
 *
 * @author Lais Augusta da Silva Meuchi
 * @author Carlos Roberto Silveira Junior
 *
 * @version 2009.11.13
 */

#ifndef _PILHAHASH_H_
#define _PILHAHASH_H_

/*includes*/
#include <stdio.h>
#include <malloc.h>
#include "padroes.h"

/*tipos de dados e estruturas*/

/**
 * @brief No da pilha.
 *
 * Representa o no da pilha, que sera distinto por uma chave. O objetivo de possuir uma
 *  chave descriminante em uma pilha eh melhorar o desempenho da mesma.
 */
struct sNodoPilha {

	tChave chave; /**chave para distincao na pilha, primeiros 8 bits escopo, restante hash*/
	tDados dado; /**dado relacionado a chave*/

	struct sNodoPilha *anterior; /**ponteiro para nodo anterior*/

};

typedef struct sNodoPilha tNodoPilha; /**tipo para o no da pilha*/
typedef tNodoPilha* tPilhaHash; /**tipo para pilha, ponteiro para o no topo*/

/*prototipos de funcoes e procedimentos*/
void criarPilha(tPilhaHash*);
short int inserirPilha(tPilhaHash*, tDados*);
short int buscarPilha(tPilhaHash*, char*, tDados*);
tControle pop(tPilhaHash*, tDados*);
void apagarPilha(tPilhaHash*, char*);
void destruirPilha(tPilhaHash*);

static tChave _hash(char*);

/*funcoes e procedimentos*/
/**
 * Faz a criacao da pilha.
 *
 * Recebe um ponteiro para a pilhaHash e inicializa-o com NULL. Isso sera importante para
 *  a insercao do primeiro valor.
 *
 * @param pilha tPilhaHash* : endereco da nova pilha.
 */
void criarPilha(tPilhaHash *pilha) {
	*pilha = NULL;
}

/**
 * Faz a insercao na pilhaHash
 *
 * Recebe um elemento e este sera inserido na pilhaHash. Respeitando o seu escopo e seu nome,
 *  ou seja, o nome e o escopo de um elemento entrarao na confeccao da chave da pilha.
 *
 * @param pilhaNova tPilhaHash* : endereco da pilha que recebera o novo dado.
 * @param valor tDados* : endereco do dado a ser inserido na pilha.
 * @return short int : SUCESSO se conseguiu inserir e ERRO se ja existia este elemento.
 */
short int inserirPilha(tPilhaHash *pilhaNova, tDados *valor) {

	auto tChave key = _hash(valor->nome);
	auto tNodoPilha *pilha;
	pilha = *pilhaNova;

	/*buscara enquanto houver pilha e a chave for diferente*/
	while (pilha && pilha->chave != key)
		pilha = pilha->anterior;

	if (*pilhaNova)
		if (pilha)
			return ERRO;
		else {
			auto tNodoPilha *novo;
			novo = (tNodoPilha*) malloc(sizeof(struct sNodoPilha));
			novo->anterior = *pilhaNova;
			novo->chave = key;
			novo->dado = *valor;

			*pilhaNova = novo;

			return SUCESSO;
		}
	else {
		(*pilhaNova) = (tNodoPilha*) malloc(sizeof(struct sNodoPilha));
		(*pilhaNova)->chave = key;
		(*pilhaNova)->dado = *valor;
		(*pilhaNova)->anterior = NULL;

		return SUCESSO;
	}
}

/**
 * Busca um valor na pilha
 *
 * Busca uma chave na pilha, se encontrar o seu dado sera devolvido em valor.
 *
 * @param pilha tPilhaHash* : endereco da pilha.
 * @param string char* : nome que esta sendo procurado.
 * @param valor tDados* : dado de saida caso no tenha sido encontrado.
 *
 * @return short int : SUCESSO se encontrou e ERRO caso contrario.
 */
short int buscarPilha(tPilhaHash *pilha, char *string, tDados *valor) {

	auto tChave key = _hash(string);
	auto tNodoPilha *aux = *pilha;

	while (aux && aux->chave != key)
		aux = aux->anterior;

	if (aux)
		if (aux->chave == key) {
			*valor = aux->dado;
			return SUCESSO;
		} else
			return ERRO;
	else
		return ERRO;
}

tControle pop(tPilhaHash* pilha, tDados* entrada) {

	if (*pilha) {
		*entrada = (*pilha)->dado;

		auto tNodoPilha *aux = (*pilha);

		*pilha = aux->anterior;
		free(aux);

		return SUCESSO;
	} else
		return ERRO;
}

/**
 * Apaga um elemento
 *
 * Faz a remocao de um elemento da pilha, eh necessario apenas a sua identificacao.
 *
 * @param pilha tPilhaHash* : endereco da pilha.
 * @param string char* : string procurada.
 */
void apagarPilha(tPilhaHash *pilha, char *string) {

	auto tChave key = _hash(string);
	auto tNodoPilha *aux = *pilha, *proximo = *pilha;

	while (aux && aux->chave != key) {
		proximo = aux;
		aux = aux->anterior;
	}

	if (!aux) {
		if (*pilha == aux)
			*pilha = aux->anterior;
		else
			proximo->anterior = aux->anterior;

		free(aux);
	}
}

/**
 * Destroi a pilha.
 *
 * Faz a desalocacao de todas as estrutura.
 *
 * @param pilha tPilhaHash* : pilha de entrada.
 */
void destruirPilha(tPilhaHash *pilha) {

	auto tNodoPilha *condenado;
	while (*pilha) {
		condenado = *pilha;
		*pilha = condenado->anterior;
		free(condenado);
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
static tChave _hash(char *string) {

	auto tChave valor = 0;
	register unsigned short int i = 0;

	while (*string)
		valor += (tChave) ((unsigned short int) (*string++) * ++i);

	return valor;
}

#endif
