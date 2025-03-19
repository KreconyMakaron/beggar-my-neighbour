#include<bits/stdc++.h>
#include<omp.h>

const int DECK_SIZE = 52;
const int THREAD_COUNT = 8;

typedef long long ll;

const char card_face[5] = {'-', 'J', 'Q', 'K', 'A'};

const int start_deck[DECK_SIZE] = {
  1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

ll SEED_START, SEED_END, SEEDS;

const int BAR_WIDTH = 35;
const int UPDATE_INTERVAL = 1'000'000;
const std::string space = "    ";
std::chrono::time_point<std::chrono::high_resolution_clock> bar;

void display_progress(ll seed, int* deck, int turns) {
  double percentage = (double)(seed - SEED_START) / SEEDS;
  int pos = BAR_WIDTH * percentage;

  std::cout << "\033[s\033[2A"; 
  
  //line 1
  std::cout << "\r\033[K";
  for(int i = 0; i < DECK_SIZE / 2; ++i) std::cout << card_face[deck[i]];

  std::cout << space;
  for (int i = 0; i < BAR_WIDTH; ++i) {
      if (i <= pos) std::cout << "█";
      else std::cout << " ";
  }
  std::cout << "  " << std::fixed << std::setprecision(2) << percentage * 100 << "%\n";

  //line 2
  std::cout << "\r\033[K";
  for(int i = 0; i < DECK_SIZE / 2; ++i) std::cout << card_face[deck[i+DECK_SIZE / 2]];

  std::chrono::duration<double, std::micro> time = std::chrono::high_resolution_clock::now() - bar;
  double avg = time.count() / UPDATE_INTERVAL;

  std::cout << space <<"\033[1mMost Turns:\033[22m " << turns << space << "\033[1mAvg:\033[22m ";
  std::cout << std::fixed << std::setprecision(2) << avg << "μs" << space;

  ll eta = floor(avg * (SEED_END - seed) / 1'000'000);

  std::cout << "\033[1mETA:\033[22m ";
  if(eta / 3600 % 24) std::cout << (eta / 3600) % 24 << "h";
  else if(eta / 60 % 60) std::cout << (eta / 60) % 60 << "m";
  else std::cout << eta % 60 << "s";
  std::cout << "\n";

  bar = std::chrono::high_resolution_clock::now();
}

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
    tail = (tail + 1);
    if(tail >= DECK_SIZE) tail -= DECK_SIZE;
  }

  void pop() {
    size--;
    head = (head + 1);
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

  std::cout << "\n\n";    // dummy lines

  SEED_START = std::stoll(argv[1]);
  SEED_END = std::stoll(argv[2]);
  SEEDS = SEED_END - SEED_START + 1;

  auto start = std::chrono::high_resolution_clock::now();
  bar = start;

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
          display_progress(seed, g_best_deck, g_most_turns);
        }
      }
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::micro> time = end - start;
  ll amt = time.count() / 1000000;

  std::cout << "\n\033[1msimulation took:\033[22m ";
  if(amt / 3600 % 24) std::cout << (amt / 3600) % 24 << "h";
  else if(amt / 60 % 60) std::cout << (amt / 60) % 60 << "m";
  else std::cout << amt % 60 << "s";
  std::cout << "\n\033[1mbest cards:\033[22m\n";

  for(int i = 0; i < DECK_SIZE / 2; ++i) std::cout << card_face[g_best_deck[i]];
  std::cout << "\n";
  for(int i = 0; i < DECK_SIZE / 2; ++i) std::cout << card_face[g_best_deck[i+DECK_SIZE / 2]];
}
