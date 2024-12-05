# Parallelism & Competition

- Para executar o programa, esteja na raiz do projeto e insira no terminal:

```bash
./psort14713982 <arquivo_entrada> <arquivo_saída> <num_threads>
```

- Para um melhor desempenho, o programa pode ser executado da seguinte maneira:

```bash
nice -19 (comando citado acima)
```

- Para uma avaliação de métrica de desempenho, o programa pode ser executado da seguinte maneira:

```bash
time (comando acima)
```

- Este programa está fortemente relacionado com sistemas operacionais Linux!
- Arquivo com um relatório com as escolhas tomadas ao decorrer do projeto está em `docs/DECISIONS.md` e arquivo com benchmarkings do projeto está em `docs/BENCHMARK.md`

# Benchmarking

- Antes de executar os scripts abaixo, certifique-se de dar as devidas permissões com o seguinte comando:

```bash
chmod +x ./script.sh && chmod +x ./umount.sh 
```

- Execute o código para tamanhos variados de vetores, com quantidades variáveis de threads, 50 vezes cada com o seguinte comando abaixo:

```bash
sudo ./script.sh
```

> Executando o comando acima, os arquivos criados serão escritos em memória para um benchmarking mais preciso

- Caso o script não seja finalizado por erro ou por tempo prolongado, opte por executar o comando abaixo para desmontar e desocupar a área de memória

```bash
sudo ./umount.sh
```

# Dependencies

- É necessário um sistema **baseado em Linux ou compatível** para executar o projeto. Dito isso, para se certificar que possui todas as dependências, segue o comando abaixo:

```bash
sudo apt update
sudo apt install gcc make build-essential libpthread-stubs0-dev python3 python3-pip
pip install jupyter pandas matplotlib
```