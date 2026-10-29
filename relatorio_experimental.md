# Relatório Experimental — Complexidade e Desempenho

**Projeto:** Busca e Organização de Dados — SCC0202  
**Compilador:** GCC, flags `-std=c99 -Wall -Wextra -pedantic`  
**Máquina/SO:** Intel Core i5-10210U (4 núcleos/8 threads), 15 GiB RAM, Linux x86_64 (kernel 7.0.0-38-generic)  
**Datasets:** `asteroid_dataset_20251019.csv` (89.227 registros) e `games.csv` (125.855 registros)

**Data das medições:** 29/10/2026

Cada configuração teve uma execução de aquecimento descartada e três execuções medidas. As 36 medições estão em [benchmark_raw.csv](resultados/benchmark_raw.csv). As tabelas mostram média e desvio padrão amostral (`s`), em milissegundos.

## 1. Método de medição

O executável foi recompilado com `make -B`, usando as flags do Makefile, sem erros ou avisos. Antes de cada configuração foi feita uma execução descartada para aquecer o processo e o acesso aos arquivos. A medição usa `clock()` da biblioteca padrão C. Portanto, os valores representam tempo de CPU consumido pelo processo, em milissegundos, e não tempo decorrido de relógio. Esperas por leitura e escrita em disco não são contabilizadas como tempo de CPU; os resultados não representam a latência total percebida.

O ambiente foi um Intel Core i5-10210U, 4 núcleos e 8 threads, Linux x86_64 com kernel 7.0.0-38-generic e 15 GiB de RAM. A máquina não estava isolada. Não foi registrado um snapshot de carga do sistema imediatamente antes desta série, então não dá para separar a variação do programa da atividade concorrente.

O programa mede carga, busca, inserção dos resultados e escrita. Na busca, `collection_visit` avalia os critérios, grava `similarity` e guarda ponteiros dos aceitos em um vetor. A inserção dos resultados acontece depois, numa segunda passada; um único par de chamadas a `clock()` envolve o laço inteiro de inserções. Assim, `SEARCH_MS` não inclui a inserção dos resultados, e `RESULT_INSERT_MS` não soma uma medição de relógio por item. A coluna total é a soma das médias arredondadas exibidas para as quatro fases; o desvio padrão do total é calculado sobre a soma das fases em cada repetição.

## 2. Complexidade das operações

Considere `n` itens, `k` itens exportados, `p` campos por item, `g` grupos, `s` subgrupos examinados no grupo correspondente, `m` itens no subgrupo, `r` e `c` chaves distintas da lista cruzada, `h` itens no bucket de uma célula e `dᵣ` e `d꜀` os nós percorridos na linha e coluna. As estimativas abaixo consideram operações sobre ponteiros e chaves de tamanho limitado; comparações de texto também dependem do comprimento das strings.

| Estrutura | Inserção usada na coleção | Busca/percurso | Remoção pela interface da coleção |
|---|---:|---:|---:|
| Pilha (`stack`) | O(1) amortizado; O(n) ao redimensionar | O(n) para percorrer | O(1), somente no topo |
| Fila (`queue`) | O(1) | O(n) para percorrer | O(1), somente na frente |
| Lista sequencial (`array_list`) | O(1) amortizado ao acrescentar | O(n) | O(n) para localizar e deslocar |
| Lista encadeada (`linked_list`) | O(1) no fim | O(n) | O(n) para localizar; O(1) para desvincular após localizar |
| Lista com sentinela (`sentinel_list`) | O(1) no fim | O(n) | O(n) para localizar; O(1) para desvincular após localizar |
| Lista ordenada (`ordered_list`) | O(n), incluindo deslocamentos | O(log n) para localizar pela chave de ordem; O(n) para percorrer | O(n), devido ao deslocamento |
| Lista generalizada (`generalized_list`) | O(g+s) para localizar/criar grupo e subgrupo, mais O(1) para anexar | O(n) para percorrer itens | O(g+s+m) para localizar e remover; a remoção também elimina grupos que ficam vazios |
| Lista cruzada (`crossed_list`) | O(r+c+dᵣ+d꜀) para localizar chaves e uma célula nova; O(r+c) para anexar a célula existente | O(n) para percurso total; O(dᵣ) ou O(d꜀) por linha ou coluna | O(r+c+h) para localizar as chaves e o item no bucket |

Pilha e fila restringem a remoção ao topo e à frente, respectivamente. Na lista ordenada, a posição pode ser encontrada por busca binária, mas inserir ou remover exige deslocar elementos do vetor. A lista generalizada procura linearmente as chaves dos grupos e subgrupos. A lista cruzada guarda um nó por par de chaves, encadeado nas duas dimensões; na coleção, esse nó aponta para um bucket com os itens que compartilham o par.

O custo do programa inclui o acesso aos campos do item: `item_get` percorre linearmente os `p` campos. Assim, para `f` filtros e `q` critérios `SIM`, a avaliação da consulta é O(n(f+q)p), sem contar os comprimentos dos textos e conversões numéricas. A gravação do campo `similarity` para cada item aceito também percorre campos existentes. A complexidade total da carga e das inserções nos resultados depende da estrutura escolhida, conforme a tabela.

Na configuração experimental, a base usa `linked_list` e os resultados usam `array_list`. A busca percorre toda a base, O(n), além do custo de avaliar os campos e inserir os aceitos. A exportação grava até `k` registros e, para cada coluna, procura seu valor no item; o custo é O(kp²) mais a escrita dos caracteres dos campos. Os tempos de inserção e busca reportados consideram a separação implementada no programa.

## 3. Protocolo

Em cada dataset, foram usados `linked_list` como base e `array_list` como estrutura de resultados. As consultas foram `queries/asteroids_benchmark.txt` e `queries/games_benchmark.txt`. Os pares limite/top-k foram (1.000, 10), (5.000, 50), (10.000, 100), (25.000, 500), (50.000, 1.000) e (0, 0). O valor zero representa toda a base e todos os resultados. Cada par foi executado três vezes.

No dataset de asteroides, o limite de 50.000 interrompe a carga antes do fim da base de 89.227 registros. Para jogos, o limite de 50.000 também interrompe a carga antes do fim da base de 125.855 registros. `INPUT_RECORDS` registra os itens efetivamente carregados; `MATCHED_RECORDS`, os itens aprovados pela consulta; e `OUTPUT_RECORDS`, os itens exportados após aplicar top-k.

## 4. Resultados

### Asteroides (`linked_list` → `array_list`)

| N carregado | k | Carga (ms) | Busca (ms) | Inserção res. (ms) | Saída (ms) | Total (ms) | Encontrados / exportados |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1.000 | 10 | 12,150 ± 0,148 | 3,075 ± 0,816 | 0,028 ± 0,001 | 0,376 ± 0,071 | 15,629 ± 0,785 | 649 / 10 |
| 5.000 | 50 | 28,606 ± 0,854 | 6,070 ± 0,123 | 0,053 ± 0,001 | 0,420 ± 0,003 | 35,149 ± 0,965 | 3.195 / 50 |
| 10.000 | 100 | 56,270 ± 3,523 | 11,929 ± 0,552 | 0,112 ± 0,002 | 0,661 ± 0,049 | 68,972 ± 3,846 | 6.323 / 100 |
| 25.000 | 500 | 155,548 ± 5,775 | 30,834 ± 2,016 | 0,306 ± 0,006 | 3,269 ± 0,833 | 189,957 ± 6,804 | 15.249 / 500 |
| 50.000 | 1.000 | 332,364 ± 49,712 | 69,944 ± 7,257 | 0,590 ± 0,057 | 6,951 ± 0,442 | 409,849 ± 57,150 | 24.012 / 1.000 |
| 89.227 (toda) | todos | 434,041 ± 12,547 | 103,079 ± 8,536 | 0,839 ± 0,129 | 191,707 ± 18,858 | 729,666 ± 12,017 | 39.380 / 39.380 |

### Jogos (`linked_list` → `array_list`)

| N carregado | k | Carga (ms) | Busca (ms) | Inserção res. (ms) | Saída (ms) | Total (ms) | Encontrados / exportados |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1.000 | 10 | 62,656 ± 3,973 | 2,507 ± 1,323 | 0,019 ± 0,003 | 0,603 ± 0,052 | 65,785 ± 5,257 | 1.000 / 10 |
| 5.000 | 50 | 245,154 ± 2,946 | 7,924 ± 0,222 | 0,080 ± 0,009 | 2,010 ± 0,192 | 255,168 ± 3,007 | 4.998 / 50 |
| 10.000 | 100 | 490,325 ± 35,630 | 19,631 ± 0,230 | 0,180 ± 0,006 | 4,009 ± 0,138 | 514,145 ± 35,589 | 9.996 / 100 |
| 25.000 | 500 | 1.201,504 ± 59,925 | 43,898 ± 0,777 | 0,524 ± 0,020 | 19,399 ± 1,924 | 1.265,325 ± 59,430 | 24.989 / 500 |
| 50.000 | 1.000 | 2.402,729 ± 55,795 | 98,035 ± 7,579 | 1,086 ± 0,170 | 38,935 ± 2,185 | 2.540,785 ± 49,296 | 49.978 / 1.000 |
| 125.855 (toda) | todos | 6.193,084 ± 105,172 | 246,937 ± 14,470 | 2,433 ± 0,087 | 4.342,640 ± 78,843 | 10.785,094 ± 85,417 | 125.811 / 125.811 |

## 5. Discussão

Os tempos de carga e busca aumentam com o número de registros, conforme o percurso linear da base. Na base completa, carga, busca, inserção e escrita representam 59,48%, 14,13%, 0,11% e 26,27% para asteroides, e 57,42%, 2,29%, 0,02% e 40,27% para jogos. A inserção em bloco custa 0,839 ms para 39.380 itens aceitos em asteroides e 2,433 ms para 125.811 em jogos.

A carga completa teve desvio padrão de 12,547 ms em asteroides e 105,172 ms em jogos. Para asteroides com N=50.000, uma amostra de carga foi 275,523 ms, enquanto as outras foram 353,849 e 367,719 ms; as três foram mantidas. As amostras são poucas e não permitem decidir se há outlier nem atribuir a diferença à máquina. `clock()` mede tempo de CPU, não tempo decorrido. O experimento mantém `linked_list` como base e `array_list` como resultado, e foi feito numa só máquina.
