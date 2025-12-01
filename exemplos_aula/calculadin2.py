import sys
from pprint import pprint
from lexer_cldin2 import make_lexer
from parser_cldin2 import make_parser
#from sem_cldin import VerificadorSemantico
from printer_cldin2 import ImpressoraAST
#from codegen_cldin2 import GeradorCodigo

def imprimir_modo_uso():
    print("Modo de uso: python3 calculadin2.py <flag> < arquivo_entrada", file=sys.stderr)
    print("Flags:", file=sys.stderr)
    print("  -l : Executa apenas a análise léxica.", file=sys.stderr)
    print("  -p : Executa as análises léxica e sintática.", file=sys.stderr)
    print("  -pp : Executa as análises léxica e sintática e imprime e AST (se não houver erros).", file=sys.stderr)
    print("  -s : Executa as análises léxica, sintática e semântica.", file=sys.stderr)
    print("  -g : Executa o pipeline completo e gera o código.", file=sys.stderr)

def main():
    
    if len(sys.argv) != 2:
        imprimir_modo_uso()
        sys.exit(0)
        
    flag = sys.argv[1]
    
    try:
        data = sys.stdin.read()
    except Exception as e:
        print(f"Erro ao ler stdin: {e}", file=sys.stderr)
        sys.exit(0)

    # Analisador léxico
    lexer = make_lexer()
    
    # Execução para -l
    if flag == '-l':
        lexer.input(data)
        while True:
            tok = lexer.token()
            if not tok:
                break
        
        if lexer.tem_erro:
            print("ERRO: Erros léxicos encontrados.", file=sys.stderr)
            sys.exit(0)
        else:
            print("SUCESSO: Análise léxica concluída.")
        return 

    # Analisador sintático
    parser = make_parser() 
    raiz_ast = parser.parse(data, lexer=lexer)

    if parser.tem_erro or lexer.tem_erro:
        print("ERRO: Erros léxicos ou sintáticos encontrados.", file=sys.stderr)
        sys.exit(0)
    
    # Execução para -p
    if flag == '-p':
        print("SUCESSO: Análises léxica e sintática concluídas.")
        return 
    
    # Execução para -pp
    if flag == '-pp':
       print("SUCESSO: Análises léxica e sintática concluídas.")
       print("\n--- AST ---")
       impressora = ImpressoraAST()
       impressora.visita(raiz_ast)
       return

    # Analisador semântico
    #verificador = VerificadorSemantico()
    #verificador.visita(raiz_ast) 
    
    #if verificador.tem_erro:
    #    print("ERRO: Erros semânticos encontrados:", file=sys.stderr)
    #    for erro in verificador.erros:
    #        print(f"- {erro}", file=sys.stderr)
    #    sys.exit(0)
    
    # Execução para -s
    #if flag == '-s':
    #    print("SUCESSO: Análises léxica, sintática e semântica concluídas.")
    #    return 

    # Execução para -g (gera código) 
    #if flag == '-g':
    #    gerador = GeradorCodigo()
    #    gerador.visita(raiz_ast)

    #    if gerador.tem_erro:
    #        print("ERRO: Erros de geração encontrados:", file=sys.stderr)
    #        for erro in gerador.erros:
    #            print(f"- {erro}", file=sys.stderr)
    #        sys.exit(0)
    
    #    for instrucao in gerador.codigo:
    #        print(instrucao)
    #    print("SUCESSO: Geração de código concluída.", file=sys.stderr)
    #    return 

    # Flag inválida
    print(f"ERRO: Flag '{flag}' desconhecida.", file=sys.stderr)
    imprimir_modo_uso()
    sys.exit(0)

if __name__ == "__main__":
    main()