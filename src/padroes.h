/**
 * padroes.h
 *
 * Encontra-se, nesta biblioteca diversas definicoes e padroes que serao
 *  utilizadas por todo o compilador. Ha tambem novos tipos definidos aqui
 *  entre outras coisas importante para a padronizacao e organizacao.
 *
 * Construcao de compiladores dois
 * Helena de Medeiros Caseli
 *
 * @author Lais Augusta da Silva Meuchi
 * @author Carlos Roberto Silveira Junior
 *
 * @version 2009.09.28
 */

/*diretivas de bibliotecas*/
#ifndef _PADROES_H_
#define _PADROES_H_

/*defines*/
/*defines globais*/
#define SUCESSO 0

/*define para o analisador lexico*/
#define ERRO_AO_ABRIR_ORIGEM 1  /*codigo de erro*/
#define TOTAL_DE_RESERVADAS 31  /*numero total da tabela de simbulos reservados*/
#define FIM_DE_ARQUIVO 2        /*quando o final do arquivo eh atingido*/
#define NOVA_LINHA 0xa          /*caracter de nova linha*/
#define TAB 0x9

#define COMENTARIO_ERRO 95
#define TOKEN_ERRO 96 /*codigo dos tokens*/
#define NUMERO_INTEIRO 97
#define NUMERO_REAL 98
#define IDENTIFICADOR 99
#define INICIO_PALAVRAS_RESERVADAS 100 /*inicio dos defines das palavras reservadas*/

/*macro com os Id das palavras e simbulos reservados*/
#define ABRE_PARENTESES 100
#define FECHA_PARENTESES 101
#define ASTERISCO 102
#define MAIS 103
#define VIRGULA 104
#define MENOS 105
#define PONTO_FINAL 106
#define BARRA 107
#define DOIS_PONTOS 108
#define DOIS_PONTOS_IGUAL 109
#define PONTO_E_VIRGULA 110
#define MENOR 111
#define MENOR_IGUAL 112
#define DIFERENTE 113
#define IGUAL 114
#define MAIOR 115
#define MAIOR_IGUAL 116
#define ENQUANTO 117
#define ENTAO 118
#define ESCREVE 119
#define FACA 120
#define FIM 121
#define INICIO 122
#define INTEIRO 123
#define LE 124
#define PROCEDIMENTO 125
#define PROGRAMA 126
#define REAL 127
#define SE 128
#define SENAO 129
#define VAR 130

/*codigos para a arvore*/
#define NO_REDUNDANTE 13

/*novos tipos*/
typedef int tControle;

typedef short int tCodigoToken;

/**
 * @brief Estrutura do token.
 *
 * Modela como sera a estrutura do token dentro deste compilador.
 */
typedef struct {
	char cadeia[256]; /**cadeia referente ao token*/
	tCodigoToken signo; /**significado desta cadeia*/
	unsigned int linha; /**linha do token*/
} tToken;

/**
 * @brief dados que serao utilizados nas estrutura
 *
 * Modela os dados inseridos na pilhaHash.
 * Apresenta os dados inseridos na pilha.
 */
typedef struct {
	char nome[256]; /**nome da variavel ou procedimento*/
	signed char tipo; /**variavel interia ou variavel real*/
	unsigned int genI; /**valor de uma variavel inteiro ou numero de parametros inteiros em um procedimento*/
	float genF; /**valor de uma variavel real ou numero de parametros reais em um procedimento*/
} tDados;

typedef unsigned long long tChave; /**tipo utilizado para chave inteiro sem sinal de 64 bits*/

/*constantes*/
const char tabelaPalavrasReservadas[TOTAL_DE_RESERVADAS][13] = { "(", ")", "*",
		"+", ",", "-", ".", "/", ":", ":=", ";", "<", "<=", "<>", "=", ">",
		">=", "enquanto", "entao", "escreve", "faca", "fim", "inicio",
		"inteiro", "le", "procedimento", "programa", "real", "se", "senao",
		"var" };

/*codigos de controle do analisador sintatico*/
#define ERRO 37
#define FIM_ARQ_PREMATURO FIM_DE_ARQUIVO
#define ARQUIVO_VAZIO 7

/*finalizador de _PADROES_H_*/
#endif
