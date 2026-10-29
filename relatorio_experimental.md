# Relatório Experimental — Complexidade e Desempenho

**Projeto:** Busca e Organização de Dados — SCC0202  
**Compilador:** GCC, flags `-std=c99 -Wall -Wextra -pedantic`  
**Máquina/SO:** Intel Core i5-10210U (4 núcleos/8 threads), 15 GiB RAM, Linux x86_64 (kernel 7.0.0-38-generic)  
**Datasets:** `asteroid_dataset_20251019.csv` (89.227 registros) e `games.csv` (125.855 registros)

**Data das medições:** 29/10/2026

Cada configuração teve uma execução de aquecimento descartada e três execuções medidas. As 36 medições estão em [benchmark_raw.csv](resultados/benchmark_raw.csv). As tabelas mostram média e desvio padrão amostral (`s`), em milissegundos.

## 1. Método de medição

O executável foi recompilado com `make -B`, usando as flags do Makefile, sem erros ou avisos. Antes de cada configuração foi feita uma execução descartada para aquecer o processo e o acesso aos arquivos. A medição usa `clock()` da biblioteca padrão C. Portanto, os valores representam tempo de CPU consumido pelo processo, em milissegundos, e não tempo decorrido de relógio. Esperas por leitura e escrita em disco não são contabilizadas como tempo de CPU; os resultados não representam a latência total percebida.

O ambiente foi um Intel Core i5-10210U, 4 núcleos e 8 threads, Linux x86_64 com kernel 7.0.0-38-generic, e 15 GiB de RAM. No snapshot imediatamente anterior à série, havia 1 usuário conectado, carga média do sistema de 0,94 / 0,55 / 0,34 e 9,9 GiB de memória disponível. A lista de processos visível mostrava `codex` e `bash` ativos; portanto, não foi possível garantir isolamento completo nem observar processos fora do ambiente. A carga concorrente é uma possível fonte de variação, mas estes dados não permitem atribuir a ela a diferença entre séries.

O programa mede carga, busca, inserção dos resultados e escrita. Na busca, `collection_visit` avalia os critérios, grava `similarity` e guarda ponteiros dos aceitos em um vetor. A inserção dos resultados acontece depois, numa segunda passada; um único par de chamadas a `clock()` envolve o laço inteiro de inserções. Assim, `SEARCH_MS` não inclui a inserção dos resultados, e `RESULT_INSERT_MS` não soma uma medição de relógio por item. A coluna total é a soma das médias arredondadas exibidas para as quatro fases; o desvio padrão do total é calculado sobre a soma das fases em cada repetição.

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
| 1.000 | 10 | 5,699 ± 0,169 | 1,150 ± 0,069 | 0,012 ± 0,001 | 0,168 ± 0,006 | 7,029 ± 0,234 | 649 / 10 |
| 5.000 | 50 | 29,364 ± 0,602 | 6,459 ± 0,119 | 0,055 ± 0,005 | 0,429 ± 0,027 | 36,307 ± 0,679 | 3.195 / 50 |
| 10.000 | 100 | 67,925 ± 2,997 | 14,822 ± 1,804 | 0,135 ± 0,015 | 0,719 ± 0,051 | 83,601 ± 3,993 | 6.323 / 100 |
| 25.000 | 500 | 152,901 ± 8,575 | 30,986 ± 2,160 | 0,314 ± 0,060 | 3,076 ± 0,443 | 187,277 ± 10,771 | 15.249 / 500 |
| 50.000 | 1.000 | 302,871 ± 43,547 | 59,616 ± 1,928 | 0,459 ± 0,007 | 5,219 ± 0,093 | 368,165 ± 41,908 | 24.012 / 1.000 |
| 89.227 (toda) | todos | 487,674 ± 31,418 | 106,991 ± 4,590 | 0,770 ± 0,064 | 189,593 ± 12,134 | 785,028 ± 46,981 | 39.380 / 39.380 |

### Jogos (`linked_list` → `array_list`)

| N carregado | k | Carga (ms) | Busca (ms) | Inserção res. (ms) | Saída (ms) | Total (ms) | Encontrados / exportados |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1.000 | 10 | 54,808 ± 6,681 | 1,833 ± 0,099 | 0,019 ± 0,002 | 0,571 ± 0,029 | 57,231 ± 6,807 | 1.000 / 10 |
| 5.000 | 50 | 242,869 ± 16,336 | 8,035 ± 0,341 | 0,073 ± 0,004 | 1,833 ± 0,060 | 252,810 ± 16,442 | 4.998 / 50 |
| 10.000 | 100 | 521,382 ± 21,921 | 18,584 ± 2,592 | 0,189 ± 0,044 | 3,656 ± 0,214 | 543,811 ± 22,265 | 9.996 / 100 |
| 25.000 | 500 | 1.218,237 ± 98,697 | 46,107 ± 0,925 | 0,517 ± 0,037 | 19,659 ± 1,288 | 1.284,520 ± 96,737 | 24.989 / 500 |
| 50.000 | 1.000 | 2.417,263 ± 73,895 | 96,717 ± 1,906 | 0,997 ± 0,014 | 38,279 ± 1,489 | 2.553,256 ± 73,829 | 49.978 / 1.000 |
| 125.855 (toda) | todos | 6.083,020 ± 92,066 | 248,654 ± 7,381 | 2,420 ± 0,066 | 4.063,851 ± 10,327 | 10.397,945 ± 80,809 | 125.811 / 125.811 |

## 5. Discussão

Os tempos de carga e busca aumentam conforme cresce o número de registros, em linha com o percurso linear da base. A carga de jogos consome mais CPU que a de asteroides, em parte pelo maior número e tamanho dos campos. A inserção em bloco evita que o custo de chamar `clock()` para cada item domine a fase: na base completa, a média de inserção é 0,770 ms para 39.380 resultados de asteroides e 2,420 ms para 125.811 resultados de jogos. A lista sequencial acrescenta ao fim em tempo amortizado constante. Quando todos os itens correspondentes são exportados, a escrita cresce com o número e o tamanho dos registros serializados.

Nas bases completas, carga, busca, inserção e escrita representam, respectivamente, 62,12%, 13,63%, 0,10% e 24,15% do total de CPU para asteroides; para jogos, 58,50%, 2,39%, 0,02% e 39,08%. Os percentuais são calculados com as médias das fases. Há variação entre execuções, especialmente na carga: o desvio padrão da carga completa foi 31,418 ms em asteroides e 92,066 ms em jogos. Como o snapshot mostrou atividade de processos além do programa medido e a máquina não estava isolada, a comparação com séries anteriores deve ser feita com cautela. `clock()` mede tempo de CPU, não espera por disco nem tempo decorrido. O experimento mantém as estruturas fixas (`linked_list` → `array_list`); avalia o crescimento com N e k, mas não compara todas as combinações de TADs.
