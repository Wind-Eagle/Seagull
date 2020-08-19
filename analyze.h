#ifndef ANALYZE_H_INCLUDED
#define ANALYZE_H_INCLUDED

#include <string>
#include <chrono>

using namespace std::chrono;

struct HashData
{
    unsigned long long FullHash=0;
    int depth=-1;
    int cost;
    char DataType;
    Move BestMove={0,0,0};
};

void generate_captures(const Board &Position, MList &Moves);
void generate_moves(const Board &Position, MList &Moves);

int Main_Analyze(Board &Position, int alpha, int beta, int depth, MList &chain, int depth0, Move first_search_move, int move_n, int check_count, bool extended, bool shortened, bool was_capture, bool infinite, steady_clock::time_point Start0);
int Critical_Analyze(Board &Position, int alpha, int beta, int check_count);

#endif // ANALYZE_H_INCLUDED



