/**
 * @brief Faz a traducao do codigo para C.
 *
 * Este modulo eh responsavel pela  traducao do codigo correto de ALG para seu equivalente
 *  em Linguagem C.
 * A Geracao de codigo eh interropda assim que o primeiro erro eh detectado. As funcoes do
 *  gerado sao chamadas pelo analisador sintatico. No entanto, o analisador semantico faz uso
 *  de diversas estruturas de dados pertencentes ao analisador semantico.
 *
 * @see sintatico
 * @see pilhaHash
 * @see semantico
 *
 * @author Carlos Roberto Silveira Junior
 * @author Lais Augusta da Silva Meuchi
 *
 * @versiom 2009.11.21
 */

#ifndef _GERADORCODIGO_H_
#define _GERADORCODIGO_H_

/*includes*/
#include <stdio.h>
#include "padroes.h"
#include "sintatico.h"
#include "semantico.h"

/*variaveis globais*/
static FILE *arquivo; /**Ponteiro para o arquivo destino*/
static char nomeArqDestino[51]; /**nome do arquivo destino*/
extern unsigned int contErros; /**Conta o numero de erros*/
extern tPilhaHash temporaria; /**pilha que sera utilizada pelas declaracoes e processamentos*/

/**
 * Inicializa o gerador de codigo.
 *
 * Faz a abertura do arquivo destino e guarda seu nome.
 *
 * @param nomeArquivo const char* : nome do arquivo destino.
 */
void gerador(const char *nomeArquivo) {

	strcpy(nomeArqDestino,nomeArquivo);

	arquivo = fopen(nomeArquivo,"wt");

	fprintf(arquivo, "#include <stdio.h>\n");
	fflush(arquivo);
}

/**
 * Finaliza o gerador.
 *
 * Fecha o gerador de codigos, sendo que se nao houve erros entao
 *  o arquivo destino sera fechado. Caso contrario o arquivo destino
 *  eh deletado.
 */
void fecharGerador(void) {

	fclose(arquivo);

	if (contErros)
		remove(nomeArqDestino);
}

/**
 * Gera o codigo da declaracao de variaveis
 *
 * Traduz o codigo referente a declaracao de variaveis de um tipo por vez.
 *  Ou seja, de uma linha "var" por vez. As variaveis no código alvo estao
 *  em ordem inversa devido a estrutura de dados utilizada - pilha.
 *
 * @param token tToken : token com os tipos das variaveis.
 */
void gecDecVar(tToken token){

	if (!contErros) {
		auto tPilhaHash aux = temporaria->anterior;

		/*verifica o tipo int/float e codifica a primeira variavel*/
		if (token.signo == INTEIRO)
			fprintf(arquivo, "\nint %s", temporaria->dado.nome);
		else
			fprintf(arquivo, "\nfloat %s", temporaria->dado.nome);

		/*escreve outras as variaveis*/
		while (aux) {
			fprintf(arquivo, ", %s", aux->dado.nome);
			aux = aux->anterior;
		}

		fprintf(arquivo, ";");
		fflush(arquivo);
	}
}

/**
 * Gera a declaracao de argumentos.
 *
 * Este procedimento eh utilizado na declaracao de argumentos. Como estamos
 *  trabalhando com estes argumentos em uma pilha eh necessaria a inversao deles.
 *  Para isso eh utilizada outra pilha auxiliar. O procedimento declara os argumentos
 *  de um tipo por vez.
 *
 * @param token tToken : token com o tipo do argumento.
 */
void gecDecArg(tToken token) {

	if (!contErros) {

		auto tPilhaHash auxInvertida, aux = temporaria;

		criarPilha(&auxInvertida);

		/*invertendo a pilha*/
		while(aux) {
			inserirPilha(&auxInvertida,&aux->dado);
			aux = aux->anterior;
		}

		/*pega os argumentos em ordem*/
		while (auxInvertida) {
			if (token.signo == INTEIRO || token.signo == NUMERO_INTEIRO)
				fprintf(arquivo, "int %s", auxInvertida->dado.nome);
			else
				fprintf(arquivo, "float %s", auxInvertida->dado.nome);

			if ((auxInvertida = auxInvertida->anterior))
				fprintf(arquivo, ", ");
		}

		fflush(arquivo);

		destruirPilha(&auxInvertida);
	}
}

/**
 * Insere uma virgula.
 *
 * Simplesmente coloca uma virgula no lugar onde o cursor do arquivo esta.
 */
void gecAddVirgula(void){

	if(!contErros)
		fprintf(arquivo, ",");
}


/**
 * Insere um ponto-e-virgula.
 *
 * Simplesmente coloca um ponto-e-virgula no lugar onde o cursor do arquivo esta.
 */
void gecAddPontoVirgula(){

	if(!contErros)
		fprintf(arquivo, ";");
}

/**
 * Escreve uma string no arquivo.
 *
 * Recebe uma string e escreve no arquivo destino.
 *
 * @param string char* : string a ser escrita.
 */
void gecGeraCodigo(char *string){

	if(!contErros)
		fprintf(arquivo, "%s", string);
}

/**
 * Traduz os comando le/escreve
 *
 * ESte procedimento eh utilizado para codificar as funcoes le e escreve mapeando-as para
 *  seus equivalentes em C printf e scanf. Levando em consideracao toda a sintaxe exigida
 *  pelas funcoes em C. Para fazer a leitura na ordem correta eh necessaria a utilizacao
 *  de outra pilha para fazer a inversao dos atributos.
 *
 * @param ehLer char : Diferente de zero se o comando a ser mapeado eh o le, zero se for
 * 						escreve.
 */
void gecLeEscreve(char ehLer) {

	if(!contErros){

		if (ehLer)
			fprintf(arquivo, "\nscanf(\"");
		else
			fprintf(arquivo, "\nprintf(\"");

		auto tPilhaHash aux;
		auto tDados saida;

		criarPilha(&aux);

		/*eh necessaria a inversao dos dados da pilha para a leitura/escrita*/
		while (pop(&temporaria, &saida) == SUCESSO) {
			fprintf(arquivo, " %%");
			if(saida.tipo == INTEIRO || saida.tipo == NUMERO_INTEIRO)
				fprintf(arquivo, "d");
			else
				fprintf(arquivo, "f");

			inserirPilha(&aux, &saida);
		}

		/*escrevendo membros optativos*/
		fprintf(arquivo,"\"");
		while (pop(&aux, &saida) == SUCESSO) {
			if(ehLer)
				fprintf(arquivo,", %c%s ", '&', saida.nome);
			else
				fprintf(arquivo,", %s ", saida.nome);

			inserirPilha(&temporaria, &saida);
		}

		/*finalizando comando*/
		fprintf(arquivo,")");
		fflush(arquivo);
	}

}

/**
 * Converte real em ALG para C
 *
 * Faz a conversão de um numero real em ALG separado por virgula para um numero
 *  real em C cujo separador é o ponto.
 */
void gecConverteReal(tToken *token) {

	register unsigned int i = 0;

	if (token->signo == NUMERO_REAL)
		while (token->cadeia[i++])
			if (token->cadeia[i] == ',') {
				token->cadeia[i] = '.';
				break;
			}
}

#endif /* _GERADORCODIGO_H_ */
