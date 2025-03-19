#pragma once
#include<chrono>
#include<cmath>
#include<iomanip>
#include<string>
#include<iostream>

namespace display {
using std::chrono::time_point;
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::cout;

class displayer {
private:
    int SEED_START;
    int SEED_END;
    int BAR_WIDTH;
    int SEEDS;
    time_point<high_resolution_clock> PREV_TIME;
    time_point<high_resolution_clock> START_TIME;
public:
  displayer(long long start, long long end, int width = 35);
  void progress(long long seed, int* deck, int turns, const int UPDATE_INTERVAL);
  void summary(int* deck);
};
}
