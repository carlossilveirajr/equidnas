programa teste;
{programa de teste, utilizado para testar
 varia estrutura do analisador, esta versao
 esta correta}

	var a,b : inteiro;
	var d,e : real;

	procedimento nome(x : real);
		var a, c : inteiro;
		
	inicio
		le(a);
		escreve(c);
		se a < c + 4 entao
			inicio
				x := 3,1415;
				escreve(a);
				d := (a + 6)/ 4 + 5;
			fim
		senao
			inicio
				e := a + x;
				enquanto d < 4 faca
					le(d);
			fim;
	fim;
	
inicio
	le(b);
	se b > 0 entao {testando se, porem sem senao}
		inicio
			nome(d);
			escreve(a,b);
			enquanto d <> (8 * d) faca {testando enquanto}
			      le(b); 
		fim {cmd, nao tem ponto-e-virgula}
	fim;
fim.
{fim do programa}
