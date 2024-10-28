```
✅ -> Testado
```

# Benchmarks dos protótipos
- O código aloca dois vetores de tamanho *1 * 10⁶*, e em seguida, gera números aleatórios para servir de base para ambos os vetores, ou seja, antes de serem ordenados, eles possuem os mesmos valores.
- Será usado como base um script que executa o código 50 vezes, para que seja possível obter uma média dos tempos e compará-los no final.

### Benchmarks 23/102024

#### Benchmark (2 Threads) ✅
- Média para o Merge Sort com Threads: 0.32236614s
- Média para o Merge Sort sem Threads: 0.31587252s

#### Benchmark (4 Threads) ✅
- Média para o Merge Sort com Threads: 0.31097528s
- Média para o Merge Sort sem Threads: 0.32594196s

#### Benchmark (8 Threads) ✅
- Média para o Merge Sort com Threads: 0.32908504s
- Média para o Merge Sort sem Threads: 0.3450875510204081s

#### Benchmark (16 Threads) ✅
- Média para o Merge Sort com Threads: 0.31587252s
- Média para o Merge Sort sem Threads: 0.32236614s

# Benchmarks do projeto