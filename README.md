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

### Implementação atual

A documentação completa da Parte 2 está disponível no [parte2/README.md](parte2/README.md).

Resumo:

- os fontes da Parte 2 ficam em [parte2/parte2_pipes_producer.c](parte2/parte2_pipes_producer.c) e [parte2/parte2_pipes_consumer.c](parte2/parte2_pipes_consumer.c);
- os logs de teste ficam em [parte2/testes/](parte2/testes/);
- a documentação interna da Parte 2 contém compilação, execução, geração de logs e coleta de métricas.

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