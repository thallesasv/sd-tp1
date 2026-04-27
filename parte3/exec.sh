#!/bin/bash

gcc parte3.c -o parte3 -pthread

EXEC=./parte3
OUTPUT="resultados.csv"

echo "N,Np,Nc,Execucao,Tempo_real" > $OUTPUT

Ns=(1 10 100 1000)

combos=("1 1" "1 2" "1 4" "1 8" "2 1" "4 1" "8 1")

# Loop por todos os cenários
for N in "${Ns[@]}"; do
  for combo in "${combos[@]}"; do
    set -- $combo
    Np=$1
    Nc=$2

    echo "Rodando cenário N=$N, Np=$Np, Nc=$Nc..."

    # Executa 10 vezes cada cenário
    for i in $(seq 1 10); do
      # Usa 'time' para medir tempo real, ignorando saída do programa
      TEMPO=$( { /usr/bin/time -f "%e" $EXEC $N $Np $Nc > /dev/null; } 2>&1 )

      # Salva no CSV
      echo "$N,$Np,$Nc,$i,$TEMPO" >> $OUTPUT
    done
  done
done

echo "Testes concluídos! Resultados salvos em $OUTPUT"
