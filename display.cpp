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

std::string field(std::string str) {
  return "\033[1m" + str + "\033[22m ";
}

displayer::displayer(long long start, long long end, int width) {
  cout << "\n\n";    // dummy lines
  SEED_START = start;
  SEED_END = end;
  SEEDS = SEED_END - SEED_START + 1;
  PREV_TIME = high_resolution_clock::now();
  START_TIME = PREV_TIME;
  BAR_WIDTH = width;
}

void displayer::progress(long long seed, int* deck, int turns, int SEEDS_BETWEEN_SNAPSHOTS) {
  const double percentage = (double)(seed - SEED_START) / SEEDS;
  const int bar_position = BAR_WIDTH * percentage;
  const duration<double, std::micro> time_elapsed = high_resolution_clock::now() - PREV_TIME;
  const double avg = time_elapsed.count() / SEEDS_BETWEEN_SNAPSHOTS;
  const long long eta = floor(avg * (SEED_END - seed) / 1'000'000);

  cout << MOVE_CURSOR; 
  
  //line 1
  cout << CLEAR_LINE;
  cout << print_cards(deck, DECK_SIZE/2);

  cout << spaces(4);
  for (int i = 0; i < BAR_WIDTH; ++i) {
      if (i <= bar_position) cout << "█";
      else cout << " ";
  }
  cout << spaces(2) << std::fixed << std::setprecision(2) << percentage * 100 << "%";
  cout << "\n";

  //line 2
  cout << CLEAR_LINE;
  cout << print_cards(deck, DECK_SIZE/2, DECK_SIZE/2);

  cout << spaces(4) << field("Most Turns:") << turns;
  cout << spaces(4) << field("Avg:") << std::fixed << std::setprecision(2) << avg << "μs";
  cout << spaces(4) << field("ETA:") << print_time(eta);
  cout << "\n";

  PREV_TIME = high_resolution_clock::now();
}

void displayer::summary(int* deck) {
  const duration<double, std::micro> time = high_resolution_clock::now() - START_TIME;
  const long long amt = floor(time.count() / 1'000'000);
  const double avg = time.count() / SEEDS;

  cout << "\n";
  cout << field("simulation took:");
  cout << print_time(amt) << "\n";

  cout << field("avg time per seed:");
  cout << std::fixed << std::setprecision(2) << avg << "μs" << "\n";

  cout << field("best cards:") << "\n";
  cout << print_cards(deck, DECK_SIZE/2);
  cout << "\n";
  cout << print_cards(deck, DECK_SIZE/2, DECK_SIZE/2);
}
}
