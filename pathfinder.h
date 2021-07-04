#pragma once

#include <iostream>
#include <set>
#include <stack>
#include "worlds.h"

using std::make_pair;
using std::pair;
using std::set;
using std::stack;

#define ROW 100
#define COL 60

typedef pair<int, int> Pair;
typedef pair<double, pair<int, int>> pPair;

struct cell;

bool isValid(int x, int y);
bool isUnBlocked(Worlds *world, int x, int y);
bool isDestination(int x, int y, Pair dest);
double calculateHValue(int x, int y, Pair dest);
void tracePath(cell cellDetails[][COL], Pair dest);
bool aStarSearch(Worlds *world, Pair src, Pair dest);
