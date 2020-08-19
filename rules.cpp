/*
-----------------------------------------------------
|                                                   |
|  Special thanks to gepardo for fixing the rules!  |
|                                                   |
-----------------------------------------------------
*/
#include "rules.h"
#include "position_cost.h"
#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <cassert>
#include <algorithm>

using std::cout;
using std::endl;

template<typename T>
inline T abs(T a) { return a > 0 ? a : -a; }

std::mt19937_64 rnd(time(nullptr));

extern int Cost_pos[64][13];

int HList[100000];

diagonal DList[64];

TripleHash Triple[(1<<12)];

unsigned long long Knight[64];
unsigned long long King[64];
unsigned long long Checked_King[64];

unsigned long long ZobristHash[64][13];
unsigned long long ZobristMove;
unsigned long long Zobrist_cstl[16];
unsigned long long Zobrist_pass[64];

unsigned long long maskl=0;
unsigned long long maskr=0;
unsigned long long masku=0;
unsigned long long maskd=0;

unsigned long long D2=0, D7=0;

MList_small d1[15][256][8];
MList_small d2[15][256][8];
MList_small dh[8][256][8];
MList_small dv[8][256][8];
MList_medium dm1[15][256][8];
MList_medium dm2[15][256][8];
MList_medium dmh[8][256][8];
MList_medium dmv[8][256][8];

int RevList1[15][8];
int RevList2[15][8];

extern unsigned long long Vertical0[8], Horizontal0[8];

inline int get_index(unsigned long long c)
{
    return __builtin_ctzll(c);
}

void initialize()
{
    for (int i=0; i<64; i++)
    {
        if (i%8!=0) maskl|=(1ULL<<i);
        if (i%8!=7) maskr|=(1ULL<<i);
        if (i>7) maskd|=(1ULL<<i);
        if (i<56) masku|=(1ULL<<i);
        if (i/8==1) D2|=(1ULL<<i);
        if (i/8==6) D7|=(1ULL<<i);
    }
    for (int i=0; i<16; i++)
    {
        HList[(1ULL<<i)]=15-i;
    }
    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            for (int k1=-1; k1<=1; k1+=2)
            {
                for (int k2=-1; k2<=1; k2+=2)
                {
                    if (i+k1*2>=0&&i+k1*2<8&&j+k2>=0&&j+k2<8) Knight[i*8+j]|=(1ULL<<((i+k1*2)*8+j+k2));
                    if (i+k1>=0&&i+k1<8&&j+k2*2>=0&&j+k2*2<8) Knight[i*8+j]|=(1ULL<<((i+k1)*8+j+k2*2));
                }
            }
            for (int k1=-1; k1<=1; k1++)
            {
                for (int k2=-1; k2<=1; k2++)
                {
                    if (k1==0&&k2==0) continue;
                    if (i+k1>=0&&i+k1<8&&j+k2>=0&&j+k2<8)
                    {
                        King[i*8+j]|=(1ULL<<((i+k1)*8+j+k2));
                    }
                }
            }
            Checked_King[i*8+j]=(Knight[i*8+j]|King[i*8+j]);
        }
    }
    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            if (i+j<=7)
            {
                DList[i*8+j].f1=j;
                DList[i*8+j].s1=i+j+1;
            }
            else
            {
                DList[i*8+j].f1=j-(i+j-7);
                DList[i*8+j].s1=15-i-j;
            }
            if (i-j<=0)
            {
                DList[i*8+j].f2=i;
                DList[i*8+j].s2=i-j+8;
            }
            else
            {
                DList[i*8+j].f2=j;
                DList[i*8+j].s2=16-(i-j+8);
            }
            RevList1[i+j][DList[i*8+j].f1]=i*8+j;
            RevList2[i-j+7][DList[i*8+j].f2]=i*8+j;
        }
    }
    for (int i=0; i<64; i++)
    {
        for (int j=0; j<13; j++)
        {
            ZobristHash[i][j]=rnd();
        }
    }
    for (int i=0; i<16; i++)
    {
        Zobrist_cstl[i]=rnd();
    }
    for (int i=0; i<64; i++)
    {
        Zobrist_pass[i]=rnd();
    }
    ZobristMove=rnd();
    for (int i=0; i<15; i++)
    {
        int siz=i+1;
        if (i>7) siz=15-i;
        for (int j=0; j<(1LL<<siz); j++)
        {
            for (int pos=0; pos<siz; pos++)
            {
                int cur=pos-1;
                while (cur>=0)
                {
                    if (j&(1<<cur))
                    {
                        d1[i][j][pos].push_back({(1ULL<<RevList1[i][pos]),(1ULL<<RevList1[i][cur]),5});
                        break;
                    }
                    dm1[i][j][pos].push_back({(1ULL<<RevList1[i][pos]),(1ULL<<RevList1[i][cur]),5});
                    cur--;
                }
                cur=pos+1;
                while (cur<siz)
                {
                    if (j&(1<<cur))
                    {
                        d1[i][j][pos].push_back({(1ULL<<RevList1[i][pos]),(1ULL<<RevList1[i][cur]),5});
                        break;
                    }
                    dm1[i][j][pos].push_back({(1ULL<<RevList1[i][pos]),(1ULL<<RevList1[i][cur]),5});
                    cur++;
                }
            }
        }
    }
    for (int i=0; i<15; i++)
    {
        int siz=i+1;
        if (i>7) siz=15-i;
        for (int j=0; j<(1LL<<siz); j++)
        {
            for (int pos=0; pos<siz; pos++)
            {
                int cur=pos-1;
                while (cur>=0)
                {
                    if (j&(1<<cur))
                    {
                        d2[i][j][pos].push_back({(1ULL<<RevList2[i][pos]),(1ULL<<RevList2[i][cur]),5});
                        break;
                    }
                    dm2[i][j][pos].push_back({(1ULL<<RevList2[i][pos]),(1ULL<<RevList2[i][cur]),5});
                    cur--;
                }
                cur=pos+1;
                while (cur<siz)
                {
                    if (j&(1<<cur))
                    {
                        d2[i][j][pos].push_back({(1ULL<<RevList2[i][pos]),(1ULL<<RevList2[i][cur]),5});
                        break;
                    }
                    dm2[i][j][pos].push_back({(1ULL<<RevList2[i][pos]),(1ULL<<RevList2[i][cur]),5});
                    cur++;
                }
            }
        }
    }
    for (int i=0; i<8; i++)
    {
        int siz=8;
        for (int j=0; j<(1LL<<siz); j++)
        {
            for (int pos=0; pos<siz; pos++)
            {
                int cur=pos-1;
                while (cur>=0)
                {
                    if (j&(1<<cur))
                    {
                        dh[i][j][pos].push_back({(1ULL<<(i*8+pos)),(1ULL<<(i*8+cur)),6});
                        break;
                    }
                    dmh[i][j][pos].push_back({(1ULL<<(i*8+pos)),(1ULL<<(i*8+cur)),6});
                    cur--;
                }
                cur=pos+1;
                while (cur<siz)
                {
                    if (j&(1<<cur))
                    {
                        dh[i][j][pos].push_back({(1ULL<<(i*8+pos)),(1ULL<<(i*8+cur)),6});
                        break;
                    }
                    dmh[i][j][pos].push_back({(1ULL<<(i*8+pos)),(1ULL<<(i*8+cur)),6});
                    cur++;
                }
            }
        }
    }
    for (int i=0; i<8; i++)
    {
        int siz=8;
        for (int j=0; j<(1LL<<siz); j++)
        {
            for (int pos=0; pos<siz; pos++)
            {
                int cur=pos-1;
                while (cur>=0)
                {
                    if (j&(1<<cur))
                    {
                        dv[i][j][pos].push_back({(1ULL<<(pos*8+i)),(1ULL<<(cur*8+i)),6});
                        break;
                    }
                    dmv[i][j][pos].push_back({(1ULL<<(pos*8+i)),(1ULL<<(cur*8+i)),6});
                    cur--;
                }
                cur=pos+1;
                while (cur<siz)
                {
                    if (j&(1<<cur))
                    {
                        dv[i][j][pos].push_back({(1ULL<<(pos*8+i)),(1ULL<<(cur*8+i)),6});
                        break;
                    }
                    dmv[i][j][pos].push_back({(1ULL<<(pos*8+i)),(1ULL<<(cur*8+i)),6});
                    cur++;
                }
            }
        }
    }
}

void parse_from_FEN(std::string notation, Board &Position)
{
    Position.AllBlack=0;
    Position.AllWhite=0;
    Position.BlackPawn2=0;
    Position.BlackPawn7=0;
    Position.BlackPawn=0;
    Position.WhitePawn2=0;
    Position.WhitePawn7=0;
    Position.WhitePawn=0;
    for (int i=0; i<15; i++) Position.diagonal_1[i]=0;
    for (int i=0; i<15; i++) Position.diagonal_2[i]=0;
    for (int i=0; i<8; i++) Position.horisontal[i]=0;
    for (int i=0; i<8; i++) Position.vertical[i]=0;
    Position.en_passing_line_b=0;
    Position.en_passing_line_w=0;
    for (int i=0; i<13; i++)
    {
        Position.Figures[i].size=0;
    }
    Position.KingBlack=false;
    Position.QueenBlack=false;
    Position.KingWhite=false;
    Position.QueenWhite=false;
    Position.Material=0;
    Position.Positional=0;
    Position.moves50=0;
    Position.move_now=0;
    Position.Zobrist=0;
    Position.Move=false;
    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            Position.pos[i*8+j]=0;
        }
    }
    int pos=0;
    int x=7;
    int y=0;
    while (notation[pos]!=' ')
    {
        if (x<0) break;
        if (notation[pos]=='/')
        {
            pos++;
            continue;
        }
        if (notation[pos]>='0'&&notation[pos]<='9')
        {
            y+=notation[pos]-'0';
            if (y==8)
            {
                y=0;
                x--;
            }
            pos++;
            continue;
        }
        if (notation[pos]=='P') Position.pos[x*8+y]=1;
        if (notation[pos]=='N') Position.pos[x*8+y]=3;
        if (notation[pos]=='B') Position.pos[x*8+y]=5;
        if (notation[pos]=='R') Position.pos[x*8+y]=7;
        if (notation[pos]=='Q') Position.pos[x*8+y]=9;
        if (notation[pos]=='K') Position.pos[x*8+y]=11;
        if (notation[pos]=='p') Position.pos[x*8+y]=2;
        if (notation[pos]=='n') Position.pos[x*8+y]=4;
        if (notation[pos]=='b') Position.pos[x*8+y]=6;
        if (notation[pos]=='r') Position.pos[x*8+y]=8;
        if (notation[pos]=='q') Position.pos[x*8+y]=10;
        if (notation[pos]=='k') Position.pos[x*8+y]=12;
        y++;
        if (y==8)
        {
            y=0;
            x--;
        }
        pos++;
    }
    pos++;
    if (notation[pos]=='b') Position.Move=true;
    pos+=2;
    while (notation[pos]!=' ')
    {
        if (notation[pos]=='K') Position.KingWhite=true;
        if (notation[pos]=='k') Position.KingBlack=true;
        if (notation[pos]=='Q') Position.QueenWhite=true;
        if (notation[pos]=='q') Position.QueenBlack=true;
        pos++;
    }
    pos++;
    if (notation[pos]>='a'&&notation[pos]<='h')
    {
        if (Position.Move) Position.en_passing_line_w=(1ULL<<(16+(notation[pos]-'a')));
        else Position.en_passing_line_b=(1ULL<<(40+(notation[pos]-'a')));
    }
    else
    {
        Position.en_passing_line_w=0;
        Position.en_passing_line_b=0;
    }
    pos+=2;
    while (notation[pos]>='0'&&notation[pos]<='9')
    {
        Position.moves50*=10;
        Position.moves50+=notation[pos]-'0';
        pos++;
    }
    pos++;
    while (notation[pos]>='0'&&notation[pos]<='9')
    {
        Position.move_now*=10;
        Position.move_now+=notation[pos]-'0';
        pos++;
    }
    Position.move_now*=2;
    if (!Position.Move) Position.move_now--;
    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            Position.Zobrist^=ZobristHash[i*8+j][Position.pos[i*8+j]];
            Position.Material+=Cost[Position.pos[i*8+j]];
            if (Position.pos[i*8+j]==0) continue;
            Position.Positional+=Cost_pos[i*8+j][Position.pos[i*8+j]];
            Position.diagonal_1[i+j]|=(1ULL<<(DList[i*8+j].f1));
            Position.diagonal_2[i-j+7]|=(1ULL<<(DList[i*8+j].f2));
            Position.horisontal[i]|=(1ULL<<j);
            Position.vertical[j]|=(1ULL<<i);
            if (Position.pos[i*8+j]%2==1)
            {
                Position.AllWhite|=(1ULL<<(i*8+j));
            }
            if (Position.pos[i*8+j]%2==0)
            {
                Position.AllBlack|=(1ULL<<(i*8+j));
            }
            if (Position.pos[i*8+j]==1)
            {
                Position.WhitePawn|=(1ULL<<(i*8+j));
                if (i==1) Position.WhitePawn2|=(1ULL<<(i*8+j));
                if (i==6) Position.WhitePawn7|=(1ULL<<(i*8+j));
                continue;
            }
            if (Position.pos[i*8+j]==2)
            {
                Position.BlackPawn|=(1ULL<<(i*8+j));
                if (i==6) Position.BlackPawn7|=(1ULL<<(i*8+j));
                if (i==1) Position.BlackPawn2|=(1ULL<<(i*8+j));
                continue;
            }
            Position.Figures[Position.pos[i*8+j]][Position.Figures[Position.pos[i*8+j]].size]=i*8+j;
            Position.Figures[Position.pos[i*8+j]].size++;
        }
    }
    if (Position.Move) Position.Zobrist^=ZobristMove;
    Position.Zobrist^=Zobrist_cstl[(int)Position.QueenWhite+(int)Position.KingWhite*2+
    (int)Position.QueenBlack*4+(int)Position.KingBlack*8];
    if (Position.en_passing_line_w+Position.en_passing_line_b) Position.Zobrist^=Zobrist_pass[get_index(Position.en_passing_line_w+Position.en_passing_line_b)];
}

void pawn_capture(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        // PATCHED
        unsigned long long PawnCap=
        ((((Position.WhitePawn << 9LL)) | ((Position.WhitePawn << 7LL))) & (Position.AllBlack|Position.en_passing_line_b));
        while (PawnCap)
        {
            unsigned long long mv=(PawnCap & -PawnCap);
            if ((mv&maskl)&&(Position.WhitePawn&(mv>>9))) {
                if (mv < (1ULL << 56ULL)) {
                    Moves.push_back({mv>>9,mv,1});
                } else {
                    Moves.push_back({mv>>9,mv,3});
                    Moves.push_back({mv>>9,mv,5});
                    Moves.push_back({mv>>9,mv,7});
                    Moves.push_back({mv>>9,mv,9});
                }
            }
            if ((mv&maskr)&&(Position.WhitePawn&(mv>>7))) {
                if (mv < (1ULL << 56ULL)) {
                    Moves.push_back({mv>>7,mv,1});
                } else {
                    Moves.push_back({mv>>7,mv,3});
                    Moves.push_back({mv>>7,mv,5});
                    Moves.push_back({mv>>7,mv,7});
                    Moves.push_back({mv>>7,mv,9});
                }
            }
            PawnCap=(PawnCap^mv);
        }
        // END PATCHED
    }
    else
    {
        // PATCHED
        unsigned long long PawnCap=
        ((((Position.BlackPawn >> 9LL)) | ((Position.BlackPawn >> 7LL))) & (Position.AllWhite|Position.en_passing_line_w));
        while (PawnCap)
        {
            unsigned long long mv=(PawnCap & -PawnCap);
            if ((mv&maskr)&&(Position.BlackPawn&(mv<<9))) {
                if (mv >= (1ULL << 8ULL)) {
                    Moves.push_back({mv<<9,mv,2});
                } else {
                    Moves.push_back({mv<<9,mv,4});
                    Moves.push_back({mv<<9,mv,6});
                    Moves.push_back({mv<<9,mv,8});
                    Moves.push_back({mv<<9,mv,10});
                }
            }
            if ((mv&maskl)&&(Position.BlackPawn&(mv<<7))) {
                if (mv >= (1ULL << 8ULL)) {
                    Moves.push_back({mv<<7,mv,2});
                } else {
                    Moves.push_back({mv<<7,mv,4});
                    Moves.push_back({mv<<7,mv,6});
                    Moves.push_back({mv<<7,mv,8});
                    Moves.push_back({mv<<7,mv,10});
                }
            }
            PawnCap=(PawnCap^mv);
        }
        // END PATCHED
    }
}

void pawn_move(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        // PATCHED
        unsigned long long PawnCap=((Position.WhitePawn<<8LL) & (((~Position.AllWhite) & (~Position.AllBlack))));
        while (PawnCap)
        {
            unsigned long long mv=(PawnCap & -PawnCap);
            if (mv < (1ULL << 56ULL)) {
                Moves.push_back({mv>>8,mv,1});
            } else {
                Moves.push_back({mv>>8,mv,3});
                Moves.push_back({mv>>8,mv,5});
                Moves.push_back({mv>>8,mv,7});
                Moves.push_back({mv>>8,mv,9});
            }
            PawnCap=(PawnCap^mv);
        }
        // END PATCHED
        PawnCap=((Position.WhitePawn2<<16LL) & (((~Position.AllWhite) & (~Position.AllBlack))));
        while (PawnCap)
        {
            unsigned long long mv=(PawnCap & -PawnCap);
            if (((~Position.AllWhite) & (~Position.AllBlack))&(mv>>8))
                Moves.push_back({mv>>16,mv,1});
            PawnCap=(PawnCap^mv);
        }
    }
    else
    {
        // PATCHED
        unsigned long long PawnCap=((Position.BlackPawn>>8LL) & (((~Position.AllWhite) & (~Position.AllBlack))));
        while (PawnCap)
        {
            unsigned long long mv=(PawnCap & -PawnCap);
            if (mv >= (1ULL << 8ULL)) {
                Moves.push_back({mv<<8,mv,2});
            } else {
                Moves.push_back({mv<<8,mv,4});
                Moves.push_back({mv<<8,mv,6});
                Moves.push_back({mv<<8,mv,8});
                Moves.push_back({mv<<8,mv,10});
            }
            PawnCap=(PawnCap^mv);
        }
        // END PATCHED
        PawnCap=((Position.BlackPawn7>>16LL) & (((~Position.AllWhite) & (~Position.AllBlack))));
        while (PawnCap)
        {
            unsigned long long mv=(PawnCap & -PawnCap);
            if (((~Position.AllWhite) & (~Position.AllBlack))&(mv<<8))
                Moves.push_back({mv<<16,mv,2});
            PawnCap=(PawnCap^mv);
        }
    }
}

void pawn_move_pass(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        unsigned long long PawnCap=((Position.WhitePawn7<<8LL) & (((~Position.AllWhite) & (~Position.AllBlack))));
        while (PawnCap)
        {
            unsigned long long mv=(PawnCap & -PawnCap);
            Moves.push_back({mv>>8,mv,3});
            Moves.push_back({mv>>8,mv,5});
            Moves.push_back({mv>>8,mv,7});
            Moves.push_back({mv>>8,mv,9});
            PawnCap=(PawnCap^mv);
        }
    }
    else
    {
        unsigned long long PawnCap=((Position.BlackPawn2>>8LL) & (((~Position.AllWhite) & (~Position.AllBlack))));
        while (PawnCap)
        {
            unsigned long long mv=(PawnCap & -PawnCap);
            Moves.push_back({mv<<8,mv,4});
            Moves.push_back({mv<<8,mv,6});
            Moves.push_back({mv<<8,mv,8});
            Moves.push_back({mv<<8,mv,10});
            PawnCap=(PawnCap^mv);
        }
    }
}


void knight_capture(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        for (int i=0; i<Position.Figures[3].size; i++)
        {
            unsigned long long KnightCap=(Knight[Position.Figures[3].a[i]]&Position.AllBlack);
            while (KnightCap)
            {
                unsigned long long mv=(KnightCap & -KnightCap);
                Moves.push_back({(1ULL<<Position.Figures[3].a[i]),mv,3});
                KnightCap=(KnightCap^mv);
            }
        }
    }
    else
    {
        for (int i=0; i<Position.Figures[4].size; i++)
        {
            unsigned long long KnightCap=(Knight[Position.Figures[4].a[i]]&Position.AllWhite);
            while (KnightCap)
            {
                unsigned long long mv=(KnightCap & -KnightCap);
                Moves.push_back({(1ULL<<Position.Figures[4].a[i]),mv,4});
                KnightCap=(KnightCap^mv);
            }
        }
    }
}

void knight_move(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        for (int i=0; i<Position.Figures[3].size; i++)
        {
            unsigned long long KnightCap=(Knight[Position.Figures[3].a[i]]&((~Position.AllWhite) & (~Position.AllBlack)));
            while (KnightCap)
            {
                unsigned long long mv=(KnightCap & -KnightCap);
                Moves.push_back({(1ULL<<Position.Figures[3].a[i]),mv,3});
                KnightCap=(KnightCap^mv);
            }
        }
    }
    else
    {
        for (int i=0; i<Position.Figures[4].size; i++)
        {
            unsigned long long KnightCap=(Knight[Position.Figures[4].a[i]]&((~Position.AllWhite) & (~Position.AllBlack)));
            while (KnightCap)
            {
                unsigned long long mv=(KnightCap & -KnightCap);
                Moves.push_back({(1ULL<<Position.Figures[4].a[i]),mv,4});
                KnightCap=(KnightCap^mv);
            }
        }
    }
}

void king_capture(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        unsigned long long KingCap=(King[Position.Figures[11].a[0]]&Position.AllBlack);
        while (KingCap)
        {
            unsigned long long mv=(KingCap & -KingCap);
            Moves.push_back({(1ULL<<Position.Figures[11].a[0]),mv,11});
            KingCap=(KingCap^mv);
        }
    }
    else
    {
        unsigned long long KingCap=(King[Position.Figures[12].a[0]]&Position.AllWhite);
        while (KingCap)
        {
            unsigned long long mv=(KingCap & -KingCap);
            Moves.push_back({(1ULL<<Position.Figures[12].a[0]),mv,12});
            KingCap=(KingCap^mv);
        }
    }
}

void king_move(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        unsigned long long KingCap=(King[Position.Figures[11].a[0]]&((~Position.AllWhite) & (~Position.AllBlack)));
        while (KingCap)
        {
            unsigned long long mv=(KingCap & -KingCap);
            Moves.push_back({(1ULL<<Position.Figures[11].a[0]),mv,11});
            KingCap=(KingCap^mv);
        }
    }
    else
    {
        unsigned long long KingCap=(King[Position.Figures[12].a[0]]&((~Position.AllWhite) & (~Position.AllBlack)));
        while (KingCap)
        {
            unsigned long long mv=(KingCap & -KingCap);
            Moves.push_back({(1ULL<<Position.Figures[12].a[0]),mv,12});
            KingCap=(KingCap^mv);
        }
    }
}

void bishop_capture(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        for (int i=0; i<Position.Figures[5].size; i++)
        {
            int cur=Position.diagonal_1[(Position.Figures[5].a[i]>>3)+(Position.Figures[5].a[i]&7)];
            MList_small cpos=d1[(Position.Figures[5].a[i]>>3)+(Position.Figures[5].a[i]&7)]
            [cur][DList[Position.Figures[5].a[i]].f1];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllBlack&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,5});
            }
            cur=Position.diagonal_2[(Position.Figures[5].a[i]>>3)+7-(Position.Figures[5].a[i]&7)];
            cpos=d2[(Position.Figures[5].a[i]>>3)+7-(Position.Figures[5].a[i]&7)]
            [cur][DList[Position.Figures[5].a[i]].f2];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllBlack&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,5});
            }
        }
    }
    else
    {
        for (int i=0; i<Position.Figures[6].size; i++)
        {
            int cur=Position.diagonal_1[(Position.Figures[6].a[i]>>3)+(Position.Figures[6].a[i]&7)];
            MList_small cpos=d1[(Position.Figures[6].a[i]>>3)+(Position.Figures[6].a[i]&7)]
            [cur][DList[Position.Figures[6].a[i]].f1];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllWhite&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,6});
            }
            cur=Position.diagonal_2[(Position.Figures[6].a[i]>>3)+7-(Position.Figures[6].a[i]&7)];
            cpos=d2[(Position.Figures[6].a[i]>>3)+7-(Position.Figures[6].a[i]&7)]
            [cur][DList[Position.Figures[6].a[i]].f2];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllWhite&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,6});
            }
        }
    }
}

void bishop_move(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        for (int i=0; i<Position.Figures[5].size; i++)
        {
            int cur=Position.diagonal_1[(Position.Figures[5].a[i]>>3)+(Position.Figures[5].a[i]&7)];
            MList_medium cpos=dm1[(Position.Figures[5].a[i]>>3)+(Position.Figures[5].a[i]&7)]
            [cur][DList[Position.Figures[5].a[i]].f1];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,5});
            }
            cur=Position.diagonal_2[(Position.Figures[5].a[i]>>3)+7-(Position.Figures[5].a[i]&7)];
            cpos=dm2[(Position.Figures[5].a[i]>>3)+7-(Position.Figures[5].a[i]&7)]
            [cur][DList[Position.Figures[5].a[i]].f2];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,5});
            }
        }
    }
    else
    {
        for (int i=0; i<Position.Figures[6].size; i++)
        {
            int cur=Position.diagonal_1[(Position.Figures[6].a[i]>>3)+(Position.Figures[6].a[i]&7)];
            MList_medium cpos=dm1[(Position.Figures[6].a[i]>>3)+(Position.Figures[6].a[i]&7)]
            [cur][DList[Position.Figures[6].a[i]].f1];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,6});
            }
            cur=Position.diagonal_2[(Position.Figures[6].a[i]>>3)+7-(Position.Figures[6].a[i]&7)];
            cpos=dm2[(Position.Figures[6].a[i]>>3)+7-(Position.Figures[6].a[i]&7)]
            [cur][DList[Position.Figures[6].a[i]].f2];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,6});
            }
        }
    }
}

void rook_capture(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        for (int i=0; i<Position.Figures[7].size; i++)
        {
            int cur=Position.horisontal[Position.Figures[7].a[i]>>3];
            MList_small cpos=dh[Position.Figures[7].a[i]>>3]
            [cur][Position.Figures[7].a[i]&7];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllBlack&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,7});
            }
            cur=Position.vertical[Position.Figures[7].a[i]&7];
            cpos=dv[Position.Figures[7].a[i]&7]
            [cur][Position.Figures[7].a[i]>>3];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllBlack&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,7});
            }
        }
    }
    else
    {
        for (int i=0; i<Position.Figures[8].size; i++)
        {
            int cur=Position.horisontal[Position.Figures[8].a[i]>>3];
            MList_small cpos=dh[Position.Figures[8].a[i]>>3]
            [cur][Position.Figures[8].a[i]&7];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllWhite&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,8});
            }
            cur=Position.vertical[Position.Figures[8].a[i]&7];
            cpos=dv[Position.Figures[8].a[i]&7]
            [cur][Position.Figures[8].a[i]>>3];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllWhite&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,8});
            }
        }
    }
}

void rook_move(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        for (int i=0; i<Position.Figures[7].size; i++)
        {
            int cur=Position.horisontal[Position.Figures[7].a[i]>>3];
            MList_medium cpos=dmh[Position.Figures[7].a[i]>>3]
            [cur][Position.Figures[7].a[i]&7];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,7});
            }
            cur=Position.vertical[Position.Figures[7].a[i]&7];
            cpos=dmv[Position.Figures[7].a[i]&7]
            [cur][Position.Figures[7].a[i]>>3];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,7});
            }
        }
    }
    else
    {
        for (int i=0; i<Position.Figures[8].size; i++)
        {
            int cur=Position.horisontal[Position.Figures[8].a[i]>>3];
            MList_medium cpos=dmh[Position.Figures[8].a[i]>>3]
            [cur][Position.Figures[8].a[i]&7];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,8});
            }
            cur=Position.vertical[Position.Figures[8].a[i]&7];
            cpos=dmv[Position.Figures[8].a[i]&7]
            [cur][Position.Figures[8].a[i]>>3];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,8});
            }
        }
    }
}

void queen_capture(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        for (int i=0; i<Position.Figures[9].size; i++)
        {
            int cur=Position.horisontal[Position.Figures[9].a[i]>>3];
            MList_small cpos=dh[Position.Figures[9].a[i]>>3]
            [cur][Position.Figures[9].a[i]&7];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllBlack&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,9});
            }
            cur=Position.vertical[Position.Figures[9].a[i]&7];
            cpos=dv[Position.Figures[9].a[i]&7]
            [cur][Position.Figures[9].a[i]>>3];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllBlack&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,9});
            }
            cur=Position.diagonal_1[(Position.Figures[9].a[i]>>3)+(Position.Figures[9].a[i]&7)];
            cpos=d1[(Position.Figures[9].a[i]>>3)+(Position.Figures[9].a[i]&7)]
            [cur][DList[Position.Figures[9].a[i]].f1];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllBlack&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,9});
            }
            cur=Position.diagonal_2[(Position.Figures[9].a[i]>>3)+7-(Position.Figures[9].a[i]&7)];
            cpos=d2[(Position.Figures[9].a[i]>>3)+7-(Position.Figures[9].a[i]&7)]
            [cur][DList[Position.Figures[9].a[i]].f2];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllBlack&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,9});
            }
        }
    }
    else
    {
        for (int i=0; i<Position.Figures[10].size; i++)
        {
            int cur=Position.horisontal[Position.Figures[10].a[i]>>3];
            MList_small cpos=dh[Position.Figures[10].a[i]>>3]
            [cur][Position.Figures[10].a[i]&7];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllWhite&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,10});
            }
            cur=Position.vertical[Position.Figures[10].a[i]&7];
            cpos=dv[Position.Figures[10].a[i]&7]
            [cur][Position.Figures[10].a[i]>>3];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllWhite&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,10});
            }
            cur=Position.diagonal_1[(Position.Figures[10].a[i]>>3)+(Position.Figures[10].a[i]&7)];
            cpos=d1[(Position.Figures[10].a[i]>>3)+(Position.Figures[10].a[i]&7)]
            [cur][DList[Position.Figures[10].a[i]].f1];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllWhite&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,10});
            }
            cur=Position.diagonal_2[(Position.Figures[10].a[i]>>3)+7-(Position.Figures[10].a[i]&7)];
            cpos=d2[(Position.Figures[10].a[i]>>3)+7-(Position.Figures[10].a[i]&7)]
            [cur][DList[Position.Figures[10].a[i]].f2];
            for (int j=0; j<cpos.size; j++)
            {
                if (Position.AllWhite&cpos[j].dst) Moves.push_back({cpos[j].src,cpos[j].dst,10});
            }
        }
    }
}

void queen_move(const Board &Position, MList &Moves, bool side)
{
    if (!side)
    {
        for (int i=0; i<Position.Figures[9].size; i++)
        {
            int cur=Position.horisontal[Position.Figures[9].a[i]>>3];
            MList_medium cpos=dmh[Position.Figures[9].a[i]>>3]
            [cur][Position.Figures[9].a[i]&7];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,9});
            }
            cur=Position.vertical[Position.Figures[9].a[i]&7];
            cpos=dmv[Position.Figures[9].a[i]&7]
            [cur][Position.Figures[9].a[i]>>3];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,9});
            }
            cur=Position.diagonal_1[(Position.Figures[9].a[i]>>3)+(Position.Figures[9].a[i]&7)];
            cpos=dm1[(Position.Figures[9].a[i]>>3)+(Position.Figures[9].a[i]&7)]
            [cur][DList[Position.Figures[9].a[i]].f1];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,9});
            }
            cur=Position.diagonal_2[(Position.Figures[9].a[i]>>3)+7-(Position.Figures[9].a[i]&7)];
            cpos=dm2[(Position.Figures[9].a[i]>>3)+7-(Position.Figures[9].a[i]&7)]
            [cur][DList[Position.Figures[9].a[i]].f2];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,9});
            }
        }
    }
    else
    {
        for (int i=0; i<Position.Figures[10].size; i++)
        {
            int cur=Position.horisontal[Position.Figures[10].a[i]>>3];
            MList_medium cpos=dmh[Position.Figures[10].a[i]>>3]
            [cur][Position.Figures[10].a[i]&7];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,10});
            }
            cur=Position.vertical[Position.Figures[10].a[i]&7];
            cpos=dmv[Position.Figures[10].a[i]&7]
            [cur][Position.Figures[10].a[i]>>3];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,10});
            }
            cur=Position.diagonal_1[(Position.Figures[10].a[i]>>3)+(Position.Figures[10].a[i]&7)];
            cpos=dm1[(Position.Figures[10].a[i]>>3)+(Position.Figures[10].a[i]&7)]
            [cur][DList[Position.Figures[10].a[i]].f1];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,10});
            }
            cur=Position.diagonal_2[(Position.Figures[10].a[i]>>3)+7-(Position.Figures[10].a[i]&7)];
            cpos=dm2[(Position.Figures[10].a[i]>>3)+7-(Position.Figures[10].a[i]&7)]
            [cur][DList[Position.Figures[10].a[i]].f2];
            for (int j=0; j<cpos.size; j++)
            {
                Moves.push_back({cpos[j].src,cpos[j].dst,10});
            }
        }
    }
}

bool is_checked(const Board &Position, int cur_cell, bool side)
{
    if (!side)
    {
        int cur=Position.horisontal[cur_cell>>3];
        MList_small cpos=dh[cur_cell>>3]
        [cur][cur_cell&7];
        for (int j=0; j<cpos.size; j++)
        {
            if (Position.pos[__builtin_ctzll(cpos[j].dst)]==10||Position.pos[__builtin_ctzll(cpos[j].dst)]==8)
                return true;
        }
        cur=Position.vertical[cur_cell&7];
        cpos=dv[cur_cell&7]
        [cur][cur_cell>>3];
        for (int j=0; j<cpos.size; j++)
        {
            if (Position.pos[__builtin_ctzll(cpos[j].dst)]==10||Position.pos[__builtin_ctzll(cpos[j].dst)]==8)
                return true;
        }
        cur=Position.diagonal_1[(cur_cell>>3)+(cur_cell&7)];
        cpos=d1[(cur_cell>>3)+(cur_cell&7)]
        [cur][DList[cur_cell].f1];
        for (int j=0; j<cpos.size; j++)
        {
            if (Position.pos[__builtin_ctzll(cpos[j].dst)]==10||Position.pos[__builtin_ctzll(cpos[j].dst)]==6)
                return true;
        }
        cur=Position.diagonal_2[(cur_cell>>3)+7-(cur_cell&7)];
        cpos=d2[(cur_cell>>3)+7-(cur_cell&7)]
        [cur][DList[cur_cell].f2];
        for (int j=0; j<cpos.size; j++)
        {
            if (Position.pos[__builtin_ctzll(cpos[j].dst)]==10||Position.pos[__builtin_ctzll(cpos[j].dst)]==6)
                return true;
        }
        unsigned long long KingCap=(King[cur_cell]&Position.AllBlack);
        while (KingCap)
        {
            unsigned long long mv=(KingCap & -KingCap);
            if (Position.pos[__builtin_ctzll(mv)]==12) return true;
            KingCap^=mv;
        }
        unsigned long long KnightCap=(Knight[cur_cell]&Position.AllBlack);
        while (KnightCap)
        {
            unsigned long long mv=(KnightCap & -KnightCap);
            if (Position.pos[__builtin_ctzll(mv)]==4) return true;
            KnightCap^=mv;
        }
        if (cur_cell<56)
        {
            if (((Position.BlackPawn|Position.BlackPawn2)&(1ULL<<(cur_cell+7)))&&(cur_cell&7))
                return true;
            if (((Position.BlackPawn|Position.BlackPawn2)&(1ULL<<(cur_cell+9)))&&(cur_cell&7)!=7)
                return true;
        }
    }
    else
    {
        int cur=Position.horisontal[cur_cell>>3];
        MList_small cpos=dh[cur_cell>>3]
        [cur][cur_cell&7];
        for (int j=0; j<cpos.size; j++)
        {
            if (Position.pos[__builtin_ctzll(cpos[j].dst)]==9||Position.pos[__builtin_ctzll(cpos[j].dst)]==7)
                return true;
        }
        cur=Position.vertical[cur_cell&7];
        cpos=dv[cur_cell&7]
        [cur][cur_cell>>3];
        for (int j=0; j<cpos.size; j++)
        {
            if (Position.pos[__builtin_ctzll(cpos[j].dst)]==9||Position.pos[__builtin_ctzll(cpos[j].dst)]==7)
                return true;
        }
        cur=Position.diagonal_1[(cur_cell>>3)+(cur_cell&7)];
        cpos=d1[(cur_cell>>3)+(cur_cell&7)]
        [cur][DList[cur_cell].f1];
        for (int j=0; j<cpos.size; j++)
        {
            if (Position.pos[__builtin_ctzll(cpos[j].dst)]==9||Position.pos[__builtin_ctzll(cpos[j].dst)]==5)
                return true;
        }
        cur=Position.diagonal_2[(cur_cell>>3)+7-(cur_cell&7)];
        cpos=d2[(cur_cell>>3)+7-(cur_cell&7)]
        [cur][DList[cur_cell].f2];
        for (int j=0; j<cpos.size; j++)
        {
            if (Position.pos[__builtin_ctzll(cpos[j].dst)]==9||Position.pos[__builtin_ctzll(cpos[j].dst)]==5)
                return true;
        }
        unsigned long long KingCap=(King[cur_cell]&Position.AllWhite);
        while (KingCap)
        {
            unsigned long long mv=(KingCap & -KingCap);
            if (Position.pos[__builtin_ctzll(mv)]==11) return true;
            KingCap^=mv;
        }
        unsigned long long KnightCap=(Knight[cur_cell]&Position.AllWhite);
        while (KnightCap)
        {
            unsigned long long mv=(KnightCap & -KnightCap);
            if (Position.pos[__builtin_ctzll(mv)]==3) return true;
            KnightCap^=mv;
        }
        if (cur_cell>7)
        {
            if (((Position.WhitePawn|Position.WhitePawn7)&(1ULL<<(cur_cell-7)))&&(cur_cell&7)!=7)
                return true;
            if (((Position.WhitePawn|Position.WhitePawn7)&(1ULL<<(cur_cell-9)))&&(cur_cell&7))
                return true;
        }
    }
    return false;
}

void castling(const Board &Position, MList &Moves, bool side)
{
    unsigned long long FreeCell=(~(Position.AllWhite|Position.AllBlack));
    if (!side)
    {
        if (Position.KingWhite&&(FreeCell&(1ULL<<5))&&(FreeCell&(1ULL<<6))&&
        (!is_checked(Position, 4, Position.Move))&&
        (!is_checked(Position, 5, Position.Move)))
        {
            Moves.push_back({(1ULL<<4),(1ULL<<6),11});
        }
        if (Position.QueenWhite&&(FreeCell&(1ULL<<3))&&(FreeCell&(1ULL<<2))&&(FreeCell&(1ULL<<1))&&
        (!is_checked(Position, 4, Position.Move))&&
        (!is_checked(Position, 3, Position.Move)))
        {
            Moves.push_back({(1ULL<<4),(1LL<<2),11});
        }
    }
    else
    {
        if (Position.KingBlack&&(FreeCell&(1ULL<<61))&&(FreeCell&(1ULL<<62))&&
        (!is_checked(Position, 60, Position.Move))&&
        (!is_checked(Position, 61, Position.Move)))
        {
            Moves.push_back({(1ULL<<60),(1ULL<<62),12});
        }
        if (Position.QueenBlack&&(FreeCell&(1ULL<<59))&&(FreeCell&(1ULL<<58))&&(FreeCell&(1ULL<<57))&&
        (!is_checked(Position, 60, Position.Move))&&
        (!is_checked(Position, 59, Position.Move)))
        {
            Moves.push_back({(1ULL<<60),(1ULL<<58),12});
        }
    }
}

inline void change_pawn(Board &Position)
{
    Position.WhitePawn2=(Position.WhitePawn&D2);
    Position.WhitePawn7=(Position.WhitePawn&D7);
    Position.BlackPawn2=(Position.BlackPawn&D2);
    Position.BlackPawn7=(Position.BlackPawn&D7);
}

extern int DoublePawn;

void change_cell(Board &Position, int cell, int figure)
{
    int pr=Position.pos[cell];
    unsigned long long two=(1ULL<<cell);
    if (pr!=0)
    {
        if (pr==1)
        {
            Position.WhitePawn^=two;
        }
        if (pr==2)
        {
            Position.BlackPawn^=two;
        }
        if ((pr&1)==1) Position.AllWhite^=two;
        else Position.AllBlack^=two;
    }
    if (figure!=0)
    {
        if (figure==1)
        {
            Position.WhitePawn^=two;
        }
        if (figure==2)
        {
            Position.BlackPawn^=two;
        }
        if ((figure&1)==1) Position.AllWhite^=two;
        else Position.AllBlack^=two;
    }
    if (figure==0&&pr!=0)
    {
        Position.diagonal_1[(cell>>3)+(cell&7)]^=(1ULL<<DList[cell].f1);
        Position.diagonal_2[(cell>>3)-(cell&7)+7]^=(1ULL<<DList[cell].f2);
        Position.horisontal[cell>>3]^=(1ULL<<(cell&7));
        Position.vertical[cell&7]^=(1ULL<<(cell>>3));
    }
    else
    {
        if (pr==0&&figure!=0)
        {
            Position.diagonal_1[(cell>>3)+(cell&7)]|=(1ULL<<DList[cell].f1);
            Position.diagonal_2[(cell>>3)-(cell&7)+7]|=(1ULL<<DList[cell].f2);
            Position.horisontal[cell>>3]|=(1ULL<<(cell&7));
            Position.vertical[cell&7]|=(1ULL<<(cell>>3));
        }
    }
    Position.Zobrist^=ZobristHash[cell][pr];
    Position.Zobrist^=ZobristHash[cell][figure];
    if (pr>2)
    {
        for (int i=0; i<Position.Figures[pr].size; i++)
        {
            if (Position.Figures[pr].a[i]==cell)
            {
                std::swap(Position.Figures[pr].a[i],Position.Figures[pr].a[Position.Figures[pr].size-1]);
                Position.Figures[pr].size--;
                break;
            }
        }
    }
    if (figure>2)
    {
        Position.Figures[figure].a[Position.Figures[figure].size]=cell;
        Position.Figures[figure].size++;
    }
    Position.pos[cell]=figure;
    Position.Material-=Cost[pr];
    Position.Material+=Cost[figure];
    Position.Positional-=Cost_pos[cell][pr];
    Position.Positional+=Cost_pos[cell][figure];
}

MadeMove make_move(Board &Position, Move mv)
{
    MadeMove res;
    mv.src=__builtin_ctzll(mv.src);
    mv.dst=__builtin_ctzll(mv.dst);
    res.src_place=mv.src;
    res.src_piece=Position.pos[mv.src];
    res.dst_place=mv.dst;
    res.dst_piece=Position.pos[mv.dst];
    res.en_pass_w=Position.en_passing_line_w;
    res.en_pass_b=Position.en_passing_line_b;
    res.moves_now=Position.move_now;
    res.moves50=Position.moves50;
    res.enPass=false;
    res.castl=0;
    if (Position.QueenWhite) res.castl^=1;
    if (Position.KingWhite) res.castl^=2;
    if (Position.QueenBlack) res.castl^=4;
    if (Position.KingBlack) res.castl^=8;
    Position.Zobrist^=Zobrist_cstl[res.castl];
    if (Position.en_passing_line_w+Position.en_passing_line_b) Position.Zobrist^=Zobrist_pass[get_index(Position.en_passing_line_w+Position.en_passing_line_b)];
    Position.en_passing_line_w=0;
    Position.en_passing_line_b=0;
    Position.move_now++;
    if (mv.src==4||mv.dst==4)
    {
        Position.KingWhite=false;
        Position.QueenWhite=false;
    }
    if (mv.src==60||mv.dst==60)
    {
        Position.KingBlack=false;
        Position.QueenBlack=false;
    }
    if (mv.src==0||mv.dst==0)
    {
        Position.QueenWhite=false;
    }
    if (mv.src==7||mv.dst==7)
    {
        Position.KingWhite=false;
    }
    if (mv.src==56||mv.dst==56)
    {
        Position.QueenBlack=false;
    }
    if (mv.src==63||mv.dst==63)
    {
        Position.KingBlack=false;
    }
    // PATCHED
    if ((Position.pos[mv.src]<=2)&&(abs((int)mv.dst-(int)mv.src)==16))
    // END PATCHED
    {
        if (!Position.Move) Position.en_passing_line_w=(1ULL<<(16+(mv.src&7)));
        else Position.en_passing_line_b=(1ULL<<(40+(mv.src&7)));
    }
    if ((Position.Figures[11].a[0]==4&&(mv==Move{4,2,11}||mv==Move{4,6,11}))||(Position.Figures[12].a[0]==60&&
    (mv==Move{60,58,12}||mv==Move{60,62,12})))
    {
        res.cstl=true;
        if (mv==Move{4,2,11})
        {
            change_cell(Position, 4, 0);
            change_cell(Position, 3, 7);
            change_cell(Position, 2, 11);
            change_cell(Position, 0, 0);
            Position.KingWhite=false;
            Position.QueenWhite=false;
        }
        if (mv==Move{4,6,11})
        {
            change_cell(Position, 4, 0);
            change_cell(Position, 5, 7);
            change_cell(Position, 6, 11);
            change_cell(Position, 7, 0);
            Position.KingWhite=false;
            Position.QueenWhite=false;
        }
        if (mv==Move{60,58,12})
        {
            change_cell(Position, 60, 0);
            change_cell(Position, 59, 8);
            change_cell(Position, 58, 12);
            change_cell(Position, 56, 0);
            Position.KingBlack=false;
            Position.QueenBlack=false;
        }
        if (mv==Move{60,62,12})
        {
            change_cell(Position, 60, 0);
            change_cell(Position, 61, 8);
            change_cell(Position, 62, 12);
            change_cell(Position, 63, 0);
            Position.KingBlack=false;
            Position.QueenBlack=false;
        }
        Position.moves50++;
        Position.Move=(!Position.Move);
        Position.Zobrist^=ZobristMove;
        change_pawn(Position);
        Position.Zobrist^=Zobrist_cstl[(int)Position.QueenWhite+(int)Position.KingWhite*2+
        (int)Position.QueenBlack*4+(int)Position.KingBlack*8];
        if (Position.en_passing_line_w+Position.en_passing_line_b) Position.Zobrist^=Zobrist_pass[get_index(Position.en_passing_line_w+Position.en_passing_line_b)];
        Triple[Position.Zobrist&((1<<12)-1)].insert(Position.Zobrist);
        return res;
    }
    if ((Position.pos[mv.src]==1||Position.pos[mv.src]==2)&&((mv.src&7)!=(mv.dst&7))&&(Position.pos[mv.dst]==0))
    {
        res.enPass=true;
        change_cell(Position, mv.dst, Position.pos[mv.src]);
        if (!Position.Move) change_cell(Position, mv.dst-8, 0);
        else change_cell(Position, mv.dst+8, 0);
        change_cell(Position, mv.src, 0);
        Position.moves50=0;
        Position.Move=(!Position.Move);
        Position.Zobrist^=ZobristMove;
        change_pawn(Position);
        Position.Zobrist^=Zobrist_cstl[(int)Position.QueenWhite+(int)Position.KingWhite*2+
        (int)Position.QueenBlack*4+(int)Position.KingBlack*8];
        if (Position.en_passing_line_w+Position.en_passing_line_b) Position.Zobrist^=Zobrist_pass[get_index(Position.en_passing_line_w+Position.en_passing_line_b)];
        Triple[Position.Zobrist&((1<<12)-1)].insert(Position.Zobrist);
        return res;
    }
    if (Position.pos[mv.dst]==0) Position.moves50++;
    else Position.moves50=0;
    if (res.src_piece==1||res.src_piece==2) Position.moves50=0;
    change_cell(Position, mv.dst, mv.fgr);
    change_cell(Position, mv.src, 0);
    Position.Move=(!Position.Move);
    Position.Zobrist^=ZobristMove;
    change_pawn(Position);
    Position.Zobrist^=Zobrist_cstl[(int)Position.QueenWhite+(int)Position.KingWhite*2+
    (int)Position.QueenBlack*4+(int)Position.KingBlack*8];
    if (Position.en_passing_line_w+Position.en_passing_line_b) Position.Zobrist^=Zobrist_pass[get_index(Position.en_passing_line_w+Position.en_passing_line_b)];
    Triple[Position.Zobrist&((1<<12)-1)].insert(Position.Zobrist);
    return res;
}

void unmake_move(Board &Position, MadeMove mv)
{
    Triple[Position.Zobrist&((1<<12)-1)].erase(Position.Zobrist);
    Position.Zobrist^=Zobrist_cstl[(int)Position.QueenWhite+(int)Position.KingWhite*2+
    (int)Position.QueenBlack*4+(int)Position.KingBlack*8];
    if (Position.en_passing_line_w+Position.en_passing_line_b) Position.Zobrist^=Zobrist_pass[get_index(Position.en_passing_line_w+Position.en_passing_line_b)];
    Position.en_passing_line_w=mv.en_pass_w;
    Position.en_passing_line_b=mv.en_pass_b;
    Position.QueenWhite=(mv.castl&1);
    Position.KingWhite=(mv.castl&2);
    Position.QueenBlack=(mv.castl&4);
    Position.KingBlack=(mv.castl&8);
    Position.move_now=mv.moves_now;
    Position.moves50=mv.moves50;
    Position.Zobrist^=Zobrist_cstl[mv.castl];
    if (Position.en_passing_line_w+Position.en_passing_line_b) Position.Zobrist^=Zobrist_pass[get_index(Position.en_passing_line_w+Position.en_passing_line_b)];
    if (mv.cstl)
    {
        if (mv.src_place==4&&mv.dst_place==2&&mv.src_piece==11)
        {
            change_cell(Position, 4, 11);
            change_cell(Position, 3, 0);
            change_cell(Position, 2, 0);
            change_cell(Position, 0, 7);
        }
        if (mv.src_place==4&&mv.dst_place==6&&mv.src_piece==11)
        {
            change_cell(Position, 4, 11);
            change_cell(Position, 5, 0);
            change_cell(Position, 6, 0);
            change_cell(Position, 7, 7);
        }
        if (mv.src_place==60&&mv.dst_place==58&&mv.src_piece==12)
        {
            change_cell(Position, 60, 12);
            change_cell(Position, 59, 0);
            change_cell(Position, 58, 0);
            change_cell(Position, 56, 8);
        }
        if (mv.src_place==60&&mv.dst_place==62&&mv.src_piece==12)
        {
            change_cell(Position, 60, 12);
            change_cell(Position, 61, 0);
            change_cell(Position, 62, 0);
            change_cell(Position, 63, 8);
        }
        Position.Move=(!Position.Move);
        Position.Zobrist^=ZobristMove;
        change_pawn(Position);
        return;
    }
    if (mv.enPass)
    {
        change_cell(Position, mv.dst_place, 0);
        if (mv.src_piece==1) change_cell(Position, mv.dst_place-8, 2);
        else change_cell(Position, mv.dst_place+8, 1);
        change_cell(Position, mv.src_place, mv.src_piece);
        Position.Move=(!Position.Move);
        Position.Zobrist^=ZobristMove;
        change_pawn(Position);
        return;
    }
    change_cell(Position, mv.dst_place, mv.dst_piece);
    change_cell(Position, mv.src_place, mv.src_piece);
    Position.Move=(!Position.Move);
    Position.Zobrist^=ZobristMove;
    change_pawn(Position);
    return;
}
