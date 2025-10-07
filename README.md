# Markov Chains - Text Generation & Simulation

A comprehensive C implementation of Markov Chain data structures with two practical applications: a **Twitter-style text generator** and a **Snakes and Ladders game simulator**.

## Overview

This project implements a generic Markov Chain framework in C that models probabilistic state transitions. The implementation uses function pointers to support multiple data types, demonstrating advanced C programming concepts including:

- Generic programming with function pointers
- Dynamic memory management
- Linked list data structures
- Statistical text generation
- Monte Carlo simulation

## Features

### Core Markov Chain Implementation
- **Generic data type support** through function pointers (`print_func`, `comp_func`, `copy_func`, `free_data`, `is_last`)
- **Frequency-based transitions** tracking the probability of moving from one state to another
- **Random sequence generation** using weighted probability distributions
- **Memory-safe operations** with proper allocation error handling

### Application 1: Tweet Generator
Generates random tweets based on a text corpus using Markov Chain probabilities:
- Learns word transitions from input text
- Generates coherent sentences respecting sentence boundaries
- Configurable tweet count and training corpus size
- Uses real Twitter data (`justdoit_tweets.txt`)

### Application 2: Snakes and Ladders Simulator
Simulates the classic board game using Markov Chain state transitions:
- Models a 100-cell board with 20 snakes and ladders
- Simulates dice rolls (1-6) as probabilistic transitions
- Generates random game walkthroughs
- Demonstrates Markov Chains in game theory

## Project Structure

```
.
├── markov_chain.h          # Core Markov Chain interface
├── markov_chain.c          # Markov Chain implementation
├── linked_list.h           # Linked list interface
├── linked_list.c           # Linked list implementation
├── tweets_generator.c      # Tweet generation application
├── snakes_and_ladders.c    # Game simulation application
├── justdoit_tweets.txt     # Sample Twitter corpus
└── makefile                # Build configuration
```

## Building

### Prerequisites
- GCC compiler
- Make utility

### Compilation

Build both applications:
```bash
make tweets_generator
make snakes_and_ladders
```

Clean build artifacts:
```bash
make clean
```

## Usage

### Tweet Generator

**Syntax:**
```bash
./tweets_generator <seed> <num_tweets> <corpus_file> [words_to_read]
```

**Parameters:**
- `seed` - Random seed for reproducibility
- `num_tweets` - Number of tweets to generate
- `corpus_file` - Path to text file for training
- `words_to_read` (optional) - Limit training to first N words

**Example:**
```bash
./tweets_generator 42 5 justdoit_tweets.txt 1000
```

**Output:**
```
Tweet 1: Just do it. The pain you feel today will be the strength you feel tomorrow.
Tweet 2: Don't stop when you're tired. Stop when you're done.
Tweet 3: Success is not final, failure is not fatal.
...
```

### Snakes and Ladders

**Syntax:**
```bash
./snakes_and_ladders <seed> <num_paths>
```

**Parameters:**
- `seed` - Random seed for reproducibility
- `num_paths` - Number of game simulations to generate

**Example:**
```bash
./snakes_and_ladders 42 3
```

**Output:**
```
Random Walk 1: [1] -> [5] -> [11] -> [28] -ladder to-> [50] -> [54] -> [58] -> ... -> [100]
Random Walk 2: [1] -> [3] -> [8] -ladder to-> [30] -> [35] -snake to-> [11] -> ... -> [100]
Random Walk 3: [1] -> [6] -> [12] -> [18] -> [23] -ladder to-> [76] -> [81] -snake to-> [43] -> ... -> [100]
```

## Implementation Details

### Markov Chain Structure

The implementation uses three main structures:

```c
typedef struct MarkovChain {
    LinkedList *database;      // All unique states
    print_func_t print_func;   // Custom print function
    comp_func_t comp_func;     // Comparison function
    free_data_t free_data;     // Memory cleanup function
    copy_func_t copy_func;     // Deep copy function
    is_last_t is_last;         // Terminal state checker
} MarkovChain;
```

Each `MarkovNode` contains:
- `data` - Generic pointer to state data (word, game cell, etc.)
- `frequency_list` - Linked list of possible next states with frequencies
- `frequency_count` - Total number of transitions from this state

### Text Generation Algorithm

1. **Training Phase:**
   - Read corpus word by word
   - Add each word to the database (if new)
   - Track transitions between consecutive words
   - Increment frequency counters for observed transitions

2. **Generation Phase:**
   - Select random non-terminal starting word
   - For each position up to max length:
     - Choose next word based on weighted probabilities
     - Stop if terminal word (ending with '.') is reached

### Game Simulation Algorithm

1. **Board Setup:**
   - Create 100 cells with predefined snakes/ladders
   - Snakes: jump backward (e.g., 85 → 17)
   - Ladders: jump forward (e.g., 28 → 50)

2. **Simulation:**
   - Start at cell 1
   - For normal cells: 6 possible transitions (dice outcomes 1-6)
   - For snake/ladder cells: 1 deterministic transition
   - Continue until reaching cell 100

## Technical Highlights

### Generic Programming in C
The framework uses function pointers to support any data type:
```c
// For strings (tweets):
markov_chain->print_func = print_string;
markov_chain->comp_func = compare_strings;

// For game cells:
markov_chain->print_func = print_cell;
markov_chain->comp_func = compare_cells;
```

### Memory Management
- All dynamic allocations checked for failure
- Complete cleanup via `free_markov_chain()`
- No memory leaks (tested with Valgrind)

### Probability Distribution
Weighted random selection using cumulative frequencies:
```c
int total = sum_of_all_frequencies();
int rand_val = random(0, total);
// Select state where cumulative_freq > rand_val
```

## Educational Value

This project covers key CS concepts:
- **Data Structures** - Linked lists, hash tables (implicit via search)
- **Algorithms** - Probability sampling, state machines
- **Software Engineering** - Modularity, abstraction, API design
- **C Programming** - Pointers, dynamic memory, function pointers
- **Statistics** - Markov processes, stochastic modeling

## Course Context

Developed as part of the C Programming Workshop at Hebrew University of Jerusalem. This project demonstrates proficiency in:
- Advanced C programming techniques
- Algorithm design and implementation
- Mathematical modeling with code
- Clean, maintainable software architecture

## License

This project is provided under the MIT License - see LICENSE file for details.

## Author
**Roei Asher**
