import pandas as pd
from great_tables import GT

# Lê o CSV gerado pelo shell script
df = pd.read_csv("resultados.csv")

# Calcula estatísticas
estatisticas = df.groupby(["N", "Np", "Nc"])["Tempo_real"].agg(
    NumExecTotal="count",
    TMedio="mean",
    TMin="min",
    TMax="max"
).reset_index()

# Salva em CSV também
estatisticas.to_csv("estatisticas.csv", index=False)

# Cria tabela bonita com great-tables
gt = GT(estatisticas)

# Formata colunas numéricas
gt = gt.fmt_number(columns=["TMedio", "TMin", "TMax"], decimals=6)

# Adiciona título
gt = gt.tab_header(title="Estatísticas de Tempo de Execução")

# Salvar como HTML
gt.save("estatisticas.html")

# Salvar como PDF
gt.save("estatisticas.pdf")
