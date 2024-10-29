# Decisões para algoritmo de ordenação Multi-Thread

#### Algoritmo escolhido: MERGE SORT
- Como algoritmo escolhido para o desenvolvimento do projeto, seguiremos como base o *Merge Sort*, algoritmo de ordenação recursivo, que tem como motivação o princípio de divisão e conquista. Consiste em ir dividindo recursivamente o meu vetor pela metade, até chegar em um momento em que terei vários "vetores" de tamanho 1. Quando chegar nesse ponto, começo a combiná-los e ordená-los.
- Esse algortimo tem complexidade $nlog(n)$ de tempo e complexidade $nlog(n)$ de espaço, ou seja, ele é um algoritmo eficiente, mas não é um algoritmo *in loco*, ou seja, ele precisa de vetores temporários que auxiliam nas combinações.

#### Observações até o momento (23/10/2024)
- O protótipo criado para testar a implementação de um Merge Sort com uso de Threads, até o momento, está se mostrando pouco eficiente, visto que está sendo criada uma thread para cada sub-vetor até chegar em um ponto em que não temos mais threads disponívies, e continuamos com um *Merge Sort sequencial*, ou seja, sem o uso de paralelismo.
- O custo para as ações que envolvem threads, como criação e união compromete o desempenho total.

#### Observações até o momento (28/10/2024)

- O protótipo está evoluindo, deixando as Threads serem "independentes". Os problemas que ainda estão sendo investigados são os de situação de corrida, onde mais de uma Thread tenta acessar regiões críticas ao mesmo tempo, falha de segmentação, onde uma Thread tenta acessar endereços não mais disponíveis (sendo esse o problema mais crítico até então), entre outros problemas.
- Enquanto isso, os tempos dos algoritmos de ordenação sem usar Multithread eram mais rápidos do que usando Multithreaded, visto que o tempo de criar e gerenciar threads custa muito caro.  