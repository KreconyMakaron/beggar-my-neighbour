#include<algorithm>
#include<cstring>
#include<random>
#include<omp.h>
#include"display.h"

const int DECK_SIZE = 52;
const int THREAD_COUNT = 8;
const int UPDATE_INTERVAL = 1'000'000;

typedef long long ll;

const int start_deck[DECK_SIZE] = {
  1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

ll SEED_START, SEED_END, SEEDS;

class queue {
public:
  int* data;
  size_t size;
  size_t head;
  size_t tail;

  queue(int* buffer, size_t count) {
    data = buffer;
    size = count;
    head = 0;
    tail = count;
  }

  void push(int val) {
    data[tail] = val;
    size++;
    tail++;
    if(tail >= DECK_SIZE) tail -= DECK_SIZE;
  }

  void pop() {
    size--;
    head++;
    if(head >= DECK_SIZE) head -= DECK_SIZE;
  }

  int front() {
    return data[head];
  }

  bool empty() {
    return size == 0;
  }
};


void simulate(int &turn_count, int *deck) {
  turn_count = 0;
  int penalty = 0;
  bool turn = 1;

  int buffer[3 * DECK_SIZE];
  memcpy(buffer,            deck,             sizeof(int) * DECK_SIZE / 2);
  memcpy(buffer+DECK_SIZE,  deck+DECK_SIZE/2, sizeof(int) * DECK_SIZE / 2);

  queue player1(buffer,             DECK_SIZE/2);
  queue player2(buffer+DECK_SIZE,   DECK_SIZE/2);
  queue pile   (buffer+2*DECK_SIZE, 0);

  while(!player1.empty() && !player2.empty()) {
    turn_count++;
    
    queue& cur = turn ? player1 : player2;
    queue& opp = turn ? player2 : player1;

    int card = cur.front();
    cur.pop();
    pile.push(card);
    turn = !turn;

    if(card) {
      penalty = card;
    }
    else if(penalty > 0) {
      penalty--;
      if(penalty == 0) {
        opp.size += pile.size;
        while(!pile.empty()) {
          opp.data[opp.tail] = pile.data[pile.head];
          opp.tail++;
          if(opp.tail >= DECK_SIZE) opp.tail -= DECK_SIZE;

          pile.size--;
          pile.head++;
          if(pile.head >= DECK_SIZE) pile.head -= DECK_SIZE;
        }
        pile.head = pile.tail = 0;
      }
      else turn = !turn;
    }
  }
}

int main(int argc, char** argv) {
  if(argc != 3) {
    std::cout << "retard";
    exit(-1);
  }

  SEED_START = std::stoll(argv[1]);
  SEED_END = std::stoll(argv[2]);
  SEEDS = SEED_END - SEED_START + 1;

  display::displayer d(SEED_START, SEED_END, std::chrono::high_resolution_clock::now());

  int g_most_turns = 0;
  int g_best_deck[DECK_SIZE];
  std::fill(g_best_deck, g_best_deck+DECK_SIZE, 0);

  #pragma omp parallel
  {
    std::mt19937 rng(omp_get_thread_num() ^ SEED_START);
    int deck[DECK_SIZE];
    memcpy(deck, start_deck, sizeof(int) * DECK_SIZE);

    #pragma omp for schedule(dynamic, 2048)
    for (ll seed = SEED_START; seed <= SEED_END; ++seed) {
      int turns;
      std::shuffle(deck, deck+DECK_SIZE, rng);

      simulate(turns, deck);

      #pragma omp critical
      {
        if(turns > g_most_turns) {
          g_most_turns = turns;
          memcpy(g_best_deck, deck, sizeof(int) * DECK_SIZE);
        }
        if(seed % UPDATE_INTERVAL == 0) {
          d.progress(seed, g_best_deck, g_most_turns, UPDATE_INTERVAL);
        }
      }
    }
  }

  d.summary(g_best_deck);
}
