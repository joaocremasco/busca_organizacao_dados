# Busca e organização de dados

## Arquivo de consulta

O módulo `query` lê uma consulta de texto linha a linha. Os nomes dos campos
devem corresponder aos cabeçalhos do CSV carregado pelo projeto. Separe cada
parte com `|`; linhas vazias e comentários iniciados por `#` são ignorados.

```text
# Campo | operador | valor
FILTER|estado|==|SP
FILTER|idade|>=|18

# Campo | valor desejado | tolerância | peso
SIM|renda|5000|1500|2
SIM|idade|30|10|1
ACCEPT|0.6

# Campo de ordenação | ASC ou DESC
ORDER|idade|DESC

# Campo principal | subcampo
GROUP|estado|cidade

# Dois campos para índice cruzado
CROSS|estado|cidade
```

`FILTER` aceita `==`, `!=`, `<`, `<=`, `>` e `>=`. Quando os dois valores
forem numéricos, a comparação é numérica; nos demais casos, é textual.
`SIM` atribui pontuação decrescente conforme a distância do valor ao alvo,
limitada a zero além da tolerância. Os pesos devem ser não negativos e pelo
menos um peso deve ser maior que zero. `ACCEPT` é opcional e vai de `0` a
`1`; sem ele, a pontuação mínima exigida é zero. `ORDER`, `GROUP` e `CROSS`
preenchem as opções de organização da coleção.
