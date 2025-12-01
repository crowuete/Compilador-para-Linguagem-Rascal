from __future__ import annotations
from typing import List, Dict
import ast_cldin2 as ast
from defs_cldin2 import Visitador, Simbolo, Categoria, Tipo, TIPO_REAL, TIPO_BOOL

# Tabela de Símbolos
class TabelaSimbolos:
    def __init__(self) -> None:
        self.escopos: List[Dict[str, Simbolo]] = [dict()]
        self.deslocamento_atual: int = 0

    def abre_escopo(self):
        self.escopos.append({})

    def fecha_escopo(self):
        if len(self.escopos) > 1:
            self.escopos.pop()

    def instala(self, s: Simbolo) -> str|None:
        atual = self.escopos[-1]
        if s.nome in atual:
            return f"Identificador '{s.nome}' já declarado neste escopo"
        s.deslocamento = self.deslocamento_atual
        atual[s.nome] = s
        self.deslocamento_atual += 1
        return None

    def busca(self, nome: str) -> Simbolo|None:
        for tabela in reversed(self.escopos):
            if nome in tabela:
                return tabela[nome]
        return None
    
    @property
    def total_vars_alocadas(self) -> int:
        return self.deslocamento_atual

class VerificadorSemantico(Visitador):
    def __init__(self, saida=sys.stdout):
        self.saida = saida

    def visita_Programa(self, no: ast.Programa):
        self.visita(no.bloco_cmds)

    def visita_BlocoCmds(self, no: ast.BlocoCmds):
        for cmd in no.lista_cmds:
            self.visita(cmd)

    def visita_Declaracao(self, no: ast.Declaracao):
        self.visita(no.id)
        self.imprime(f" : {no.nome_tipo})")

    def visita_Condicional(self, no: ast.Condicional):
        self.imprime("(If ")
        self.visita(no.condicao)
        self.imprime(" ")
        self.visita(no.bloco_then)
        if no.bloco_else:
            self.imprime(" ")
            self.visita(no.bloco_else)
        self.imprime(")")

    def visita_Funcao(self, no: ast.Funcao):
        self.imprime(f"({no.nome_funcao.upper()} ")
        self.visita(no.argumento)
        self.imprime(")")

    def visita_Atribuicao(self, no: ast.Atribuicao):
        self.imprime("(Atrib ")
        self.visita(no.id)
        self.imprime(" ")
        self.visita(no.calculo)
        self.imprime(")")

    def visita_CalculoBinario(self, no: ast.CalculoBinario):
        self.imprime("(CalcBin ")
        self.visita(no.esq)
        self.imprime(f" {no.op} ")
        self.visita(no.dir)
        self.imprime(")")

    def visita_CalculoUnario(self, no: ast.CalculoUnario):
        self.imprime(f"(CalcUn {no.op} ")
        self.visita(no.calculo)
        self.imprime(")")

    def visita_CalcId(self, no: ast.CalcId):
        self.imprime(f"(Id {no.nome})")

    def visita_CalcConstNum(self, no: ast.CalcConstNum):
        self.imprime(f"(ConstNum {repr(no.valor)})")

    def visita_CalcConstBool(self, no: ast.CalcConstBool):
        self.imprime(f"(ConstBool {repr(no.valor)})")