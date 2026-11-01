# Relatório Experimental — Complexidade e Desempenho

**Projeto:** Busca e Organização de Dados — SCC0202
**Compilador:** GCC, flags `-std=c99 -Wall -Wextra -pedantic`
**Máquina/SO:** Intel Core i5-10210U (4 núcleos/8 threads), 15 GiB RAM, Linux x86_64; máquina não isolada; frequência observada em 57% do máximo nominal (kernel 7.0.0-42-generic)
**Datasets:** `asteroid_dataset_20251019.csv` (89.227 registros) e `games.csv` (125.855 registros)

**Data das medições:** 01/11/2026

Cada configuração teve uma execução de aquecimento descartada e três execuções medidas. As 36 medições estão em [benchmark_raw.csv](../resultados/benchmark_raw.csv). As tabelas mostram média e desvio padrão amostral (`s`), em milissegundos.

## 1. Método de medição

O executável foi recompilado com `make -B`, usando as flags do Makefile, sem erros ou avisos. Antes de cada configuração foi feita uma execução descartada para aquecer o processo e o acesso aos arquivos. A medição usa `clock()` da biblioteca padrão C. Portanto, os valores representam tempo de CPU consumido pelo processo, em milissegundos, e não tempo decorrido de relógio. Esperas por leitura e escrita em disco não são contabilizadas como tempo de CPU; os resultados não representam a latência total percebida.

O ambiente foi um Intel Core i5-10210U, 4 núcleos e 8 threads, Linux x86_64 com kernel 7.0.0-42-generic e 15 GiB de RAM. A máquina não estava isolada. No momento da coleta, o load average era 6,64, 6,26 e 6,00; a frequência reportada estava em 57% do máximo nominal. Essa observação pontual não permite separar a variação do programa da atividade concorrente ao longo de toda a série.

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
| 1.000 | 10 | 9,915 ± 1,144 | 2,488 ± 0,330 | 0,024 ± 0,001 | 0,337 ± 0,094 | 12,764 ± 1,102 | 649 / 10 |
| 5.000 | 50 | 45,734 ± 1,525 | 10,906 ± 1,447 | 0,093 ± 0,024 | 0,696 ± 0,028 | 57,429 ± 1,598 | 3.195 / 50 |
| 10.000 | 100 | 93,834 ± 2,991 | 21,003 ± 2,083 | 0,225 ± 0,093 | 1,128 ± 0,108 | 116,190 ± 3,057 | 6.323 / 100 |
| 25.000 | 500 | 259,943 ± 10,395 | 58,729 ± 3,694 | 0,507 ± 0,090 | 4,625 ± 0,597 | 323,805 ± 9,731 | 15.249 / 500 |
| 50.000 | 1.000 | 510,761 ± 8,330 | 101,930 ± 9,109 | 0,746 ± 0,247 | 8,911 ± 1,121 | 622,348 ± 13,754 | 24.012 / 1.000 |
| 89.227 (toda) | todos | 970,935 ± 13,388 | 187,741 ± 15,744 | 1,100 ± 0,166 | 332,816 ± 18,368 | 1.492,593 ± 36,418 | 39.380 / 39.380 |

### Jogos (`linked_list` → `array_list`)

| N carregado | k | Carga (ms) | Busca (ms) | Inserção res. (ms) | Saída (ms) | Total (ms) | Encontrados / exportados |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1.000 | 10 | 93,164 ± 6,064 | 3,582 ± 0,335 | 0,037 ± 0,003 | 0,817 ± 0,202 | 97,601 ± 6,179 | 1.000 / 10 |
| 5.000 | 50 | 449,742 ± 26,568 | 16,095 ± 3,451 | 0,140 ± 0,011 | 3,270 ± 0,857 | 469,247 ± 29,737 | 4.998 / 50 |
| 10.000 | 100 | 879,470 ± 19,230 | 30,877 ± 1,333 | 0,297 ± 0,057 | 6,746 ± 0,558 | 917,390 ± 19,934 | 9.996 / 100 |
| 25.000 | 500 | 2.218,394 ± 18,373 | 78,598 ± 7,154 | 0,818 ± 0,096 | 30,340 ± 3,386 | 2.328,149 ± 25,470 | 24.989 / 500 |
| 50.000 | 1.000 | 4.602,054 ± 165,089 | 184,245 ± 7,737 | 1,835 ± 0,098 | 65,157 ± 6,400 | 4.853,291 ± 156,493 | 49.978 / 1.000 |
| 125.855 (toda) | todos | 11.657,033 ± 123,182 | 413,325 ± 45,218 | 3,807 ± 0,397 | 7.962,563 ± 317,230 | 20.036,727 ± 474,308 | 125.811 / 125.811 |

## 5. Discussão

Na base completa, carga, busca, inserção e escrita representam 65,05%, 12,58%, 0,07% e 22,30% para asteroides e 58,18%, 2,06%, 0,02% e 39,74% para jogos. A carga é a fase dominante. A inserção em bloco levou 1,100 ms para 39.380 itens aceitos em asteroides e 3,807 ms para 125.811 itens em jogos.

O load average observado durante a coleta ficou entre 6,0 e 6,6 em uma máquina com 8 CPUs lógicas; a frequência reportada estava em 57% do máximo nominal. A máquina não estava isolada, então a carga concorrente pode ter influenciado os tempos. `clock()` mede tempo de CPU, não tempo decorrido.
