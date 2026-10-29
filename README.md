# Busca e organização de dados

Projeto em C para carregar um arquivo CSV, consultar seus registros e organizá-los usando diferentes tipos abstratos de dados (TADs). A mesma aplicação atende à base de asteroides e à base de jogos, trocando o leitor escolhido na linha de comando.

## Funcionalidades

- Leitura de CSV com campos entre aspas e delimitadores comuns.
- Filtros, pontuação de similaridade e limiar de aceitação.
- Escolha independente da estrutura da base e da coleção de resultados.
- Exportação dos resultados em CSV com limite configurável.
- Oito estruturas implementadas pelo projeto: pilha, fila e seis tipos de lista.

## Estruturas disponíveis

Use estes nomes em `--base` e `--result`:

| Nome | Estrutura |
|---|---|
| `stack` | Pilha |
| `queue` | Fila |
| `array_list` | Lista sequencial |
| `linked_list` | Lista encadeada |
| `sentinel_list` | Lista com nó sentinela |
| `ordered_list` | Lista ordenada |
| `generalized_list` | Lista generalizada por grupos e subgrupos |
| `crossed_list` | Lista cruzada por dois campos |

As estruturas avançadas usam as opções `ORDER`, `GROUP` e `CROSS` da consulta, respectivamente.

## Compilar

Requisitos: GCC e Make.

```sh
make
```

O comando gera o executável `trabalho`, compilado em C99 com `-Wall -Wextra -pedantic`.

Para executar a suíte das estruturas:

```sh
make test
```

Para remover executável e arquivos de compilação:

```sh
make clean
```

## Executar

Exemplo com a base de asteroides incluída no repositório:

```sh
./trabalho \
  --reader nasa \
  --input asteroid_dataset_20251019.csv \
  --base linked_list \
  --result array_list \
  --query queries/asteroids_by_risk.txt \
  --output /tmp/asteroides.csv \
  --top-k 100 \
  --limit 10000
```

`--top-k 0` exporta todos os resultados e `--limit 0` carrega toda a base. Ao terminar, o programa informa as contagens e os tempos de carga, busca, inserção dos resultados e escrita.

Para usar a base de jogos, selecione `--reader games` e informe `games.csv` em `--input`. O arquivo não é versionado no Git por causa do tamanho; coloque-o localmente na raiz do projeto. O leitor separa os campos `Discount` e `DLC count`, que aparecem unidos no cabeçalho dessa base.

## Formato das consultas

Cada linha contém um comando com campos separados por `|`. Linhas vazias e comentários iniciados por `#` são ignorados.

```text
FILTER|campo|operador|valor
SIM|campo|alvo|tolerancia|peso
ACCEPT|limiar
ORDER|campo|ASC
GROUP|campo|subcampo
CROSS|campo1|campo2
```

- `FILTER` aceita `==`, `!=`, `<`, `<=`, `>` e `>=`. A comparação é numérica quando ambos os valores são números; caso contrário, é textual.
- `SIM` calcula uma pontuação entre 0 e 1 que diminui com a distância do valor ao alvo. A tolerância deve ser positiva; os pesos não podem ser negativos e não podem ser todos zero.
- `ACCEPT` define a pontuação mínima, entre 0 e 1.
- `ORDER` define o campo e a direção da lista ordenada (`ASC` ou `DESC`).
- `GROUP` define os campos da lista generalizada.
- `CROSS` define os campos da lista cruzada.

As consultas de exemplo ficam em [`queries/`](queries/).

## Relatórios

- [Relatório do projeto e experimental em PDF](docs/relatorio_latex_projeto.pdf)
- [Relatório do projeto em Markdown](docs/relatorio_projeto.md)
- [Relatório experimental em Markdown](docs/relatorio_experimental.md)
- [Fontes do relatório em LaTeX](docs/relatorio.tex)
- [Descrição do projeto](docs/descricao-projeto1.pdf)

## Organização do repositório

```text
src/                     módulos da aplicação
include/                 interfaces públicas da aplicação
data_structures/src/     implementações das estruturas
data_structures/include/ interfaces das estruturas
queries/                 consultas de exemplo e benchmark
resultados/              saídas de exemplo e medições brutas
tests/                   testes das estruturas
docs/                    relatórios, figuras e descrição do projeto
```
