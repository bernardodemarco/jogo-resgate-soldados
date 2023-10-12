# Jogo Resgate de Soldados

## Sobre

O jogo faz parte do primeiro trabalho da disciplina de [Sistemas Operacionais (INE5611) da Universidade Federal de Santal Catarina](https://planos.inf.ufsc.br/modulos/planos/visualizar.php?id=4386). Ele foi implementado na linguagem C, com o objetivo de praticar conceitos de programação concorrente.

O objetivo do jogo é controlar um helicóptero e transportar 10 soldados de uma plataforma localizada à esquerda da tela para uma localizada à direita. No entanto, existem desafios, como baterias antiaéreas inimigas que disparam mísseis aleatoriamente contra o helicóptero.

O jogador vence o jogo caso consiga transportar todos os soldados sem ser atingido por nenhum míssil e sem colidir o helicóptero com nenhum outro objeto do cenário.

Além disso, existem três níveis de dificuldade: fácil, médio e difícil. Conforme o nível de dificuldade aumenta, a munição dos canhões aumenta, e o tempo de recarga da munição e o tempo entre cada disparo diminuem.

## Autores

- Bernardo De Marco Gonçalves
- Guilherme Cardoso de Oliveira
- Lorenzo Coracini de Oliveira

## Setup em ambiente Linux

```bash
   # instala lib sdl2
   sudo apt install libsdl2-dev

   # compila jogo
   make compile

   # executa jogo em nível de dificuldade fácil
   make easy

   # executa jogo em nível de dificuldade médio
   make medium

   # executa jogo em nível de dificuldade difícil
   make hard

   # remove arquivo compilado do jogo
   make clean
```

## Estruturas das pastas

```bash
  ├── src
  │   ├── game.c # jogo
  │   ├── constants.h # constantes
  │   └── types.h # definição de tipos
  ├── build
  │   ├── .gitkeep # arquivo para pasta ser reconhecida pelo git
  │   └── game # jogo compilado
  ├── Makefile
  ├── documentation.pdf # documentação do jogo
  └── README.md # documentação do jogo
```

## Threads

1. **Thread Helicóptero:**

   - Responsável pelo controle do helicóptero.
   - Transporta reféns da base à esquerda para a base à direita.
   - Gerencia a quantidade de soldados restantes a serem transportados.
   - Monitora a posição do helicóptero e detecta colisões com com objetos do cenário.

2. **Thread Bateria Antiaérea:**

   - Jogo possui duas bases antiaéreas (duas threads).
   - Cada bateria antiaérea move-se de um lado para outro.
   - Cada bateria antiaérea possui um vetor de mísseis.
   - Cada bateria antiaérea possui um semáforo para controle da munição.
   - Se possui munição disponível, realiza o disparo de um míssil (criação de uma thread Míssil).
   - Se não possui munição disponível, move-se para o depósito de recarga à esquerda.

3. **Thread Míssil:**
   - É criada pela bateria antiaérea.
   - Manipula as _structs_ Missile.
   - Se o míssil colidir com o helicóptero, então a thread é finalizada e é alterado o valor da variável _has_missile_collided_with_helicopter_ para _true_.
   - Se o míssil sair da tela e não tiver colidido com o helicóptero, o míssil é desativado e a thread é finalizada.

## Variáveis Mutex

1. **Mutex Depósito Recarga:**

   - Controla o acesso ao depósito de recarga localizado à esquerda.
   - Garante que apenas uma bateria antiaérea possa reabastecer sua munição por vez.

2. **Mutex Ponte:**
   - Garante que apenas uma bateria antiaérea possa atravessar a ponte por vez.

## Semáforos

1. **Semáforo Munição das Baterias Antiaéreas:**
   - Controla o acesso à munição de cada bateria antiaérea.
   - Antes de disparar um míssil, a bateria antiaérea tenta passa pelo semáforo. Se conseguir, então realiza o disparo. Se não conseguir passar pelo semáforo, ela move-se até o depósito de recarga.
   - No depósito de recarga, é executado _número de munição_ vezes a função _sem_post()_. Após essas execuções, é adicionado um _delay_.
   - Devido à variável _Mutex Depósito Recarga_, caso o depósito já esteja ocupado, a bateria antiaérea deve aguardar a sua liberação.

## Recursos Adicionais

- **Níveis de Dificuldade:**
  - Existem três níveis de dificuldade disponíveis: fácil, médio e difícil.
  - As variáveis que são alteradas de acordo com o nível de dificuldade são: munição, tempo de recarregamento e tempo entre disparos.
  - O nível de dificuldade é passado como parâmetro para a função _main()_. O valor 0 equivale a fácil, 1 a médio e 2 a difícil.
  - Os comandos no arquivo _Makefile_ para executar o jogo são: _easy_, _medium_ e _hard_.
  - Cada um deles passa o respectivo valor de dificuldade.
  - Logo no início da função _main()_, o número é pego e passado para uma outra função chamada _init_difficulty_vars()_.
  - Essa função inicializa o valor das variáveis _ammunition_, _time_between_shots_ e _reload_time_ de acordo com o nível de dificuldade.
