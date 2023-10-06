# Descrição do Jogo - Resgate de Soldados

## Visão Geral

O jogo "Resgate de Soldados" é um jogo implementado em linguagem C, projetado para praticar conceitos de programação concorrente. O objetivo do jogo é controlar um helicóptero e transportar 10 soldados da esquerda da tela para uma plataforma localizada à direita. No entanto, existem desafios, como baterias antiaéreas inimigas que disparam foguetes aleatoriamente e obstáculos que podem causar a derrota do jogador.

## Autores

- Bernardo Gonçalves
- Guilherme Cardoso
- Lorenzo Coracini

#### Tutorial que podem ser úteis

- [Colisão objetos (bateria anti-aérea com ponte + foguete com helicóptero)](https://www.google.com/search?q=detect+colision+between+two+objects+sdl&oq=detect+colision+between+two+objects+sdl&gs_lcrp=EgZjaHJvbWUyBggAEEUYOTIJCAEQIRgKGKABMgkIAhAhGAoYoAHSAQg5MzQ1ajBqN6gCALACAA&sourceid=chrome&ie=UTF-8#fpstate=ive&vld=cid:4f9acd0d,vid:cWek-nbAcAE,st:0)

## Setup Linux

```bash
   # instala lib sdl2
   sudo apt install libsdl2-dev

   # compila jogo
   make build

   # executa jogo
   make run
```

## Threads

1. **Thread Helicóptero:**

   - Responsável pelo controle do helicóptero.
   - Gerencia a quantidade de soldados restantes a serem transportados.
   - Monitora a posição do helicóptero e detecta colisões com obstáculos.

2. **Thread Bateria Antiaérea:**
   - Cada bateria antiaérea é uma thread independente.
   - Controla o disparo de foguetes em direção ao helicóptero.
   - Gerencia a munição da bateria.
   - Move-se para o depósito de recarga quando a munição está esgotada.

## Variáveis Mutex

1. **Mutex Depósito Recarga:**

   - Controla o acesso ao depósito de recarga localizado à esquerda.
   - Garante que apenas uma bateria antiaérea possa reabastecer sua munição por vez.
   - Evita condições de corrida durante o reabastecimento.

2. **Mutex Ponte:**
   - Garante que apenas uma bateria antiaérea possa atravessar a ponte por vez.
   - Evita colisões ou bloqueios na ponte devido a múltiplas baterias tentando atravessar simultaneamente.

## Semáforos

1. **Semáforo Munição das Baterias Antiaéreas:**
   - Controla o acesso à munição de cada bateria antiaérea.
   - Quando a munição está esgotada, a bateria deve aguardar recarga.

## Recursos Adicionais

- **Níveis de Dificuldade:**
  - Implementação de uma variável global que determina o nível de dificuldade do jogo (fácil, médio, difícil).
  - Isso afeta a quantidade inicial de munição das baterias antiaéreas e o tempo de recarga.

## Falta Implementar

   - left building
   - right building
   - hostages
   - missiles ( shoot, reload and colision )
   - different game difficulties