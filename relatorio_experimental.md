# Relatório Experimental — Complexidade e Desempenho

**Projeto:** Busca e Organização de Dados — SCC0202  
**Compilador:** GCC, flags `-std=c99 -Wall -Wextra -pedantic`  
**Máquina/SO:** Intel Core i5-10210U (4 núcleos/8 threads), 15 GiB RAM, Linux x86_64 (kernel 7.0.0-38-generic)  
**Datasets:** `asteroid_dataset_20251019.csv` (89.227 registros) e `games.csv` (125.855 registros)
**Data das medições:** 29/10/2026

Cada configuração foi executada três vezes. As 36 medições individuais estão em [benchmark_raw.csv](resultados/benchmark_raw.csv); as tabelas abaixo mostram as médias aritméticas.

## 1. Método de medição

O executável foi recompilado com `make -B`, usando as flags do Makefile, sem erros ou avisos. A medição usa `clock()` da biblioteca padrão C. Portanto, os valores representam tempo de CPU consumido pelo processo, em milissegundos, e não tempo decorrido de relógio. Esperas por leitura e escrita em disco não são contabilizadas como tempo de CPU; por isso, os resultados de carga e exportação não devem ser interpretados como latência total percebida.

Os tempos são separados pelo próprio programa em carga, busca, inserção dos resultados e escrita. `SEARCH_MS` exclui o tempo medido dentro de `collection_insert` para os resultados. A soma das quatro etapas é apresentada como total médio.

## 2. Complexidade das operações

Considere `n` itens, `k` itens exportados, `p` campos por item, `g` grupos, `s` subgrupos examinados no grupo correspondente, `m` itens no subgrupo, `r` e `c` chaves distintas da lista cruzada e `dᵣ` e `d꜀` os nós percorridos na linha e coluna. As estimativas abaixo consideram operações sobre ponteiros e chaves de tamanho limitado; comparações de texto também dependem do comprimento das strings.

| Estrutura | Inserção usada na coleção | Busca/percurso | Remoção pela interface da coleção |
|---|---:|---:|---:|
| Pilha (`stack`) | O(1) amortizado; O(n) ao redimensionar | O(n) para percorrer | O(1), somente no topo |
| Fila (`queue`) | O(1) | O(n) para percorrer | O(1), somente na frente |
| Lista sequencial (`array_list`) | O(1) amortizado ao acrescentar | O(n) | O(n) para localizar e deslocar |
| Lista encadeada (`linked_list`) | O(1) no fim | O(n) | O(n) para localizar; O(1) para desvincular após localizar |
| Lista com sentinela (`sentinel_list`) | O(1) no fim | O(n) | O(n) para localizar; O(1) para desvincular após localizar |
| Lista ordenada (`ordered_list`) | O(n), incluindo deslocamentos | O(log n) para localizar pela chave de ordem; O(n) para percorrer | O(n), devido ao deslocamento |
| Lista generalizada (`generalized_list`) | O(g+s) para localizar/criar grupo e subgrupo, mais O(1) para anexar | O(n) para percorrer itens | O(g+s+m) para localizar e remover; a remoção também elimina grupos que ficam vazios |
| Lista cruzada (`crossed_list`) | O(r+c+dᵣ+d꜀) para localizar chaves e posição de inserção | O(n) para percurso total; O(dᵣ) ou O(d꜀) por linha ou coluna | O(n) pela interface da coleção, que primeiro localiza o item; O(dᵣ) para remoção direta por coordenada |

Pilha e fila restringem a remoção ao topo e à frente, respectivamente. Na lista ordenada, a posição pode ser encontrada por busca binária, mas inserir ou remover exige deslocar elementos do vetor. A lista generalizada procura linearmente as chaves dos grupos e subgrupos. A lista cruzada guarda um nó por item, encadeado nas duas dimensões; a inclusão também procura ou cria as chaves de linha e coluna.

O custo do programa inclui o acesso aos campos do item: `item_get` percorre linearmente os `p` campos. Assim, para `f` filtros e `q` critérios `SIM`, a avaliação da consulta é O(n(f+q)p), sem contar os comprimentos dos textos e conversões numéricas. A gravação do campo `similarity` para cada item aceito também percorre campos existentes. A complexidade total da carga e das inserções nos resultados depende da estrutura escolhida, conforme a tabela.

Na configuração experimental, a base usa `linked_list` e os resultados usam `array_list`. A busca percorre toda a base, O(n), além do custo de avaliar os campos e inserir os aceitos. A exportação grava até `k` registros e, para cada coluna, procura seu valor no item; o custo é O(kp²) mais a escrita dos caracteres dos campos. Os tempos de inserção e busca reportados consideram a separação implementada no programa.

## 3. Protocolo

Em cada dataset, foram usados `linked_list` como base e `array_list` como estrutura de resultados. As consultas foram `queries/asteroids_benchmark.txt` e `queries/games_benchmark.txt`. Os pares limite/top-k foram (1.000, 10), (5.000, 50), (10.000, 100), (25.000, 500), (50.000, 1.000) e (0, 0). O valor zero representa toda a base e todos os resultados. Cada par foi executado três vezes.

No dataset de asteroides, o limite de 50.000 interrompe a carga antes do fim da base de 89.227 registros. Para jogos, o limite de 50.000 também interrompe a carga antes do fim da base de 125.855 registros. `INPUT_RECORDS` registra os itens efetivamente carregados; `MATCHED_RECORDS`, os itens aprovados pela consulta; e `OUTPUT_RECORDS`, os itens exportados após aplicar top-k.

## 4. Resultados

### Asteroides (`linked_list` → `array_list`)

| N carregado | k | Carga (ms) | Busca (ms) | Inserção res. (ms) | Saída (ms) | Total (ms) | Encontrados / exportados |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1.000 | 10 | 7,198 | 2,349 | 0,411 | 0,199 | 10,157 | 649 / 10 |
| 5.000 | 50 | 34,608 | 8,715 | 1,648 | 0,445 | 45,416 | 3.195 / 50 |
| 10.000 | 100 | 73,958 | 19,427 | 3,700 | 0,842 | 97,927 | 6.323 / 100 |
| 25.000 | 500 | 137,963 | 36,169 | 6,286 | 2,805 | 183,223 | 15.249 / 500 |
| 50.000 | 1.000 | 290,516 | 69,769 | 10,550 | 5,621 | 376,456 | 24.012 / 1.000 |
| 89.227 (toda) | todos | 486,981 | 116,408 | 17,240 | 176,856 | 797,486 | 39.380 / 39.380 |

### Jogos (`linked_list` → `array_list`)

| N carregado | k | Carga (ms) | Busca (ms) | Inserção res. (ms) | Saída (ms) | Total (ms) | Encontrados / exportados |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1.000 | 10 | 82,051 | 4,125 | 0,704 | 1,451 | 88,331 | 1.000 / 10 |
| 5.000 | 50 | 281,525 | 9,821 | 1,907 | 1,826 | 295,078 | 4.998 / 50 |
| 10.000 | 100 | 512,157 | 21,585 | 4,135 | 3,742 | 541,618 | 9.996 / 100 |
| 25.000 | 500 | 1.258,800 | 59,221 | 11,338 | 18,449 | 1.347,808 | 24.989 / 500 |
| 50.000 | 1.000 | 2.584,080 | 126,870 | 24,769 | 37,883 | 2.773,602 | 49.978 / 1.000 |
| 125.855 (toda) | todos | 6.526,804 | 315,030 | 60,090 | 3.951,312 | 10.853,235 | 125.811 / 125.811 |

## 5. Discussão

Os tempos de carga e busca aumentam conforme cresce o número de registros, em linha com o percurso linear da base. A carga de jogos consome mais CPU que a de asteroides, em parte pelo maior número e tamanho dos campos. A inserção dos resultados permanece abaixo da carga e da busca nesta configuração porque a lista sequencial acrescenta ao fim em tempo amortizado constante. Quando todos os itens correspondentes são exportados, a etapa de escrita aumenta com o número e o tamanho dos registros serializados.

Há variação entre execuções e entre tamanhos próximos, especialmente na carga; por isso, as tendências são mais úteis que pequenas diferenças. Como `clock()` mede tempo de CPU, estes valores não quantificam esperas de disco nem substituem uma medição de tempo decorrido. O experimento mantém as estruturas fixas (`linked_list` → `array_list`); ele avalia o crescimento com N e k, mas não compara o desempenho de todas as combinações de TADs.
