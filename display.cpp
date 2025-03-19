#include "display.h"

namespace display {
const int DECK_SIZE = 52;
const char CARD_FACE[5] = {'-', 'J', 'Q', 'K', 'A'};
const char* MOVE_CURSOR = "\033[2A";
const char* CLEAR_LINE = "\r\033[K";

std::string print_cards(int* buffer, size_t length, size_t offset = 0) {
  std::string t;
  for(int i = 0; i < length; ++i) t += CARD_FACE[buffer[i + offset]];
  return t;
}

std::string print_time(long long time) {
  std::stringstream ss;
  if(time / 3600 % 24) ss << (time / 3600) % 24 << "h";
  else if(time / 60 % 60) ss << (time / 60) % 60 << "m";
  else ss << time % 60 << "s";
  return ss.str();
}

std::string spaces(int count) {
  std::string t = "";
  while(count--) t += " ";
  return t;
}

displayer::displayer(long long start, long long end, time_point<high_resolution_clock> start_time, int width) {
  std::cout << "\n\n";    // dummy lines
  SEED_START = start;
  SEED_END = end;
  TIME = start_time;
  BAR_WIDTH = width;
}

void displayer::progress(long long seed, int* deck, int turns, int SEEDS_BETWEEN_SNAPSHOTS) {
  const int SEEDS = SEED_END - SEED_START + 1;
  const double percentage = (double)(seed - SEED_START) / SEEDS;
  const int bar_position = BAR_WIDTH * percentage;
  const duration<double, std::micro> time_elapsed = high_resolution_clock::now() - TIME;
  const double avg = time_elapsed.count() / SEEDS_BETWEEN_SNAPSHOTS;
  const long long eta = floor(avg * (SEED_END - seed) / 1'000'000);

  std::cout << MOVE_CURSOR; 
  
  //line 1
  std::cout << CLEAR_LINE;
  std::cout << print_cards(deck, DECK_SIZE/2);

  std::cout << spaces(4);
  for (int i = 0; i < BAR_WIDTH; ++i) {
      if (i <= bar_position) std::cout << "█";
      else std::cout << " ";
  }
  std::cout << spaces(2) << std::fixed << std::setprecision(2) << percentage * 100 << "%";
  std::cout << "\n";

  //line 2
  std::cout << CLEAR_LINE;
  std::cout << print_cards(deck, DECK_SIZE/2, DECK_SIZE/2);

  std::cout << spaces(4) << "\033[1mMost Turns:\033[22m " << turns;
  std::cout << spaces(4) << "\033[1mAvg:\033[22m " << std::fixed << std::setprecision(2) << avg << "μs";
  std::cout << spaces(4) << "\033[1mETA:\033[22m " << print_time(eta);
  std::cout << "\n";

  TIME = high_resolution_clock::now();
}

void displayer::summary(int* deck) {
  auto now = high_resolution_clock::now();
  duration<double, std::micro> time = now - TIME;
  long long amt = floor(time.count() / 1'000'000);

  std::cout << "\n\033[1msimulation took:\033[22m ";
  std::cout << print_time(amt);
  std::cout << "\n\033[1mbest cards:\033[22m\n";

  std::cout << print_cards(deck, DECK_SIZE/2);
  std::cout << "\n";
  std::cout << print_cards(deck, DECK_SIZE/2, DECK_SIZE/2);
}
}
