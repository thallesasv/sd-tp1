# SD - TP1

Repositório do Trabalho Prático 1 de Sistemas Distribuídos (CEFET-MG, 2026/1).

Este diretório foi organizado para concentrar as implementações da Parte 2 (Pipes) e da Parte 3 (Produtor-Consumidor com Semáforos) do enunciado em SD_TP_1.pdf.

## Objetivos do TP1

O trabalho tem como foco praticar mecanismos de IPC (Interprocess Communication), threads e sincronização, com ênfase em C/C++.

Além da implementação, o enunciado exige:

- execução dos estudos de caso;
- análise dos resultados;
- relatório final (até 5 páginas) com decisões de projeto/implementação e avaliação dos casos.

## Parte 2 - Pipes (Produtor-Consumidor entre processos)

### Requisitos do enunciado

- comunicação entre 2 processos via pipe anônimo;
- criação do pipe antes de fork();
- produtor gera números aleatórios e crescentes:
	- N0 = 1
	- Ni = Ni-1 + delta
	- delta em [1, 100]
- consumidor recebe cada número e informa se é primo;
- encerramento com sentinela 0 enviada pelo produtor;
- cuidado com representação numérica no pipe (dica do enunciado: 20 bytes fixos por mensagem).

### Implementação atual (concluída)

Arquivos principais:

- parte2_pipes_producer.c
- parte2_pipes_consumer.c

Diretório do projeto:

```bash
cd "/home/thallesasv/Área de trabalho/SD/sd-tp1"
```

Compilação:

```bash
gcc -O2 -Wall -Wextra -o parte2_pipes_split parte2_pipes_producer.c parte2_pipes_consumer.c -lm
```

Execução:

```bash
./parte2_pipes_split <quantidade>
```

Exemplo:

```bash
./parte2_pipes_split 1000
```

### Execução dos testes e coleta de métricas (Parte 2)

Gerar 10 execuções de um cenário específico (`N=1000`) e salvar os logs na pasta correspondente:

```bash
mkdir -p testes/parte2/n1000
for i in $(seq 1 10); do ./parte2_pipes_split 1000 > "testes/parte2/n1000/teste_1000_$i.txt"; done
```

Gerar 10 execuções para todos os cenários (`N=1`, `10`, `100`, `1000`), organizando cada conjunto em sua pasta:

```bash
for n in 1 10 100 1000; do
	mkdir -p "testes/parte2/n$n"
	for i in $(seq 1 10); do
		./parte2_pipes_split "$n" > "testes/parte2/n$n/teste_${n}_$i.txt"
	done
done
```

Calcular automaticamente quantidade de amostras, média, mínimo e máximo de tempo por cenário (lendo os arquivos em `testes/parte2/n*`):

```bash
export LC_ALL=C
for d in testes/parte2/n*; do
	n=$(basename "$d" | sed 's/^n//')
	stats=$(awk '/Tempo total de execucao/{t=$5; sum+=t; c++; if(c==1 || t<min) min=t; if(c==1 || t>max) max=t} END{if(c>0) printf "%d %.6f %.6f %.6f", c, sum/c, min, max; else printf "0 0 0 0"}' "$d"/teste_*.txt)
	printf "N=%s amostras=%s media_s=%s min_s=%s max_s=%s\n" "$n" $stats
done | sort -n -t'=' -k2,2
```

### Comportamento esperado (Parte 2)

- O processo produtor gera números crescentes (`N0 = 1`, `Ni = Ni-1 + delta`, com `delta` entre 1 e 100).
- O processo consumidor recebe os números pelo pipe anônimo.
- Para cada número recebido, o consumidor informa se é primo ou não primo.
- Ao final, o produtor envia `0` para sinalizar encerramento do consumidor.
- Ao final da execução, o programa imprime o tempo total (`Tempo total de execucao: ... s`).

Exemplo de saída:

```text
Consumidor: 76 -> nao primo
Consumidor: 169 -> nao primo
Consumidor: 353 -> primo
...
Tempo total de execucao: 0.000200 s
```

Erros comuns:

- `gcc: command not found`: instale o compilador GCC.
- Erro de link relacionado a `sqrt`: garanta que a opção `-lm` está no comando de compilação.
- `Permissao negada` ao executar: rode `chmod +x parte2_pipes_split`.

## Parte 3 - Produtor-Consumidor com Semáforos (multithread)

### Requisitos do enunciado

- memória compartilhada como vetor de inteiros de tamanho N;
- Np threads produtoras e Nc threads consumidoras;
- produtor gera inteiros aleatórios em [1, 10^7];
- consumidor retira item do buffer e verifica primalidade;
- uso de semáforos para:
	- exclusão mútua no acesso ao buffer;
	- controle de vagas livres e posições ocupadas;
- término do programa após processar M = 10^5 números.

### Estudos de caso obrigatórios (Parte 3)

Valores de buffer:

- N = 1, 10, 100, 1000

Combinações de threads:

- (Np, Nc) = (1,1), (1,2), (1,4), (1,8), (2,1), (4,1), (8,1)

Medições e análises exigidas:

- executar cada combinação 10 vezes;
- calcular tempo médio de execução;
- gerar gráfico de tempo médio por combinação, com uma curva para cada N;
- gerar gráfico de ocupação do buffer ao longo do tempo para cada cenário;
- discutir conclusões observadas.