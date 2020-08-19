#include "rules.h"
#include "analyze.h"
#include "position_cost.h"
#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <cassert>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <cmath>

using std::cout;
using std::endl;
using namespace std::chrono;

std::string debug;

extern bool stop_it;

extern long long AllCount;

extern int Cost_pos[64][13];

extern TripleHash Triple[(1<<16)];

Move Killer1[400], Killer2[400];

long long History[64][64];

inline int get_index(unsigned long long c)
{
    return __builtin_ctzll(c);
}

HashData HashTable[(1<<22)];

void push_to_hash(Move best, int cost, int depth, int alpha0, int beta0, unsigned long long fullHash)
{
    if (stop_it) return;
    if (HashTable[fullHash&((1<<22)-1)].depth>depth) return;
    unsigned long long shortHash=fullHash&((1<<22)-1);
    int s=cost;
    char DT='N';
    if (cost<=alpha0)
    {
        s=alpha0;
        DT='U';
    }
    if (cost>=beta0)
    {
        s=beta0;
        DT='L';
    }
    HashTable[shortHash]=HashData{fullHash,depth,s,DT,best};
}

inline int get_capture_cost(const Board &Position, Move &mv)
{
    return (abs(Cost[Position.pos[get_index(mv.dst)]])<<12)-abs(Cost[Position.pos[get_index(mv.src)]]);
}

void generate_captures(const Board &Position, MList &Moves)
{
    pawn_capture(Position, Moves, Position.Move);
    knight_capture(Position, Moves, Position.Move);
    bishop_capture(Position, Moves, Position.Move);
    rook_capture(Position, Moves, Position.Move);
    queen_capture(Position, Moves, Position.Move);
    king_capture(Position, Moves, Position.Move);
    for (int i=0; i<Moves.size; i++)
    {
        for (int j=i; j<Moves.size-1; j++)
        {
            if (get_capture_cost(Position, Moves[j])<get_capture_cost(Position, Moves[j+1])) std::swap(Moves[j], Moves[j+1]);
        }
    }
}

bool get_move_cost(Move &mv1, Move &mv2)
{
    return History[get_index(mv1.src)][get_index(mv1.dst)]>History[get_index(mv2.src)][get_index(mv2.dst)];
}

void generate_moves(const Board &Position, MList &Moves)
{
    castling(Position, Moves, Position.Move);
    king_move(Position, Moves, Position.Move);
    queen_move(Position, Moves, Position.Move);
    rook_move(Position, Moves, Position.Move);
    bishop_move(Position, Moves, Position.Move);
    knight_move(Position, Moves, Position.Move);
    pawn_move(Position, Moves, Position.Move);
    /*for (int i=0; i<Moves.size; i++)
    {
        for (int j=i; j<Moves.size-1; j++)
        {
            if (get_move_cost(Moves[j])<get_move_cost(Moves[j+1])) std::swap(Moves[j], Moves[j+1]);
        }
    }*/
    std::sort(Moves.a, Moves.a+Moves.size, get_move_cost);
}

int Critical_Analyze(Board &Position, int alpha, int beta, int check_count)
{
    if (stop_it) return 0;
    int val=PosCost(Position, alpha, beta);
    if (val>=beta) return beta;
    if (val>alpha) alpha=val;
    MList Moves;
    generate_captures(Position, Moves);
    pawn_move_pass(Position, Moves, Position.Move);
    for (int i=0; i<Moves.size; i++)
    {
        AllCount++;
        MadeMove pmv=make_move(Position, Moves[i]);
        if (!is_checked(Position, Position.Figures[11+(!Position.Move)].a[0], !Position.Move))
        {
            int found=-Critical_Analyze(Position,-beta,-alpha, check_count);
            alpha=std::max(alpha,found);
        }
        unmake_move(Position, pmv);
        if (alpha>=beta)
        {
            return beta;
        }
    }
    return alpha;
}

extern unsigned long long ZobristHash[64][13];
extern unsigned long long ZobristMove;
extern unsigned long long Zobrist_cstl[16];
extern unsigned long long Zobrist_pass[64];

extern int time_need;

#define try_analyze \
{                       \
    if (!is_checked(Position, Position.Figures[11+(!Position.Move)].a[0], !Position.Move))                       \
    {                       \
        cnt++;                  \
        MList curchain;                       \
        int found=alpha; \
        int reduce=4+additional_reduce;  \
        if (checked) reduce=0;  \
        if (pmv.src_piece<=2&&(pmv.dst_place<8||pmv.dst_place>=56)) reduce=0;  \
        if (reduce==4&&depth>8&&history_pruning>4)  \
        {  \
            found=-Main_Analyze(Position, -alpha-1, -alpha, depth-8, curchain, depth0, first_search_move,move_n+1, check_count, extended, shortened, cap, infinite, Start0);    \
            if (found>alpha)  \
            {  \
                found=-Main_Analyze(Position, -beta, -alpha, depth-4, curchain, depth0, first_search_move,move_n+1, check_count, extended, shortened, cap, infinite, Start0); \
            }  \
        }  \
        else  \
        {  \
            if (cnt==1) \
            { \
                found=-Main_Analyze(Position, -beta, -alpha, depth-reduce, curchain, depth0, first_search_move,move_n+1, check_count, extended, shortened, cap, infinite, Start0);    \
            } \
            else \
            { \
                found=-Main_Analyze(Position, -alpha-1, -alpha, depth-reduce, curchain, depth0, first_search_move,move_n+1, check_count, extended, shortened, cap, infinite, Start0);    \
                if (found > alpha && found < beta) \
                { \
                    found=-Main_Analyze(Position, -beta, -found, depth-reduce, curchain, depth0, first_search_move,move_n+1, check_count, extended, shortened, cap, infinite, Start0); \
                } \
            } \
        }  \
        alpha=std::max(alpha,found);                       \
        if (found>bestnum)                       \
        {                       \
            bestnum=found;                       \
            best=curmove;                       \
            bestchain=curchain; \
        }                       \
        auto finish=steady_clock().now(); \
        int time_spent=duration_cast<milliseconds>(finish-Start0).count(); \
        if (infinite==false&&time_spent>time_need) \
        { \
            stop_it=true; \
        } \
    }      \
}

TripleHash SmallTriple[(1<<16)];

std::string debug_info="";

extern int MAX_N;

const int fut_raz[3]={0,200,500};

int Main_Analyze(Board &Position, int alpha, int beta, int depth, MList &chain, int depth0, Move first_search_move, int move_n, int check_count, bool extended, bool shortened, bool was_capture, bool infinite, steady_clock::time_point Start0)
{
    if (stop_it) return 0;
    if (Triple[Position.Zobrist&((1<<12)-1)][Position.Zobrist]>=3) return 0;
    if (SmallTriple[Position.Zobrist&((1<<16)-1)][Position.Zobrist]>=1) return 0;
    if (Position.moves50==101) return 0;
    if (depth<=3||move_n>=MAX_N) return Critical_Analyze(Position, alpha, beta, check_count);
    SmallTriple[Position.Zobrist&((1<<16)-1)].insert(Position.Zobrist);
    bool checked=is_checked(Position, Position.Figures[11+Position.Move].a[0], Position.Move);
    int history_pruning=0;
    int cnt=0;
    bool Killer_used=false;
    Move best;
    MList bestchain;
    int bestnum=-30001;
    int alpha0=alpha;
    int beta0=beta;
    int additional_reduce=0;
    if (move_n==0&&depth0>4)
    {
        bool cap=false;
        if (Position.pos[get_index(first_search_move.dst)]!=0) cap=true;
        AllCount++;
        MadeMove pmv=make_move(Position, first_search_move);
        Move curmove=first_search_move;
        try_analyze;
        unmake_move(Position, pmv);
        if (stop_it) return 0;
        if (alpha>=beta)
        {
            if (depth==depth0)
            {
                debug_info="First move";
            }
            push_to_hash(best,beta,depth,alpha0,beta0,Position.Zobrist);
            SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
            return beta;
        }
    }
    MList tempchain;
    if (extended==false&&shortened==false&&was_capture==false&&checked==false&&move_n>0)
    {
        int PosCost0=PosCost(Position, alpha, beta);
        if (depth<=8&&PosCost0-fut_raz[depth/4]>=beta)
        {
            SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
            return beta;
        }
        if (depth<=8&&PosCost0>=beta)
        {
            additional_reduce=4;
        }
        int R=12;
        bool late_endspiel=false;
        if (__builtin_popcountll(Position.AllWhite)-__builtin_popcountll(Position.WhitePawn|Position.WhitePawn2|Position.WhitePawn7)==1) late_endspiel=true;
        if (__builtin_popcountll(Position.AllBlack)-__builtin_popcountll(Position.WhitePawn|Position.BlackPawn2|Position.BlackPawn7)==1) late_endspiel=true;
        if (depth0-depth>=20&&PosCost0>=beta&&late_endspiel==false)
        {
            Position.Move=(!Position.Move);
            Position.Zobrist^=ZobristMove;
            int found=-Main_Analyze(Position, -beta, -beta+1, depth-4-R, tempchain, depth0, first_search_move, move_n, check_count, false, true, was_capture, infinite, Start0);
            Position.Move=(!Position.Move);
            Position.Zobrist^=ZobristMove;
            if (found>=beta)
            {
                SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
                return beta;
            }
        }
    }
    MList Moves;
    unsigned long long hsh = Position.Zobrist&((1<<22)-1);
    const HashData &hashEntry = HashTable[hsh];
    if (stop_it) return 0;
    bool triple_gone=false;
    if (hashEntry.depth>=depth&&hashEntry.FullHash==Position.Zobrist)
    {
        if (hashEntry.DataType=='L')
        {
            alpha=std::max(alpha,hashEntry.cost);
        }
        if (hashEntry.DataType=='U')
        {
            beta=std::min(beta,hashEntry.cost);
        }
        if (alpha >= beta)
        {
            if (depth==depth0)
            {
                /*cout<<"info Critical LU hash error!"<<'\n';
                assert(false);*/
                debug_info="LU hash";
                chain.size=0;
                chain.push_back(hashEntry.BestMove);
            }
            SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
            return beta;
        }
        if (hashEntry.DataType=='N')
        {
            AllCount++;
            bool cap=false;
            if (Position.pos[get_index(hashEntry.BestMove.dst)]!=0) cap=true;
            MadeMove pmv=make_move(Position, hashEntry.BestMove);
            if (Triple[Position.Zobrist&((1<<12)-1)][Position.Zobrist]>=3) triple_gone=true;
            Move curmove=hashEntry.BestMove;
            try_analyze;
            unmake_move(Position, pmv);
            if (stop_it) return 0;
            if (alpha>=beta)
            {
                if (depth==depth0)
                {
                    /*cout<<"info Critical N hash error!"<<'\n';
                    assert(false);*/
                    debug_info="N hash";
                    chain.size=0;
                    chain.push_back(best);
                }
                SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
                return beta;
            }
            if (triple_gone==false)
            {
                bestchain.push_back(best);
                chain=bestchain;
                SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
                return alpha;
            }
        }
    }
    Move HMove{0, 0, 0};
    if (stop_it) return 0;
    if (hashEntry.depth>-1&&hashEntry.FullHash==Position.Zobrist&&triple_gone==false)
    {
        HMove=hashEntry.BestMove;
        AllCount++;
        bool cap=false;
        if (Position.pos[get_index(hashEntry.BestMove.dst)]!=0) cap=true;
        MadeMove pmv=make_move(Position, hashEntry.BestMove);
        Move curmove=hashEntry.BestMove;
        try_analyze;
        unmake_move(Position, pmv);
        if (stop_it) return 0;
        if (alpha>=beta)
        {
            if (depth==depth0)
            {
                /*cout<<"info Critical hash move error!"<<'\n';
                assert(false);*/
                debug_info="Hash move";
                chain.size=0;
                chain.push_back(best);
            }
            push_to_hash(best,beta,depth,alpha0,beta0,Position.Zobrist);
            SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
            return beta;
        }
    }
    if (stop_it) return 0;
    Move bmove;
    if (depth>20)
    {
        MList Temp;
        int found=-Main_Analyze(Position, -beta, -alpha, depth-12, Temp, depth0, first_search_move,move_n+1, check_count, extended, shortened, was_capture, infinite, Start0);
        if (Temp.size>0)
        {
            Move curmove=Temp[Temp.size-1];
            bmove=curmove;
            bool cap=false;
            if (Position.pos[get_index(curmove.dst)]!=0) cap=true;
            MadeMove pmv=make_move(Position, curmove);
            try_analyze(curmove);
            unmake_move(Position, pmv);
        }
    }
    generate_captures(Position, Moves);
    for (int i=0; i<Moves.size; i++)
    {
        if (Moves[i]==HMove) continue;
        if (Moves[i]==bmove) continue;
        AllCount++;
        bool cap=true;
        MadeMove pmv=make_move(Position, Moves[i]);
        Move curmove=Moves[i];
        try_analyze(Moves[i]);
        unmake_move(Position, pmv);
        if (stop_it) return 0;
        if (alpha>=beta)
        {
            if (depth==depth0)
            {
                /*cout<<"info Critical capture error!"<<'\n';
                assert(false);*/
                debug_info="Critical capture";
                chain.size=0;
                chain.push_back(best);
            }
            push_to_hash(best,beta,depth,alpha0,beta0,Position.Zobrist);
            SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
            return beta;
        }
    }
    Moves.size=0;
    if (stop_it) return 0;
    generate_moves(Position, Moves);
    for (int i=0; i<Moves.size; i++)
    {
        if (Moves[i]==Killer1[move_n]||Moves[i]==Killer2[move_n])
        {
            bool cap=false;
            if (Moves[i]==HMove) continue;
            AllCount++;
            MadeMove pmv=make_move(Position, Moves[i]);
            Move curmove=Moves[i];
            try_analyze(Moves[i]);
            unmake_move(Position, pmv);
            if (stop_it) return 0;
            if (alpha>=beta)
            {
                if (depth==depth0)
                {
                    /*cout<<"info Critical killer error!"<<'\n';
                    assert(false);*/
                    debug_info="Killer move";
                    chain.size=0;
                    chain.push_back(best);
                }
                push_to_hash(best,beta,depth,alpha0,beta0,Position.Zobrist);
                SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
                return beta;
            }
        }
    }
    Killer_used=true;
    if (stop_it) return 0;
    for (int i=0; i<Moves.size; i++)
    {
        if (Moves[i]==HMove||Moves[i]==Killer1[move_n]||Moves[i]==Killer2[move_n]) continue;
        if (Moves[i]==bmove) continue;
        AllCount++;
        bool cap=false;
        MadeMove pmv=make_move(Position, Moves[i]);
        Move curmove=Moves[i];
        history_pruning++;
        try_analyze(Moves[i]);
        unmake_move(Position, pmv);
        if (stop_it) return 0;
        if (alpha>=beta)
        {
            if (depth==depth0)
            {
                /*cout<<"info Critical moves error!"<<'\n';
                assert(false);*/
                debug_info="Move move";
                chain.size=0;
                chain.push_back(best);
            }
            History[get_index(curmove.src)][get_index(curmove.dst)]+=depth*depth;
            if (Killer_used)
            {
                Killer2[move_n]=Killer1[move_n];
                Killer1[move_n]=curmove;
            }
            push_to_hash(best,beta,depth,alpha0,beta0,Position.Zobrist);
            SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
            return beta;
        }
    }
    if (cnt==0)
    {
        SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
        if (checked) return -29999+(depth0-depth);
        else return 0;
    }
    push_to_hash(best,bestnum,depth,alpha0,beta0,Position.Zobrist);
    bestchain.push_back(best);
    chain=bestchain;
    SmallTriple[Position.Zobrist&((1<<16)-1)].erase(Position.Zobrist);
    return alpha;
}
