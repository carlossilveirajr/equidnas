programa errado;
	var a, b : inteiro;
	var c : reais; {esperava-se inteiro ou real}
	
	procedimento g(a : inteiro);
	inicio
		a := c; {variavel não declarada}
		se #b < 3 entao b := 2 + 1;	{esperava-se fim}
	fim;
inicio
	g(@); {token invalido; numero de parametros errados}
	p(b); {procediemnto nao declarado}
	le(a,c); {le pode manipular variaveis do mesmo tipo}
fim.
