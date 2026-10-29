# Relatório Experimental — Complexidade e Desempenho

**Projeto:** Busca e Organização de Dados — SCC0202  
**Compilador:** GCC, flags `-std=c99 -Wall -Wextra -pedantic`  
**Máquina/SO:** Intel Core i5-10210U (4 núcleos/8 threads), 15 GiB RAM, Linux x86_64 (kernel 7.0.0-38-generic)  
**Datasets:** `asteroid_dataset_20251019.csv` (39.380 registros) e `games.csv` (125.811 registros)  
**Data dos ensaios:** 28/10/2026

Cada valor é a média aritmética de três execuções. O arquivo [benchmark_raw.csv](resultados/benchmark_raw.csv) contém as 36 medições individuais.

## 1. Notação

`n` é o número de itens armazenados; `k` é o número de resultados exportados; `g` é o número de grupos distintos; `s` é o número de subgrupos examinados dentro do grupo; `r` e `c` são as quantidades de chaves distintas dos índices da lista cruzada; `dᵣ` e `d꜀` são os graus da linha e da coluna percorridos. As complexidades de busca consideram busca por valor/identidade quando aplicável. Para o programa completo, a busca da consulta percorre todos os `n` registros, independentemente da estrutura-base.

## 2. Complexidade das operações

| Estrutura (TAD) | Inserção usada pelo programa | Busca/percurso | Remoção | Observações |
|---|---:|---:|---:|---|
| Pilha (`stack`) | O(1) amortizado; O(n) quando redimensiona | O(n) para busca/percurso | O(1) no topo; localizar item arbitrário O(n) | Inserção e remoção seguem LIFO. |
| Fila (`queue`) | O(1) | O(n) | O(1) na frente; localizar item arbitrário O(n) | FIFO; percurso visita os n itens. |
| Lista sequencial (`array_list`) | O(1) amortizado para acrescentar | O(n) | O(n), pois busca e desloca elementos | Arranjo contíguo redimensionável. |
| Lista encadeada (`linked_list`) | O(1) no fim (mantém ponteiro final) | O(n) | O(n) para localizar; O(1) para religar nós após localizar | Nós alocados individualmente. |
| Lista com cabeça (`sentinel_list`) | O(1) no fim | O(n) | O(n) para localizar; O(1) para religar após localizar | O sentinela não representa item. |
| Lista ordenada (`ordered_list`) | O(n) | O(log n) para localizar por chave de ordenação; O(n) para percorrer | O(n), incluindo busca binária e deslocamento | A posição é localizada por busca binária; o arranjo precisa deslocar elementos. |
| Lista generalizada (`generalized_list`) | O(g+s) para procurar/criar grupo e subgrupo; O(1) para anexar item ao subgrupo | O(n) para percorrer todos os itens | O(g+s+m), no pior caso, para localizar grupo/subgrupo e item | `m` é o número de itens no subgrupo. O custo cresce com a busca linear das chaves. |
| Lista cruzada (`crossed_list`) | O(dᵣ+d꜀), além da busca linear de novas chaves (O(r+c)) | O(n) para percurso total; O(dᵣ) ou O(d꜀) por linha/coluna | O(dᵣ) para localizar coordenada e O(1) para desvincular | Cada registro tem um nó ligado simultaneamente às listas da linha e coluna. |

O custo de carregar `n` itens depende da estrutura. Para pilha, fila, listas sequenciais/encadeadas/com cabeça, a inserção total é O(n) amortizado. Na lista ordenada, inserir n itens pode custar O(n²), devido aos deslocamentos. Na generalizada, depende da quantidade e distribuição de chaves; no pior caso, a procura linear de grupos/subgrupos também pode acumular custo quadrático. Na lista cruzada, depende dos graus das linhas/colunas e da quantidade de chaves distintas.

A busca do programa testa os filtros e similaridades em cada item. Para `f` filtros e `q` critérios `SIM`, o custo é O(n(f+q)), mais as inserções dos itens aceitos na estrutura de resultados. A exportação percorre e escreve até `k` itens, com custo O(k) mais o custo de serialização dos campos. Os tempos reportados separam carga, busca, inserção dos resultados e escrita; `SEARCH_MS` exclui `RESULT_INSERT_MS`.

## 3. Protocolo experimental

1. Usar a compilação do Makefile com `-std=c99 -Wall -Wextra -pedantic`.
2. Em cada base, executar seis pares de `--limit` e `--top-k`: (1.000, 10), (5.000, 50), (10.000, 100), (25.000, 500), (50.000, 1.000) e (0, 0), em que zero indica toda a base e todos os resultados. Como a base de asteroides tem 39.380 linhas, o limite de 50.000 chega ao fim do arquivo; `INPUT_RECORDS` no CSV bruto registra esse total efetivo.
3. Usar `linked_list` como base, `array_list` como estrutura de resultados e as consultas `queries/asteroids_benchmark.txt` e `queries/games_benchmark.txt`.
4. Repetir cada par três vezes e calcular a média aritmética de `LOAD_MS`, `SEARCH_MS`, `RESULT_INSERT_MS` e `WRITE_MS`. O arquivo bruto mantém as contagens e os tempos de cada execução.
5. Conferir se as contagens de entrada, correspondências e saída batem com os limites configurados.

## 4. Tabelas de resultados

### Asteroides (`linked_list` → `array_list`)

| N | k | Carga (ms) | Busca (ms) | Inserção res. (ms) | Saída (ms) | Total (ms) | Encontrados / exportados |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1.000 | 10 | 6,645 | 1,430 | 0,048 | 0,161 | 8,284 | 649 / 10 |
| 5.000 | 50 | 60,285 | 15,432 | 0,459 | 0,883 | 77,060 | 3.195 / 50 |
| 10.000 | 100 | 56,180 | 11,945 | 0,326 | 0,575 | 69,026 | 6.323 / 100 |
| 25.000 | 500 | 132,594 | 31,900 | 0,824 | 2,819 | 168,138 | 15.249 / 500 |
| 39.380 (limite 50.000) | 1.000 | 239,031 | 59,366 | 1,276 | 5,997 | 305,670 | 24.012 / 1.000 |
| 39.380 (toda) | todos | 375,453 | 97,905 | 1,961 | 168,871 | 644,189 | 39.380 / 39.380 |

### Jogos (`linked_list` → `array_list`)

| N | k | Carga (ms) | Busca (ms) | Inserção res. (ms) | Saída (ms) | Total (ms) | Encontrados / exportados |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1.000 | 10 | 34,886 | 1,487 | 0,043 | 0,401 | 36,818 | 1.000 / 10 |
| 5.000 | 50 | 190,600 | 9,775 | 0,253 | 1,785 | 202,413 | 4.998 / 50 |
| 10.000 | 100 | 353,417 | 17,236 | 0,494 | 3,520 | 374,667 | 9.996 / 100 |
| 25.000 | 500 | 855,705 | 44,357 | 1,329 | 17,509 | 918,900 | 24.989 / 500 |
| 50.000 | 1.000 | 1.679,419 | 100,590 | 2,913 | 37,032 | 1.819,955 | 49.978 / 1.000 |
| 125.811 (toda) | todos | 4.331,173 | 251,573 | 7,047 | 3.431,884 | 8.021,677 | 125.811 / 125.811 |


## 5. Discussão

O tempo de carga cresce com `n`; a base de jogos demora mais por registro, em parte pelo volume e tamanho dos campos CSV. A busca também aumenta com `n`, como esperado pelo percurso completo da coleção. A inserção de resultados fica pequena nesta configuração, pois a lista sequencial acrescenta ao fim em tempo amortizado constante. Para a base completa de jogos, a escrita leva mais tempo porque `top-k=0` exporta todos os 125.811 registros.

Há variação entre tamanhos próximos — por exemplo, 5.000 e 10.000 asteroides — compatível com cache de disco e ruído do sistema. A comparação deve considerar a tendência geral, sem atribuir significado a diferenças pequenas. Os ensaios de tempo mantiveram base e resultado fixos; as estruturas avançadas foram exercitadas nos exemplos do relatório do projeto, mas não comparadas em um fatorial de benchmark.

