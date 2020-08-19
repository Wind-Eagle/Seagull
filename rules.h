#ifndef RULES_H_INCLUDED
#define RULES_H_INCLUDED

#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <cassert>
#include <set>

const int Cost[13]={0,100,-100,320,-320,330,-330,550,-550,1000,-1000,5000,-5000};

extern unsigned long long Knight[64];
extern unsigned long long King[64];

struct FList
{
    int a[10];
    int size=0;
    inline int & operator [] (int c)
    {
        return a[c];
    }
};

inline bool operator==(const FList &a, const FList &b) {
    if (a.size != b.size) return false;
    using std::set;
    set<int> s1(a.a, a.a + a.size);
    set<int> s2(b.a, b.a + b.size);
    assert((int)s1.size() == a.size);
    return s1 == s2;
    /*for (int i = 0; i < a.size; ++i) {
        if (a.a[i] != b.a[i]) return false;
    }
    return true;*/
}

struct Move
{
    unsigned long long src;
    unsigned long long dst;
    unsigned long long fgr;
};

struct MList_small
{
    Move a[2];
    int size=0;
    inline Move & operator [] (int c)
    {
        return a[c];
    }
    inline void push_back(Move c)
    {
        a[size]=c;
        size++;
    }
};

struct MList_medium
{
    Move a[8];
    int size=0;
    inline Move & operator [] (int c)
    {
        return a[c];
    }
    inline void push_back(Move c)
    {
        a[size]=c;
        size++;
    }
};

struct MList
{
    // PATCHED
    Move a[240];
    // END PATCHED
    int size=0;
    inline Move & operator [] (int c)
    {
        return a[c];
    }
    inline void push_back(const Move &c)
    {
        a[size]=c;
        size++;
    }
};

struct TripleHash
{
    unsigned long long a[50];
    int size=0;
    inline int operator [] (unsigned long long h)
    {
        int cnt=0;
        for (int i=0; i<size; i++)
        {
            if (a[i]==h) cnt++;
        }
        return cnt;
    }
    inline void insert(unsigned long long h)
    {
        a[size]=h;
        size++;
    }
    inline void erase(unsigned long long h)
    {
        for (int i=size-1; i>=0; i--)
        {
            if (a[i]==h)
            {
                for (int j=i; j<size-1; j++)
                {
                    std::swap(a[j],a[j+1]);
                }
                size--;
                break;
            }
        }
    }
};

inline bool operator == (const Move &a, const Move &b)
{
    return (a.src==b.src&&a.dst==b.dst&&a.fgr==b.fgr);
}

struct diagonal
{
    int f1;
    int f2;
    int s1;
    int s2;
};

extern unsigned long long Lout;
extern unsigned long long Rout;
extern unsigned long long Uout;
extern unsigned long long Dout;

struct Board
{
    unsigned long long WhitePawn=0;
    unsigned long long BlackPawn=0;
    unsigned long long WhitePawn2=0;
    unsigned long long BlackPawn7=0;
    unsigned long long WhitePawn7=0;
    unsigned long long BlackPawn2=0;
    unsigned long long AllWhite=0;
    unsigned long long AllBlack=0;
    int pos[64];
    unsigned long long diagonal_1[15];
    unsigned long long diagonal_2[15];
    unsigned long long horisontal[8];
    unsigned long long vertical[8];
    int Material=0;
    int Positional=0;
    bool Move=false;
    unsigned long long Zobrist=0;
    bool KingWhite=false;
    bool KingBlack=false;
    bool QueenWhite=false;
    bool QueenBlack=false;
    int moves50=0;
    int move_now=0;
    unsigned long long en_passing_line_w=0;
    unsigned long long en_passing_line_b=0;
    FList Figures[13];
};

template<typename T>
bool eqArray(const T *a, const T *b, int sz) {
    using std::cerr;
    using std::endl;
    for (int i = 0; i < sz; ++i) {
        if (!(*(a + i) == *(b + i))) { cerr << "Item #" << i << " differs!" << endl; return false; }
    }
    return true;
}


inline bool operator==(const Board &a, const Board &b) {
#define DO_CMP(field) if (a.field != b.field) { cerr << "Field " << #field << " differs: " << a.field << " " << b.field << endl; return lozh; }
#define DO_CMP_ARR(field, len) if (!eqArray(a.field, b.field, len)) { cerr << "Field " << #field << " differs" << endl; return lozh; }
    using std::cerr;
    using std::endl;
    const bool istina = true;
    const bool lozh = false;
    DO_CMP(WhitePawn);
    DO_CMP(BlackPawn);
    DO_CMP(WhitePawn2);
    DO_CMP(BlackPawn7);
    DO_CMP(WhitePawn7);
    DO_CMP(BlackPawn2);
    DO_CMP(AllWhite);
    DO_CMP(AllBlack);
    DO_CMP_ARR(pos, 64);
    DO_CMP_ARR(diagonal_1, 15);
    DO_CMP_ARR(diagonal_2, 15);
    DO_CMP_ARR(horisontal, 8);
    DO_CMP_ARR(vertical, 8);
    DO_CMP(Material);
    DO_CMP(Positional);
    DO_CMP(Move);
    DO_CMP(Zobrist);
    DO_CMP(KingWhite);
    DO_CMP(KingBlack);
    DO_CMP(QueenWhite);
    DO_CMP(QueenBlack);
    DO_CMP(moves50);
    DO_CMP(move_now);
    DO_CMP(en_passing_line_w);
    DO_CMP(en_passing_line_b);
    DO_CMP_ARR(Figures, 13);
    return istina;
#undef DO_CMP
#undef DO_CMP_ARR
}

struct MadeMove
{
    int src_place=0;
    int src_piece=0;
    int dst_place=0;
    int dst_piece=0;
    bool enPass=false;
    bool cstl=false;
    unsigned long long en_pass_w=0;
    unsigned long long en_pass_b=0;
    int castl=0;
    int moves50=0;
    int moves_now=0;
};

void initialize();

void parse_from_FEN(std::string notation, Board &Position);

void pawn_capture(const Board &Position, MList &Moves, bool side);
void pawn_move(const Board &Position, MList &Moves, bool side);
void pawn_move_pass(const Board &Position, MList &Moves, bool side);
void knight_capture(const Board &Position, MList &Moves, bool side);
void knight_move(const Board &Position, MList &Moves, bool side);
void king_capture(const Board &Position, MList &Moves, bool side);
void king_move(const Board &Position, MList &Moves, bool side);
void bishop_capture(const Board &Position, MList &Moves, bool side);
void bishop_move(const Board &Position, MList &Moves, bool side);
void rook_capture(const Board &Position, MList &Moves, bool side);
void rook_move(const Board &Position, MList &Moves, bool side);
void queen_capture(const Board &Position, MList &Moves, bool side);
void queen_move(const Board &Position, MList &Moves, bool side);
bool is_checked(const Board &Position, int cur_cell, bool side);
void castling(const Board &Position, MList &Moves, bool side);
MadeMove make_move(Board &Position, Move mv);
void unmake_move(Board &Position, MadeMove mv);

#endif // RULES_H_INCLUDED

