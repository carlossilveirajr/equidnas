/**
 * sintatico.h
 *
 * Implementa o analisador sintatico
 *
 * Analisador sintatico do compilador Equidnas para a linguagem ALG, feito
 *  atraves de procedimentos, implementanto os grafos sintaticos da Lais.
 *  O tratamento de erro e feito atravez do modo panico.
 * Com a analisador semantico entrelacado.
 *
 * @see lexico
 * @see semantico
 * @see geradorCodigo
 *
 * @author Lais Augusta da Silva Meuchi, 298506
 * @author Carlos Roberto Silveira Junior, 298735
 *
 * @version 2009.11.19
 */

#ifndef _SINTATICO_H_
#define _SINTATICO_H_

/*includes*/
#include <stdio.h>
#include "padroes.h"
#include "lexico.h"
#include "semantico.h"
#include "geradorCodigo.h"

/*trazendo funcao de outro modulo*/
extern unsigned int getLinhaAtual(void);
extern tControle getToken(tToken*);

/*variavel global*/
unsigned int contErros; /** Conta o numero de erros*/
static unsigned int escopo = 0; /**escopo -> 0: global 1: local*/

/*prototipo de funcoes em ordem alfabetica*/
void analiseSintatica();
void mensagemErro(const char *);
tControle modoPanico(tToken*, const unsigned short int*,const unsigned short int);
tControle sintCmd(tToken*);
tControle sintComando(tToken*);
tControle sintCondicao(tToken*);
tControle sintCorpoPrograma(tToken*);
tControle sintDeclaracaoProcedimentos(tToken*);
tControle sintDeclaracaoVariaveis(tToken*);
tControle sintExpressao(tToken*);
tControle sintFator(tToken *token);
tControle sintListaArg(tToken*);
tControle sintListaParametro(tToken*);
tControle sintMaisFator(tToken *token);
tControle sintOutrosTermos(tToken*);
tControle sintPrograma(tToken*);
tControle sintTermo(tToken*);
tControle sintVariaveis(tToken*);

/*Implementacoes sem ordem*/
/**
 * @brief Escreve mensagem de erro no arquivo
 *
 * Eh responsavel por relatar o erro no arquivo.
 *
 * @param mensagem const char* : mensagem que sera escrita no arquivo
 */
void mensagemErro(const char *mensagem) {

	++contErros;
	fprintf(stdout, "Linha %05u: %s\n", getLinhaAtual(), mensagem);
}

/**
 * @brief Implementa o modo panico
 *
 * Fica em um loop consumindo tokens ate encontrar algum que seja igual a um sincronizador. Ela eh sempre
 *  chamada quando acontece um erro sintatico, ou seja, sempre que obtiver um token que nao era esperado
 *  para aquela situacao.
 *
 * @param token tToken* : token de entrada e que sera sincronizado
 * @param sincronizadores const unsigned short int* : vetor de sincronizadores
 * @param tamnho const unsigned short int : tamanho do vetor
 *
 * @return tControle : retorna um relato do erro ou FIM_ARQ_PREMATURO
 */
tControle modoPanico(tToken *token, const unsigned short int *sincronizadores,
		const unsigned short int tamanho) {

	auto signed char achouSincronizador = 0;
	register unsigned short int i;

	do {
		if (token->signo == COMENTARIO_ERRO) {

			mensagemErro("Comentario nao fechado.");
			return FIM_ARQ_PREMATURO;
		} else {
			for (i = 0; i < tamanho; ++i)
				if (token->signo == sincronizadores[i]) {
					achouSincronizador = 1;
					break;
				}
		}

		if (!achouSincronizador && getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;

	} while (!achouSincronizador);

	return ERRO;
}

/**
 * Implementa o grafo correspondente ao nao terminal expressao.
 *
 * Inicialmente checa se o token recebido eh o + ou o - se for ela obtem outro
 * token podendo retornar erro de fim de arquivo inesperado.
 * Depois chama sintFator passando o token lido: se retornar erro o modo panico
 * eh acionado. Depois eh chamado o sintMaisFator com a mesma politica da
 * funcao anterior e em seguida sintOutrosTermos.
 * O token de saida pode ser um token de sincronizacao ou o proximo token da
 * analise.
 * O retorno pode ser tanto sucesso como fim de arquivo inesperado.
 *
 * @param token tToken * : recebe o primeiro token e retorna o token apos a
 * analise.
 *
 * @return SUCESSO ou codigo de erro.
 */
tControle sintExpressao(tToken *token) {

	/*Verifica se o token capturado foi o + ou o - */
	if (token->signo == MAIS || token->signo == MENOS)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;

	auto tControle erro = sintFator(token);

	if (erro == SUCESSO)
		erro = sintMaisFator(token);
	else
		sintMaisFator(token);

	if (erro == SUCESSO)
		erro = sintOutrosTermos(token);
	else
		sintOutrosTermos(token);

	return erro;
}

/**
 * @breif Implementa o grafo outrosTermos
 *
 * Se ocorrer erro este sera propagado cima. Ja recebe o primeiro token,
 *  apenas faz a analise deste.
 *
 * @param token tToken* : recebe o primeiro token e retorna outro token
 *  apos a saida.
 *
 * @return tControle para quando ocorrer algum erro.
 */
tControle sintOutrosTermos(tToken *token) {

	auto tControle erro = SUCESSO;

	if (token->signo == MAIS || token->signo == MENOS) {

		/*codigo do gerador*/
		gecGeraCodigo(token->cadeia);

		if (getToken(token) == SUCESSO) {

			erro = sintTermo(token);

			if (erro == SUCESSO)
				return sintOutrosTermos(token);
			else {

				sintOutrosTermos(token);

				return erro;
			}

		} else
			return FIM_ARQ_PREMATURO;
	}
	else
		return erro;
}

/**
 * @brief Implementa o grafo de fator.
 *
 * Esta funcao eh composta por um switch que identificara qual dos possiveis
 *  primeiros de fator estamos analisando. Se for identificador, inteiro ou
 *  real ent�o eh getToken e retornado seu valor; se for abre parenteses eh
 *  feito um getToken e chamado sintExpressao, depois espera-se fecha parenteses
 *  caso esta sequencia esteje certa eh retornado o valor de retorno do
 *  sintExpressao, senao eh chamado o modo panico para tratar. Como esta funcao
 *  nao pode ser vazia, se o token recebido nao for um de seus primeiros eh
 *  chmado o modo panico para tratar.
 *
 * @see sintFator()
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintFator(tToken *token) {

	auto tControle erro = SUCESSO;

	switch (token->signo) {

	case IDENTIFICADOR:
		/*codigo do analisador semantico*/
		semVerDeclVar(*token);

	case NUMERO_INTEIRO:
	case NUMERO_REAL:

		/*codigo do gerador*/
		gecConverteReal(token);

		/*codigo do analisador semantico*/
		semEmpilhaTemp(*token);

		/*codigo do gerador*/
		gecGeraCodigo(token->cadeia);

		return getToken(token);

	case ABRE_PARENTESES:

		/*codigo do gerador*/
		gecGeraCodigo(token->cadeia);

		if (getToken(token) == SUCESSO)
			erro = sintExpressao(token);
		else
			return FIM_ARQ_PREMATURO;

		/*codigo do gerador*/
		gecGeraCodigo(token->cadeia);

		if (token->signo == FECHA_PARENTESES)
			if (getToken(token) == SUCESSO)
				return erro;
			else
				return FIM_ARQ_PREMATURO;
		else
			mensagemErro("Esperava-se fecha parenteses.");

		break;
	default:
		mensagemErro("Esperava-se primeiros de fator.");
	}

	/*seguidores de fator*/
	const unsigned short int sincronizadores[] = { ASTERISCO, BARRA, MAIS,
			MENOS, FIM, ABRE_PARENTESES, ENTAO };
	return modoPanico(token, sincronizadores, 7);

}

/**
 * @brief Implementa o grafo de MaisFator.
 *
 * Se for encontrado um asterisco ou barra entao busca-se outro token e a execucao
 *  prossegue a partir de sintFator e uma chamada recursiva para ele mesmo
 *
 * @see sintFator()
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintMaisFator(tToken *token) {

	if (token->signo == ASTERISCO || token->signo == BARRA) {

		/*codigo do gerador*/
		gecGeraCodigo(token->cadeia);

		if (getToken(token) == SUCESSO) {

			if (sintFator(token) == SUCESSO)
				return sintMaisFator(token);
			else {

				sintMaisFator(token);

				return ERRO;
			}
		} else
			return FIM_ARQ_PREMATURO;
	}
	else
		return SUCESSO;
}

/**
 * @brief Implementa o grafo de termo.
 *
 * Este pode ou nao comecar com '+' ou '-' depois faz uma chamada ao sintFator
 *  (grafo Fator) logo em seguida chama sintMaisFator (grafo MaisFator),
 *  se qualquer um destes der erro este sera propagado.
 *
 * @see sintFator()
 * @see sintMaisFator()
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintTermo(tToken *token) {

	/*pode ou nao comecar com '+' / '-'*/
	if (token->signo == MAIS || token->signo == MENOS) {

		/*codigo do gerador*/
		gecGeraCodigo(token->cadeia);

		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
	}

	/*se nao ocorreu erro em sintFator
	 * o valor dependera do sintMaisFator*/
	if (sintFator(token) == SUCESSO)
		return sintMaisFator(token);
	else {

		sintMaisFator(token);

		return ERRO;
	}
}

/**
 * @brief Implementa o grafo de condicao.
 *
 * Primeiramente, faz uma chamada a sintExpressao e guarda seu retorno, verifica se agora
 *  temos um condicional, se nao estamos em situacao de erro e o modo panico eh chamado.
 *  Depois chama-se sintExpressao novamente, o valor de retorno dependera das funcoes
 *  chamadas e da existencia do condicional entre as exprecoes.
 *
 * @see sintExpressao()
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintCondicao(tToken *token) {

	auto tControle erro = sintExpressao(token);

	switch (token->signo) {

	case IGUAL:
		/*codigo do gerador*/
		gecGeraCodigo(" == ");
		break;
	case DIFERENTE:
		/*codigo do gerador*/
		gecGeraCodigo(" != ");
		break;
	case MENOR_IGUAL:
	case MAIOR_IGUAL:
	case MAIOR:
	case MENOR:
		/*codigo do gerador*/
		gecGeraCodigo(token->cadeia);
		break;
	default:
		mensagemErro("Esperava-se algum condicional.");

		const unsigned short int sicronizadores[] = { MAIS, MENOS,
				IDENTIFICADOR, NUMERO_INTEIRO, NUMERO_REAL, ABRE_PARENTESES,
				FIM };
		modoPanico(token, sicronizadores, 7);

		/*jah ocorreu um erro entrou em modo panico entao o valor de
		 * sintExpressao nao tem serventia*/
		sintExpressao(token);
		return ERRO;
	}

	/*continuacao a partir de algum sinal de desigualdade*/
	if (getToken(token) != SUCESSO)
		return FIM_ARQ_PREMATURO;

	/*se obteve sucesso o valor dependera da chamada anterior
	 * caso contrario eh uma situacao de erro*/
	if (erro == ERRO)
		sintExpressao(token);
	else
		erro = sintExpressao(token);

	/*codigo do analisador semantico*/
	semApagaTemp();

	return erro;
}

/**
 * @brief Implementa o grafo comando.
 *
 * Verifica se encontrou um primeiro do grafo Cmd, caso tenha encontrado chama sintCmd
 *  e verifica o terminador dele, senao retorna SUCESSO.
 *
 * @see sintCmd()
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO)
 */
tControle sintComando(tToken *token) {

	auto tControle erro;

	switch (token->signo) {
	/*primeiros de cmd*/
	case LE:
	case ESCREVE:
	case ENQUANTO:
	case SE:
	case IDENTIFICADOR:
	case INICIO:

		erro = sintCmd(token);

		/*verifica terminador de cmd*/
		if (token->signo == PONTO_E_VIRGULA) {

			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
			else
				return sintComando(token) == SUCESSO ? erro : ERRO;
		}
		else {

			mensagemErro("Esperava-se ponto-e-virgula.");

			const unsigned short int sincronizadores[] = { LE, ESCREVE,
					ENQUANTO, SE, IDENTIFICADOR, INICIO, FIM };
			modoPanico(token, sincronizadores, 7);

			return ERRO;
		}
		/*caso nao primeiro de cmd*/
	default:
		return SUCESSO;
	}
}

/**
 * @brief Implementa o grafo listaArg.
 *
 * Verifica se encontrou uma lista de argumento como esta pode nao exitir nao configura
 *  uma situacao de erro passar pelo grafo. No entanto, se encontrar uma abre parenteses,
 *  sera busca a seguinte sequencia identificador e ponto-e-virgula/fecha parenteses.
 *  A situacao de erro eh quando temos uma abre parenteses e nao temos um fecha. Nao
 *  faz chamadas a nenhuma outra funcao do analisador sintatico exceto para tratar erros.
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintListaArg(tToken *token) {

	if (token->signo == ABRE_PARENTESES) {

		/*pode haver uma grande lista de argumentos separados por ponto-e-virgula*/
		while ((getToken(token) == SUCESSO) && (token->signo == IDENTIFICADOR)) {

			/*codigo do analisador semantico*/
			semVerDeclVar(*token);
			semEmpilhaTemp(*token);

			/*gerador de codigo*/
			gecGeraCodigo(token->cadeia);

			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
			else /*diferente de ponto-e-virgula, temos que fechar a lista*/
				if (token->signo != PONTO_E_VIRGULA)
					break;
				else
					gecAddVirgula();
		}

		/*verifica se a lista esta sendo fechada*/
		if (token->signo == FECHA_PARENTESES)
			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
			else
				;
		else { /*configuracao de erro*/

			mensagemErro("Esperava-se fecha parenteses.");

			const unsigned short int sincronizadores[] = { PONTO_E_VIRGULA };
			modoPanico(token, sincronizadores, 1);

			/*faltou falar do ponto de virgula,
			 * tambem pode estar faltando ele na
			 * gramatica*/
			return ERRO;
		}
	}
	return SUCESSO;
}

/**
 * @brief Implementa o grafo CorpoPrograma.
 *
 * Primeiramente, verifica se ha um inicio;em caso de erro ha uma sincronizacao atravez do
 *  modo panico, depois eh feita uma chamada a sintComandos. Por fim verifica-se a existencia
 *  de fim e ponto-e-virgula mostrando que um programa acabou, se n�o houver eh feita uma
 *  sincronizacao e o erro eh relatado.
 *
 * @see sintComandos()
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintCorpoPrograma(tToken *token) {

	auto tControle controle = sintDeclaracaoVariaveis(token);

	if (token->signo == INICIO)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	else {

		mensagemErro("Esperava-se inicio.");

		const unsigned short int sincronizadores[] = { LE, ESCREVE, ENQUANTO,
				SE, IDENTIFICADOR, INICIO };
		controle = modoPanico(token, sincronizadores, 6);
	}

	/*se jah houve erro este deve ser propagado*/
	if (controle == SUCESSO)
		controle = sintComando(token);
	else
		sintComando(token);

	/*obrigatorio o fim apos o fim o ponto-e-virgula*/
	if (token->signo == FIM)
		if (getToken(token) == SUCESSO)
			/*obrigatorio o ponto-e-virgula*/
			if (token->signo == PONTO_E_VIRGULA)
				if (getToken(token) == SUCESSO)
					/*se houve um erro em sintComandos*/
					return controle;
				else
					return FIM_ARQ_PREMATURO;
			else
				mensagemErro("Esperava-se ponto-e-virgula.");
		else
			return FIM_ARQ_PREMATURO;
	else
		mensagemErro("Esperava-se fim.");

	/*situacao de erro, se chegou aqui nao houve sucesso*/
	const unsigned short int sicronizadores[] = { PROCEDIMENTO, INICIO };
	return modoPanico(token, sicronizadores, 2);
}

/**
 * @brief Implementa grafo Variaveis
 *
 * Fica em um loop no qual sempre ha um identificador seguido de uma virgula
 *  se a virgula nao for encontrada, entao o loop eh quebrado e retorna-se sucesso.
 *  Se nao for encontrado um identificador apos a virgula entao apresenta-se uma
 *  configuracao de erro que sera tratado e relatado.
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintVariaveis(tToken *token) {

	auto signed char achouVirgula;

	do {
		achouVirgula = 0;

		if (token->signo == IDENTIFICADOR) {

			/*codigo do analisador semantico*/
			semEmpilhaTemp(*token);

			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
		} else {
			mensagemErro("Esperava-se identificador.");

			const unsigned short int sincronizadores[] = { DOIS_PONTOS, REAL,
					INTEIRO, FECHA_PARENTESES };
			return modoPanico(token, sincronizadores, 4);
		}

		if (token->signo == VIRGULA) {
			achouVirgula = 1;
			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
		}

	} while (token->signo == IDENTIFICADOR);

	if (!achouVirgula)
		return SUCESSO;
	else {
		mensagemErro("Esperava-se identificador.");

		const unsigned short int sincronizadores[] = { DOIS_PONTOS, REAL,
				INTEIRO };
		return modoPanico(token, sincronizadores, 3);
	}
}

/**
 * @brief Implementa grafo ListaParametro
 *
 * Inicia-se chmando sintVariaveis, depois tentar� reconhecer um dois-pontos
 *  o proximo token esperado � inteiro ou real, depois se encontrar um
 *  ponto-e-virgula faz uma chamada a sintListaParamentro, se n�o termina
 *  retornando SUCESSO, se houve algum erro no reconhecimento de algum terminal
 *  eh chamado o modo panico para tratar este erro.
 *
 * @see sintVariveis()
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintListaParametro(tToken *token) {

	auto tControle erro = sintVariaveis(token);

	if (token->signo == DOIS_PONTOS)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	else {
		mensagemErro("Esperava-se dois-pontos.");
		const unsigned short int sincronizador[] = { REAL, INTEIRO,
				PONTO_E_VIRGULA, FECHA_PARENTESES };
		erro = modoPanico(token, sincronizador, 4);
	}

	if (token->signo == REAL || token->signo == INTEIRO) {

		/*codigo do gerador*/
		gecDecArg(*token);

		/*codigo do analisador semantico*/
		semDeclareVariaveis(escopo, token->signo);

		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
	} else {
		mensagemErro("Esperava-se real ou inteiro.");
		const unsigned short int sincronizador[] = { PONTO_E_VIRGULA,
				FECHA_PARENTESES };
		erro = modoPanico(token, sincronizador, 2);
	}

	if (token->signo == PONTO_E_VIRGULA) {

		/*codigo do gerador */
		gecAddVirgula();

		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			return sintListaParametro(token);
	}
	else
		return erro;
}

/**
 * @brief Implementa grafo dc_p
 *
 * Verifica se comeca o token procedimento, se nao passara diretamente,
 *  ou seja, considera como procedimento inexistente. Depois se para a declaracao
 *  de parametros e variavaveis, por ultimo trata o corpo do programa.
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintDeclaracaoProcedimentos(tToken *token) {

	/*testa para ver se existe procedimento*/
	if (token->signo != PROCEDIMENTO)
		return SUCESSO;
	else if (getToken(token) != SUCESSO)
		return FIM_ARQ_PREMATURO;

	/*codigo do gerador*/
	gecGeraCodigo("\nvoid ");

	/*se procedimento existir a compilacao continua*/
	escopo = 1;
	auto tControle erro = SUCESSO;
	auto char nomeProcedimento[256];

	/*codigo do gerador*/
	gecGeraCodigo(token->cadeia);

	/*nome do procedimento*/
	if (token->signo == IDENTIFICADOR) {
		strcpy(nomeProcedimento, token->cadeia);
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
	} else {
		mensagemErro("Esperava-se identificador.");
		const unsigned short int sincronizador[] = { ABRE_PARENTESES,
				PONTO_E_VIRGULA };
		erro = modoPanico(token, sincronizador, 2);
	}

	/*codigo do gerador*/
	gecGeraCodigo("(");

	/*abre e fecha parenteses com argumentos*/
	if (token->signo == ABRE_PARENTESES) {
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else if (sintListaParametro(token) != SUCESSO)
			erro = ERRO;

		if (token->signo == FECHA_PARENTESES)
			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
			else
				;
		else {
			mensagemErro("Esperava-se fecha parenteses.");
			const unsigned short int sincronizador[] = { PONTO_E_VIRGULA };
			erro = modoPanico(token, sincronizador, 1);
		}
	}

	/*codigo do gerador*/
	gecGeraCodigo(") {");

	/*para o ponto e virgula*/
	if (token->signo == PONTO_E_VIRGULA)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	else {
		mensagemErro("Esperava-se ponto-e-virgula.");
		const unsigned short int
				sincronizador[] = { VAR, INICIO, PROCEDIMENTO };
		erro = modoPanico(token, sincronizador, 3);
	}

	/*codigo do analisador semantico*/
	semDeclareProcedimento(nomeProcedimento);

	if (sintCorpoPrograma(token) != SUCESSO)
		erro = ERRO;

	/*codigo do analisador semantico*/
	escopo = 0;
	semLimpaLocal();

	/*codigo do gerador*/
	gecGeraCodigo("\n}\n");

	return sintDeclaracaoProcedimentos(token) == SUCESSO ? erro : ERRO;
}

/**
 * @brief Implementa o grafo dc_v.
 *
 * Se encontrar um var entao procura-se por um identificadores seperados
 *  por virgula, podendo haver apenas um identificador, apos este fila
 *  busca-se um dois-pontos; real/inteiro e depois um terminador de ponto-e-virgula
 *  Ao terminar podem haver outros var, estn�o eh feita uma chamada recursiva e ela
 *  mesma.
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintDeclaracaoVariaveis(tToken *token) {

	auto tControle erro = SUCESSO;

	/*se encontrar um var*/
	if (token->signo == VAR)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	/*se nao encontrar var, mas encontrou identificador
	 * o programador esqueceu-se do var*/
	else if (token->signo != IDENTIFICADOR)
		return SUCESSO;
	else {
		mensagemErro("Esperava-se var.");
		erro = ERRO;
	}

	/*continua a compilacao caso nao tenha encontrado var
	 * ou identificador*/
	if (sintVariaveis(token))
		erro = ERRO;

	if (token->signo == DOIS_PONTOS)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	else {

		mensagemErro("Esperava-se dois pontos.");

		const unsigned short int sincronizador[] = { REAL, INTEIRO,
				PONTO_E_VIRGULA, FECHA_PARENTESES };
		erro = modoPanico(token, sincronizador, 4);
	}

	if (token->signo == REAL || token->signo == INTEIRO) {
		/* codigo do gerador */
		gecDecVar(*token);

		/*codigo do analisador semantico*/
		semDeclareVariaveis(escopo, token->signo);

		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
	} else {

		/* codigo do gerador */
		semApagaTemp();

		mensagemErro("Esperava-se inteiro ou real.");

		const unsigned short int sincronizador[] = { PONTO_E_VIRGULA,
				FECHA_PARENTESES, PROCEDIMENTO, INICIO };
		erro = modoPanico(token, sincronizador, 4);
	}

	if (token->signo == PONTO_E_VIRGULA)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	else {

		mensagemErro("Esperava-se ponto e virgula.");

		const unsigned short int sincronizadores[] = { VAR, PROCEDIMENTO,
				INICIO };
		erro = modoPanico(token, sincronizadores, 3);
	}

	if (erro == SUCESSO)
		return sintDeclaracaoVariaveis(token);

	sintDeclaracaoVariaveis(token);
	return erro;
}

/**
 * @breif Implementa o grafo cmd.
 *
 * Vera se o token esta entre um de seus primeiros e continuara a analise
 *  a partir desta conclusao, sendo que nao eh permitido cadeia fazia.
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintCmd(tToken *token) {

	auto tControle erro = SUCESSO;
	auto char leEscreve = 1; /*verifica se eh le ou escreve*/

	switch (token->signo) {
	/*para ler e escrever*/
	case LE:      leEscreve = !leEscreve;
	case ESCREVE: leEscreve = !leEscreve;
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;

		if (token->signo == ABRE_PARENTESES)
			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
			else
				;
		else {

			mensagemErro("Esperava-se abre parenteses.");

			const unsigned short int sincro[] = { IDENTIFICADOR,
					FECHA_PARENTESES };
			erro = modoPanico(token, sincro, 1);
		}

		if (sintVariaveis(token) != SUCESSO)
			erro = ERRO;

		/*codigo do gerador*/
		gecLeEscreve(leEscreve);
		/*gerador de codigo*/
		gecAddPontoVirgula();

		/*codigo do analisador semantico*/
		semVerLeEscreve();
		semApagaTemp();

		if (token->signo == FECHA_PARENTESES)
			if (getToken(token) == SUCESSO)
				return erro;
			else
				return FIM_ARQ_PREMATURO;
		else {
			mensagemErro("Esperava-se fecha parenteses.");
			const unsigned short int sincro[] = { PONTO_E_VIRGULA, FIM, SENAO };
			erro = modoPanico(token, sincro, 3);
		}
		break;

		/*tratamento do enquanto*/
	case ENQUANTO:
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;

		/*codigo do gerador*/
		gecGeraCodigo("\nwhile( ");

		erro = sintCondicao(token);

		/*codigo do gerador*/
		gecGeraCodigo(")");

		if (token->signo == FACA)
			if (getToken(token) == SUCESSO)
				return sintCmd(token);
			else
				return FIM_ARQ_PREMATURO;
		else {
			mensagemErro("Esperava-se faca.");
			const unsigned short int sincro[] = { LE, ESCREVE, ENQUANTO, SE,
					IDENTIFICADOR, INICIO };
			erro = modoPanico(token, sincro, 6);
		}
		sintCmd(token);

		/*gerador de codigo*/
		gecAddPontoVirgula();

		break;

	case SE:
		/*pegando proximo token para analise*/
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;

		/*codigo do gerador*/
		gecGeraCodigo("\nif( ");

		/*chamando sintCondicao*/
		if (sintCondicao(token) != SUCESSO)
			erro = sintCondicao(token);

		/*codigo do gerador*/
		gecGeraCodigo(") ");

		/*reconhecendo entao*/
		if (token->signo == ENTAO)
			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
			else
				;
		else {
			mensagemErro("Esperava-se entao.");

			const unsigned short int sincro[] = { ESCREVE, LE, ENQUANTO, SE,
					IDENTIFICADOR, INICIO };
			erro = modoPanico(token, sincro, 6);
		}

		/*chamando sintCmd*/
		if (erro == SUCESSO)
			erro = sintCmd(token);
		else
			sintCmd(token);

		/*fim enquatrar fim ou senao*/
		if (token->signo == FIM)
			if (getToken(token) == SUCESSO)
				return erro;
			else
				return FIM_ARQ_PREMATURO;
		else if (token->signo == SENAO) {

			/*gerador de codigo*/
			gecGeraCodigo("\nelse");

			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
			else if (sintCmd(token) == SUCESSO)
				return erro;
			else
				return ERRO;
		}
		else {
			mensagemErro("Esperava-se senao ou fim.");

			const unsigned short int sincro[] = { PONTO_E_VIRGULA, FIM, SENAO,
					LE, ESCREVE, ENQUANTO, SE, IDENTIFICADOR, INICIO };
			erro = modoPanico(token, sincro, 9);
		}

		break;

		/*tratamento do identificador*/
	case IDENTIFICADOR:

		/*gerador de codigo*/
		gecGeraCodigo("\n");
		gecGeraCodigo(token->cadeia);

		/*codigo do analisador semantico*/
		semEmpilhaTemp(*token);

		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;

		if (token->signo == DOIS_PONTOS_IGUAL) {

			/*gerador de codigo*/
			gecGeraCodigo(" = ");

			/*codigo do analisador semantico*/
			semDesempilhaTemp(token);
			semVerDeclVar(*token);
			semEmpilhaTemp(*token);

			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
			else
				erro = sintExpressao(token);

			/*codigo do analisador semantico*/
			semVerAtribuicao();

			/*gerador de codigo*/
			gecAddPontoVirgula();

			return erro;
		} else {

			/*gerador de codigo*/
			gecGeraCodigo("( ");

			/*codigo do analisador semantico*/
			tToken proc;
			semDesempilhaTemp(&proc);
			semVerDeclProc(proc);

			erro = sintListaArg(token);

			/*gerador de codigo*/
			gecGeraCodigo(");\n");

			/*codigo do analisador semantico*/
			return semVerParametros(proc.cadeia);

		}

		/*tratamento do inicio*/
	case INICIO:

		/*gerador de codigo*/
		gecGeraCodigo("\n{\n");

		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			erro = sintComando(token);

		if (token->signo == FIM) {

			/*gerador de codigo*/
			gecGeraCodigo("\n}\n");

			if (getToken(token) != SUCESSO)
				return FIM_ARQ_PREMATURO;
			else
				return erro;
		}
		else {
			mensagemErro("Esperava-se fim.");

			const unsigned short int sincro[] = { PONTO_E_VIRGULA, FIM, SENAO };
			erro = modoPanico(token, sincro, 3);
		}
		break;

	default:
		mensagemErro("Esperava-se primerios de CMD.");
		const unsigned short int sincro[] = { PONTO_E_VIRGULA, FIM };
		erro = modoPanico(token, sincro, 2);
	}

	return erro;
}

/**
 * @brief Implementa o grafo de programa.
 *
 * Primeiramente, verifica a existencia de um token programa, este eh obrigatorio,
 *  depois verifica um identificador e um ponto-e-virgula, todos este tokens sao
 *  obrigatorios para este etapa da analise sintatica. Somente depois eh feita uma
 *  chamada a sintDeclaracaoVariaveis e sintDeclaracaoProcedimentos. Apos esta funcoes
 *	eh verificado a existencia de inicio (comaca o bloco do programa) e depois chama
 *  sintComando, para encerrar, eh verificado a existencia da cadeia fim e ponto
 *  final, se houver algum erro este sera relatado a funcao chamadora, porem a analise nao
 *  sera interropda. Apos o ponto final o arquivo deve terminar, exceto no caso de algum
 *  comentario, mesmo assim este sera tratado pelo analisador lexico.
 *
 * @see sintDeclaracaoVariaveis()
 * @see sintDeclaracaoProcedimentos()
 * @see sintComando()
 *
 * @param token tToken* : recebe o primeiro token e retorna o proximo
 *
 * @return para caso de erro (SUCESSO ou FIM_ARQ_PREMATURO ou ERRO)
 */
tControle sintPrograma(tToken *token) {

	auto tControle erro = SUCESSO;

	/*para o token programa*/
	if (token->signo == PROGRAMA)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	else {

		mensagemErro("Esperava-se programa.");

		const unsigned short int sincro[] = { IDENTIFICADOR, PONTO_E_VIRGULA,
				VAR, PROCEDIMENTO, INICIO };
		modoPanico(token, sincro, 5);

		erro = ERRO;
	}

	/*para um identificador*/
	if (token->signo == IDENTIFICADOR)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	else {

		mensagemErro("Esperava-se identificador.");

		const unsigned short int sincro[] = { PONTO_E_VIRGULA, VAR,
				PROCEDIMENTO, INICIO };
		modoPanico(token, sincro, 4);

		erro = ERRO;
	}

	/*para o token ponto e virgula*/
	if (token->signo == PONTO_E_VIRGULA)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	else {

		mensagemErro("Esperava-se ponto e virgula.");

		const unsigned short int sincro[] = { VAR, PROCEDIMENTO, INICIO };
		modoPanico(token, sincro, 3);

		erro = ERRO;
	}

	/*funcao de declaracao de variavavel*/
	if (sintDeclaracaoVariaveis(token) != SUCESSO)
		erro = ERRO;

	/*funcao de declaracao de procedimento*/
	if (sintDeclaracaoProcedimentos(token) != SUCESSO)
		erro = ERRO;

	/*codigo do gerador*/
	gecGeraCodigo("\nint main(void) {\n");

	/*para o token inicio*/
	if (token->signo == INICIO)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	else {

		mensagemErro("Esperava-se inicio.");

		const unsigned short int sincro[] = { LE, ESCREVE, ENQUANTO, SE,
				IDENTIFICADOR, INICIO };
		erro = modoPanico(token, sincro, 6);
	}

	if (sintComando(token) != SUCESSO)
		erro = ERRO;

	/*para o token fim*/
	if (token->signo == FIM)
		if (getToken(token) != SUCESSO)
			return FIM_ARQ_PREMATURO;
		else
			;
	else {
		mensagemErro("Esperava-se fim.");

		const unsigned short int sincro[] = { PONTO_FINAL };
		erro = modoPanico(token, sincro, 1);
	}

	/*para o ponto-final*/
	if (token->signo != PONTO_FINAL) {
		mensagemErro("Esperava-se ponto-final");
		erro = ERRO;
	}

	/*codigo do gerador*/
	gecGeraCodigo("\nreturn 0;\n}");

	/*para final do arquivo*/
	if (getToken(token) != FIM_DE_ARQUIVO) {
		mensagemErro("Esperava-se fim-de-arquivo");
		erro = ERRO;
	}

	return erro;
}

/**
 * @brief faz o processo de analise sintatica
 *
 * Esta funcao eh a chamadora, ela eh a responsavel por abrir o arquivo
 *  no qual sera impressa a resposta, nesta tamb�m eh declarado o token
 *  que eh utilizado pelo programa, parecido com um token global,
 *  porem passado por referencia sempre. Fica a seu cargo imprimir as
 *  mensagens de conclusao de compilacao.
 */
void analiseSintatica(void) {

	contErros = 0;

	auto tToken token;
	auto tControle erro;

	if (getToken(&token) == SUCESSO)
		erro = sintPrograma(&token);
	else
		erro = ARQUIVO_VAZIO;

	if (contErros)
		erro = ERRO;

	switch (erro) {
	case SUCESSO:
		fprintf(stdout, "\nFim de compilacao. :-)\n");
		break;
	case FIM_ARQ_PREMATURO:
		fprintf(stdout, "\nFim de arquivo inesperado. ;-(\n");
		break;
	case ERRO:
		fprintf(stdout,	"\nPrograma escrito com erros. \nOcorreram %u erros\n\t\t Bazinga!!\n",
				contErros);
		break;
	case ARQUIVO_VAZIO:
		fprintf(stdout, "\nArquivo vazio. =D\n");
	}
}

#endif
