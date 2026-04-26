# Parte 2 - Pipes (Produtor/Consumidor)

Este diretório concentra a implementação da Parte 2 e os logs de teste gerados para os estudos de caso.

## Arquivos principais

- `parte2_pipes_producer.c`
- `parte2_pipes_consumer.c`

## Diretório de trabalho

Defina o caminho raiz do projeto em uma variável genérica e execute os comandos a partir dela:

```bash
export PROJECT_ROOT=/path/to/sd-tp1
```

Em seguida, entre na pasta da Parte 2:

```bash
cd "$PROJECT_ROOT/parte2"
```

## Compilação

```bash
gcc -O2 -Wall -Wextra -o parte2_pipes_split parte2_pipes_producer.c parte2_pipes_consumer.c -lm
```

## Execução

```bash
./parte2_pipes_split <quantidade>
```

Exemplo:

```bash
./parte2_pipes_split 10
```

## Funcionalidade implementada

- O produtor gera números crescentes com a regra `N0 = 1` e `Ni = Ni-1 + delta`, com `delta` em `[1, 100]`.
- O consumidor recebe cada valor pelo pipe anônimo e informa se é primo ou não primo.
- O produtor envia `0` como sentinela de encerramento.
- O programa imprime ao final o tempo total de execução.

## Protocolo de mensagens

- Cada valor é enviado em uma mensagem fixa de 20 bytes.
- O produtor serializa o número como string decimal com largura fixa.
- O consumidor lê exatamente 20 bytes por mensagem.

## Testes e coleta de métricas

Gerar 10 execuções de `N=1000`:

```bash
mkdir -p testes/n1000
for i in $(seq 1 10); do ./parte2_pipes_split 1000 > "testes/n1000/teste_1000_$i.txt"; done
```

Gerar 10 execuções para todos os cenários (`N=1`, `10`, `100`, `1000`):

```bash
for n in 1 10 100 1000; do
	mkdir -p "testes/n$n"
	for i in $(seq 1 10); do
		./parte2_pipes_split "$n" > "testes/n$n/teste_${n}_$i.txt"
	done
done
```

Calcular quantidade de amostras, média, mínimo e máximo por cenário:

```bash
export LC_ALL=C
for d in testes/n*; do
	n=$(basename "$d" | sed 's/^n//')
	stats=$(awk '/Tempo total de execucao/{t=$5; sum+=t; c++; if(c==1 || t<min) min=t; if(c==1 || t>max) max=t} END{if(c>0) printf "%d %.6f %.6f %.6f", c, sum/c, min, max; else printf "0 0 0 0"}' "$d"/teste_*.txt)
	printf "N=%s amostras=%s media_s=%s min_s=%s max_s=%s\n" "$n" $stats
done | sort -n -t'=' -k2,2
```

## Saída esperada

Exemplo:

```text
Consumidor: 76 -> nao primo
Consumidor: 169 -> nao primo
Consumidor: 353 -> primo
Tempo total de execucao: 0.000200 s
```

## Erros comuns

- `gcc: command not found`: instale o compilador GCC.
- Erro de link relacionado a `sqrt`: mantenha a opção `-lm` na compilação.
- `Permissao negada` ao executar: rode `chmod +x parte2_pipes_split`.