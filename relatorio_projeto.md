# Relatório do Projeto 1 — Busca e Organização de Dados

**Disciplina:** SCC0202 — Algoritmos e Estruturas de Dados I  
**Integrantes:** [preencher]  
**Dataset da Parte 2:** dados de jogos (`games.csv`), aprovado pelo docente  
**Leitor:** `games`

**Ambiente dos exemplos:** Linux x86_64, Intel Core i5-10210U. `clock()` mede tempo de CPU, não tempo decorrido. Nesta auditoria, os exemplos 1 a 3 foram repetidos três vezes. Os exemplos 4 a 6 também foram repetidos três vezes; as medições individuais estão em `resultados/examples_raw.csv`.

## 1. Organização do programa

O programa é escrito em C e dividido em módulos. `src/main.c` interpreta os argumentos de linha de comando, carrega a base na coleção escolhida, percorre essa coleção aplicando a consulta, insere os itens aceitos na coleção de resultados e exporta até `top-k` registros. O executável é gerado por `make` com o nome `trabalho`; a compilação usa `-std=c99 -Wall -Wextra -pedantic`.

`src/reader.c` lê o cabeçalho e os registros CSV, detecta vírgula, ponto e vírgula ou tabulação, trata campos entre aspas e cria os itens. Para `--reader games`, corrige o cabeçalho em que `Discount` e `DLC count` aparecem unidos. `src/item.c` representa os campos de cada registro, sem fixar a estrutura ao domínio dos asteroides. `src/query.c` interpreta filtros, similaridades e opções de organização. `src/collection.c` adapta a interface comum às oito estruturas implementadas em `data_structures/`. `src/export.c` mantém a ordem e o delimitador das colunas; para jogos, grava o cabeçalho corrigido que corresponde às duas colunas de dados.

Os nomes convencionais em inglês aceitos em `--base` e `--result` são `stack`, `queue`, `array_list`, `linked_list`, `sentinel_list`, `ordered_list`, `generalized_list` e `crossed_list`. O programa também reconhece os nomes indicados no enunciado em português.

## 2. Estruturas de dados

As estruturas são implementadas pelo grupo como TADs. Pilha e fila usam arranjos/listas com operações próprias; as listas sequenciais e ordenadas armazenam ponteiros em arranjos redimensionáveis; as listas encadeadas usam nós; a lista com cabeça mantém um nó sentinela real; e a lista generalizada agrupa os itens em níveis. Na coleção cruzada, cada célula representa um par de chaves e contém uma lista dos itens que compartilham esse par.

Na aplicação, `GROUP|campo|subcampo` é usado para organizar resultados na lista generalizada. `CROSS|campo1|campo2` determina os dois índices da lista cruzada. A estrutura escolhida para a base é percorrida para a busca e os itens que passam pelos filtros e pelo limiar de similaridade são inseridos na estrutura escolhida para resultados.

## 3. Consultas e exemplos de execução

O formato das consultas é texto delimitado por `|`. `FILTER` define condições obrigatórias, `SIM` define uma pontuação numérica ponderada e `ACCEPT` estabelece o limiar. `ORDER`, `GROUP` e `CROSS` configuram as estruturas avançadas correspondentes.

### Parte 1 — Asteroides

Os exemplos usam `asteroid_dataset_20251019.csv`.

#### Exemplo 1 — filtro e similaridade

Consulta ([asteroids_by_risk.txt](queries/asteroids_by_risk.txt)):

```text
FILTER|distance_au|<=|0.08
SIM|velocity_km_s|15.0|10.0|1.0
SIM|risk_score|0.70|0.30|2.0
ACCEPT|0.60
```

Comando:

```sh
./trabalho --reader nasa --input asteroid_dataset_20251019.csv --base linked_list --result array_list --query queries/asteroids_by_risk.txt --output resultados/asteroids_by_risk.csv --top-k 100 --limit 10000
```

Resultado: `STATUS=OK`. 10.000 lidos. 93 encontrados e 93 exportados. Médias de três execuções: carga 91.794 ms; busca 13.293 ms; inserção de resultados 0.010 ms; escrita 0.853 ms. Os valores individuais estão em [examples_raw.csv](resultados/examples_raw.csv).

#### Exemplo 2 — resultados ordenados

Consulta ([asteroids_ordered.txt](queries/asteroids_ordered.txt)): filtra `distance_au <= 0.08` e ordena por `risk_score` decrescente.

```sh
./trabalho --reader nasa --input asteroid_dataset_20251019.csv --base queue --result ordered_list --query queries/asteroids_ordered.txt --output resultados/asteroids_ordered.csv --top-k 50 --limit 10000
```

Resultado: `STATUS=OK`. 10.000 lidos. 5.501 encontrados e 50 exportados. Médias de três execuções: carga 74.342 ms; busca 15.694 ms; inserção de resultados 85.696 ms; escrita 0.490 ms.

#### Exemplo 3 — agrupamento e índice cruzado

Consulta ([asteroids_group_cross.txt](queries/asteroids_group_cross.txt)): filtra `distance_au <= 0.1`, agrupa por `threat_category` e `year` e define esses campos também no índice cruzado. A base usa lista generalizada e o resultado usa lista cruzada.

```sh
./trabalho --reader nasa --input asteroid_dataset_20251019.csv --base generalized_list --result crossed_list --query queries/asteroids_group_cross.txt --output resultados/asteroids_group_cross.csv --top-k 100 --limit 20000
```

Resultado: `STATUS=OK`. 20.000 lidos. 12.919 encontrados e 100 exportados. Médias de três execuções: carga 137.972 ms; busca 29.708 ms; inserção de resultados 6.710 ms; escrita 0.845 ms.

### Parte 2 — Dados de jogos

O grupo reutiliza o mesmo programa e os mesmos TADs, trocando o leitor para `games`. A consulta [games_by_genre.txt](queries/games_by_genre.txt) filtra jogos Windows com preço entre 1 e 100, calcula similaridade em `Peak CCU` e organiza por gênero e idade mínima. A consulta [popular_games_by_genre.txt](queries/popular_games_by_genre.txt) seleciona jogos Windows com `Peak CCU` a partir de 1000 e configura o índice cruzado por gênero e Windows.

#### Exemplo 4 — agrupamento por gênero e idade

```sh
./trabalho --reader games --input games.csv --base linked_list --result generalized_list --query queries/games_by_genre.txt --output resultados/games_by_genre.csv --top-k 100 --limit 10000
```

Resultado: `STATUS=OK`. 10.000 lidos. 6.145 encontrados e 100 exportados. Médias de três execuções: carga 512.497 ms; busca 21.991 ms; inserção de resultados 15.895 ms; escrita 4.561 ms.

#### Exemplo 5 — índice cruzado

```sh
./trabalho --reader games --input games.csv --base array_list --result crossed_list --query queries/popular_games_by_genre.txt --output resultados/popular_games_by_genre.csv --top-k 100 --limit 10000
```

Resultado: `STATUS=OK`. 10.000 lidos. 43 encontrados e 43 exportados. Médias de três execuções: carga 524.614 ms; busca 4.484 ms; inserção de resultados 0.065 ms; escrita 2.738 ms.

#### Exemplo 6 — segunda combinação de TADs

Consulta ([popular_games_ordered.txt](queries/popular_games_ordered.txt)): filtra jogos Windows com `Peak CCU >= 1000` e ordena por `Peak CCU` decrescente.

```sh
./trabalho --reader games --input games.csv --base array_list --result ordered_list --query queries/popular_games_ordered.txt --output resultados/popular_games_ordered.csv --top-k 100 --limit 10000
```

Resultado: `STATUS=OK`. 10.000 lidos. 43 encontrados e 43 exportados. Médias de três execuções: carga 494.867 ms; busca 7.803 ms; inserção de resultados 0.100 ms; escrita 2.653 ms.

Os CSVs de asteroides gerados mantêm o cabeçalho original. Em jogos, o arquivo fonte une `Discount` e `DLC count` no cabeçalho apesar de conter duas colunas de dados; o leitor separa os nomes, e a exportação escreve o cabeçalho corrigido com 40 colunas. Ordem e delimitador são mantidos.

## 4. Memória e saída

Os registros são mantidos em um armazenamento compartilhado durante a execução; as coleções guardam referências aos itens. A destruição das coleções libera os nós e índices, e o armazenamento dos itens é liberado ao final. A exportação respeita `top-k` (zero significa todos) e preserva a ordem e o delimitador. O cabeçalho de jogos é normalizado para separar `Discount` e `DLC count`, pois a origem os une embora os registros tenham duas colunas.

