programa somaCorreto;
	var a, b : inteiro;
	var c    : real;

	procedimento soma(a, b : inteiro);
	inicio
		c := a + b;

		se c > 10 entao
			escreve(a)
		senao
			escreve(b);
	fim;

inicio
	le(a,b);
	soma(a;b);

	escreve(c);
fim.

