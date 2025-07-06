# POSIX Multithreaded Game - Soldiers Rescue

This game was developed in C to practice and demonstrate concurrent programming concepts using POSIX threads and synchronization primitives. It simulates an air-rescue mission in which a helicopter must transport ten soldiers from a base on the left side of the screen to a base on the right, while avoiding missile attacks from enemy anti-aircraft batteries.

## Table of Contents

- [Concurrency Model](#concurrency-model)
- [Difficulty Levels](#difficulty-levels)
- [Build and Run](#build-and-run)
- [Project Structure](#project-structure)

## Concurrency Model

The game employs a set of cooperating threads to simulate the battlefield components. The **Helicopter thread** is responsible for tracking the helicopter's position, managing the soldier count, and detecting collisions with on-screen obstacles.

https://github.com/bernardodemarco/jogo-resgate-soldados/blob/f9859ea539bbb90327288519b8e5fcb78176c527/src/game.c#L201-L266

Each **Anti-Aircraft Battery thread** represents a moving battery unit. Each battery maintains its own semaphore-guarded ammunition supply. When ammunition is available, a missile thread is spawned; otherwise, the battery moves to a reload depot to replenish its supply.

https://github.com/bernardodemarco/jogo-resgate-soldados/blob/f9859ea539bbb90327288519b8e5fcb78176c527/src/game.c#L431-L495

Each **Missile thread** handles the behavior of a single projectile. It continuously updates its position until either impacting the helicopter, triggering a game-over condition, or leaving the screen and terminating without collision.

https://github.com/bernardodemarco/jogo-resgate-soldados/blob/f9859ea539bbb90327288519b8e5fcb78176c527/src/game.c#L280-L303

A mutex controls exclusive access to the bridge, allowing only one battery to cross at a time.

https://github.com/bernardodemarco/jogo-resgate-soldados/blob/f9859ea539bbb90327288519b8e5fcb78176c527/src/game.c#L471-L475

Another mutex protects the reload depot, ensuring that only one battery reloads at once.

https://github.com/bernardodemarco/jogo-resgate-soldados/blob/f9859ea539bbb90327288519b8e5fcb78176c527/src/game.c#L452-L458

Ammunition availability is regulated by semaphores to prevent firing when ammunition is (((depleted))).

https://github.com/bernardodemarco/jogo-resgate-soldados/blob/f9859ea539bbb90327288519b8e5fcb78176c527/src/game.c#L447-L450

https://github.com/bernardodemarco/jogo-resgate-soldados/blob/f9859ea539bbb90327288519b8e5fcb78176c527/src/game.c#L424-L429

## Difficulty Levels

Three difficulty settings are available: Easy, Medium, and Hard. These settings adjust ammunition capacity, reload time, and the interval between missile launches. Higher difficulty levels increase game intensity by providing more ammunition to the batteries while reducing waiting times.

## Build and Run

The project depends on the SDL 2 library for rendering graphical elements. On Debian-based systems, the following command installs the necessary dependency:

```bash
sudo apt install libsdl2-dev
```

To compile the game, execute:


```bash
make compile
```

The game can be run at the desired difficulty level using:

```bash
# runs in Easy mode
make easy

# runs in Medium mode
make medium

# runs in Hard mode
make hard
```

To remove compiled artifacts:

```bash
make clean
```

## Project Structure

<!-- ```bash
  ├── src
  │   ├── game.c # game source code
  │   ├── constants.h # constants
  │   └── types.h # types definition
  ├── build
  │   └── game # compiled game
  ├── Makefile
  └── README.md
``` -->
