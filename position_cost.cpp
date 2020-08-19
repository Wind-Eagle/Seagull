#include "rules.h"
#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <algorithm>
#include <cmath>

using std::cout;
using std::endl;

int Cost_pos[64][13];

int Dist[64][64];

unsigned long long Ea=0, Eh=0;

unsigned long long Vertical0[8], Horizontal0[8];

unsigned long long nvert[8];

unsigned long long upper_hor_white[8];

unsigned long long upper_hor_black[8];

unsigned long long white_half=0;

unsigned long long black_half=0;

int Pawn0[64]=
{
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 5, 5, -20, -20, 5, 5, 5,
    0, -5, -5, 0, 0, -5, -5, 0,
    0, 0, 0, 20, 20, 0, 0, 0,
    15, 15, 15, 25, 25, 15, 15, 15,
    30, 30, 30, 50, 50, 30, 30, 30,
    70, 70, 70, 70, 70, 70, 70, 70,
    0, 0, 0, 0, 0, 0, 0, 0
};

int Knight0[64]=
{
    -50, -40, -30, -20, -20, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -20, 0, 15, 20, 20, 15, 0, -20,
    -20, 0, 15, 20, 20, 15, 0, -20,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -50, -40, -30, -20, -20, -30, -40, -50
};

int Bishop0[64]=
{
    0, -10, -10, -10, -10, -10, -10, 0,
    -10, 10, 0, 0, 0, 0, 10, -10,
    -10, 5, 10, 10, 10, 10, 5, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 0, 10, 5, 5, 10, 0, -10,
    -10, 10, 0, 0, 0, 0, 10, -10,
    0, -10, -10, -10, -10, -10, -10, 0
};

int Rook0[64]=
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    25, 25, 25, 25, 25, 25, 25, 25
};

int Queen0[64]=
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    25, 25, 25, 25, 25, 25, 25, 25
};

int King0[64]=
{
    20, 30, 0, -10, -10, 0, 30, 20,
    10, 20, -5, -10, -10, -5, 20, 10,
    -10, -20, -30, -30, -30, -30, -20, -10,
    -30, -40, -50, -50, -50, -50, -40, -30,
    -30, -40, -50, -50, -50, -50, -40, -30,
    -30, -40, -50, -50, -50, -50, -40, -30,
    -30, -40, -50, -50, -50, -50, -40, -30,
    -30, -40, -50, -50, -50, -50, -40, -30,
};

int King1[64]=
{
    -50, -30, -30, -30, -30, -30, -30, -50,
    -30, -20, -10, 0, 0, -10, -20, -30,
    -30, 0, 10, 20, 20, 10, 0, -30,
    -30, 0, 10, 25, 25, 10, 0, -30,
    -30, 0, 10, 25, 25, 10, 0, -30,
    -30, 0, 10, 20, 20, 10, 0, -30,
    -30, -20, -10, 0, 0, -10, -20, -30,
    -50, -30, -30, -30, -30, -30, -30, -50
};

unsigned long long BadBishop[64];

void initialize_position_cost()
{
    for (int i=0; i<64; i++)
    {
        Cost_pos[i][1]=Pawn0[i];
        Cost_pos[63-i][2]=-Pawn0[i];
    }
    for (int i=0; i<64; i++)
    {
        Cost_pos[i][3]=Knight0[i];
        Cost_pos[63-i][4]=-Knight0[i];
    }
    for (int i=0; i<64; i++)
    {
        Cost_pos[i][5]=Bishop0[i];
        Cost_pos[63-i][6]=-Bishop0[i];
    }
    for (int i=0; i<64; i++)
    {
        Cost_pos[i][7]=Rook0[i];
        Cost_pos[63-i][8]=-Rook0[i];
    }
    for (int i=0; i<64; i++)
    {
        Cost_pos[i][9]=Queen0[i];
        Cost_pos[63-i][10]=-Queen0[i];
    }
    for (int i=0; i<64; i++)
    {
        Cost_pos[i][11]=King0[i];
        Cost_pos[63-i][12]=-King0[i];
    }
    for (int i=0; i<64; i++)
    {
        if (i%8!=0) Ea|=(1ULL<<i);
        if (i%8!=7) Eh|=(1ULL<<i);
        Vertical0[i%8]|=(1ULL<<i);
        Horizontal0[i/8]|=(1ULL<<i);
    }
    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            nvert[j]^=(1ULL<<(i*8+j));
            if (j>0) nvert[j-1]^=(1ULL<<(i*8+j));
            if (j<7) nvert[j+1]^=(1ULL<<(i*8+j));
            if (i<4) white_half^=(1ULL<<(i*8+j));
            else black_half^=(1ULL<<(i*8+j));
        }
    }
    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            for (int i0=0; i0<8; i0++)
            {
                for (int j0=0; j0<8; j0++)
                {
                    Dist[i*8+j][i0*8+j0]=round(sqrt(abs(i-i0)*abs(i-i0)+abs(j-j0)*abs(j-j0)));
                    if (abs(i-i0)<=2&&abs(j-j0)<=2&&(i*8+j)%2==(i0*8+j0)%2) BadBishop[i*8+j]^=(1ULL<<(i0*8+j0));
                    if (i0>i) upper_hor_white[i]^=(1ULL<<(i0*8+j0));
                    if (i0<i) upper_hor_black[i]^=(1ULL<<(i0*8+j0));
                }
            }
        }
    }
}

inline int get_index(unsigned long long c)
{
    return __builtin_ctzll(c);
}

int BishopPair=30;
int RookPair=-20;
int KnightPair=-30;
int NoPawn=-100;

const int KnightWeakness[9]={-100,-50,-25,-10,0,0,0,0,0};

inline int get_material_cost(const Board &Position, int alpha, int beta)
{
    int cost=Position.Material;
    if (Position.Figures[5].size>=2) cost+=BishopPair;
    if (Position.Figures[6].size>=2) cost-=BishopPair;
    if (Position.Figures[7].size>=2) cost+=RookPair;
    if (Position.Figures[8].size>=2) cost-=RookPair;
    if (Position.Figures[5].size==0&&Position.Figures[3].size>=2) cost+=KnightPair;
    if (Position.Figures[6].size==0&&Position.Figures[4].size>=2) cost-=KnightPair;
    if (__builtin_popcountll(Position.WhitePawn)==0) cost+=NoPawn;
    if (__builtin_popcountll(Position.BlackPawn)==0) cost-=NoPawn;
    cost+=Position.Figures[3].size*KnightWeakness[__builtin_popcountll(Position.BlackPawn)];
    cost-=Position.Figures[4].size*KnightWeakness[__builtin_popcountll(Position.WhitePawn)];
    return cost;
}

const int dst_cost[15]={100,50,25,15,10,7,5,2,0,0,0,0,0,0,0};

const int pawn_def[8]={0,0,0,5,10,15,25,0};

const int PassedPawn[8]={0,0,0,5,15,30,50,0};

int DoublePawn=-20;
int IsolatedPawn=-20;
int BadBishopPawn=0;
int KnightOutpost=20;
int RookOpenFile=25;
int RookSemiOpenFile=10;
int RookQueen=10;
int KingOpenFile=-50;
int KingSemiOpenFile=-15;
int KingPawn=20;
int MovePriority=20;
int PawnStorm[9]={0,-10,-30,-60,-100,-100,-100,-100,-100};

inline int get_positional_cost(const Board &Position, int alpha, int beta)
{
    int cost=Position.Positional;
    //pawns
    unsigned long long Pawn=Position.WhitePawn;
    while (Pawn)
    {
        unsigned long long mv=(Pawn & -Pawn);
        Pawn^=mv;
        mv=get_index(mv);
        if (__builtin_popcountll(Position.WhitePawn&Vertical0[mv&7])>1)
        {
            cost+=DoublePawn;
        }
        if ((Position.BlackPawn&nvert[mv&7]&upper_hor_white[mv>>3])==0)
        {
            cost+=PassedPawn[mv>>3];
        }
        if (((Position.WhitePawn&nvert[mv&7])==0)&&((Position.BlackPawn&Vertical0[mv&7])!=0)) cost+=IsolatedPawn;
        if (Position.pos[mv-7]==1||Position.pos[mv-9]==1) cost+=pawn_def[mv>>3];
    }
    Pawn=Position.BlackPawn;
    while (Pawn)
    {
        unsigned long long mv=(Pawn & -Pawn);
        Pawn^=mv;
        mv=get_index(mv);
        if (__builtin_popcountll(Position.BlackPawn&Vertical0[mv&7])>1)
        {
            cost-=DoublePawn;
        }
        if ((Position.WhitePawn&Vertical0[mv&7]&upper_hor_black[mv>>3])==0)
        {
            cost-=PassedPawn[7-(mv>>3)];
        }
        if (((Position.BlackPawn&nvert[mv&7])==0)&&((Position.WhitePawn&Vertical0[mv&7])!=0)) cost-=IsolatedPawn;
        if (Position.pos[mv+7]==2||Position.pos[mv+9]==2) cost-=pawn_def[7-(mv>>3)];
    }
    //pawn based evaluation
    for (int i=0; i<Position.Figures[5].size; i++)
    {
        cost+=__builtin_popcountll(BadBishop[Position.Figures[5].a[i]]&Position.WhitePawn)*BadBishopPawn;
    }
    for (int i=0; i<Position.Figures[6].size; i++)
    {
        cost-=__builtin_popcountll(BadBishop[Position.Figures[6].a[i]]&Position.BlackPawn)*BadBishopPawn;
    }
    for (int i=0; i<Position.Figures[3].size; i++)
    {
        if (Position.Figures[3].a[i]==27||Position.Figures[3].a[i]==28||Position.Figures[3].a[i]==35||Position.Figures[3].a[i]==36)
        {
            if (Position.pos[Position.Figures[3].a[i]-7]==1||Position.pos[Position.Figures[3].a[i]-9]==1) cost+=KnightOutpost;
        }
    }
    for (int i=0; i<Position.Figures[4].size; i++)
    {
        if (Position.Figures[4].a[i]==27||Position.Figures[4].a[i]==28||Position.Figures[4].a[i]==35||Position.Figures[4].a[i]==36)
        {
            if (Position.pos[Position.Figures[4].a[i]+7]==2||Position.pos[Position.Figures[4].a[i]+9]==2) cost-=KnightOutpost;
        }
    }
    for (int i=0; i<Position.Figures[7].size; i++)
    {
        if ((Vertical0[Position.Figures[7].a[i]>>3]&Position.WhitePawn)==0)
        {
            if ((Vertical0[Position.Figures[7].a[i]>>3]&Position.BlackPawn)==0)
            {
                cost+=RookOpenFile;
            }
            else
            {
                cost+=RookSemiOpenFile;
            }
        }
    }
    for (int i=0; i<Position.Figures[8].size; i++)
    {
        if ((Vertical0[Position.Figures[8].a[i]>>3]&Position.BlackPawn)==0)
        {
            if ((Vertical0[Position.Figures[8].a[i]>>3]&Position.WhitePawn)==0)
            {
                cost-=RookOpenFile;
            }
            else
            {
                cost-=RookSemiOpenFile;
            }
        }
    }
    //knight cost
    //rook cost
    for (int i=0; i<Position.Figures[7].size; i++)
    {
        if ((Position.Figures[10].a[0]&7)==(Position.Figures[7].a[i]&7)) cost+=RookQueen;
        if ((Vertical0[Position.Figures[7].a[i]>>3]&Position.WhitePawn)==0)
        {
            if (abs((int)(Position.Figures[7].a[i]&7)-(int)(Position.Figures[12].a[0]&7))<=1)
            {
                if ((Vertical0[Position.Figures[7].a[i]>>3]&Position.BlackPawn)==0) cost-=KingOpenFile;
                cost-=KingSemiOpenFile;
            }
        }
    }
    for (int i=0; i<Position.Figures[8].size; i++)
    {
        if ((Position.Figures[9].a[0]&7)==(Position.Figures[8].a[i]&7)) cost-=RookQueen;
        if ((Vertical0[Position.Figures[8].a[i]>>3]&Position.BlackPawn)==0)
        {
            if (abs((int)(Position.Figures[8].a[i]&7)-(int)(Position.Figures[11].a[0]&7))<=1)
            {
                if ((Vertical0[Position.Figures[8].a[i]>>3]&Position.WhitePawn)==0) cost+=KingOpenFile;
                else cost+=KingSemiOpenFile;
            }
        }
    }
    //queen cost
    for (int i=0; i<Position.Figures[9].size; i++)
    {
        cost+=dst_cost[Dist[Position.Figures[9].a[i]][Position.Figures[12].a[0]]];
        cost+=Cost_pos[Position.Figures[9].a[i]][7];
    }
    for (int i=0; i<Position.Figures[10].size; i++)
    {
        cost-=dst_cost[Dist[Position.Figures[10].a[i]][Position.Figures[11].a[0]]];
        cost-=Cost_pos[Position.Figures[10].a[i]][8];
    }
    //start of new
    for (int i=0; i<Position.Figures[9].size; i++)
    {
        if ((Vertical0[Position.Figures[9].a[i]>>3]&Position.WhitePawn)==0)
        {
            if (abs((int)(Position.Figures[9].a[i]&7)-(int)(Position.Figures[12].a[0]&7))<=1)
            {
                if ((Vertical0[Position.Figures[9].a[i]>>3]&Position.BlackPawn)==0) cost-=KingOpenFile;
                cost-=KingSemiOpenFile;
            }
        }
    }
    for (int i=0; i<Position.Figures[10].size; i++)
    {
        if ((Vertical0[Position.Figures[10].a[i]>>3]&Position.BlackPawn)==0)
        {
            if (abs((int)(Position.Figures[10].a[i]&7)-(int)(Position.Figures[11].a[0]&7))<=1)
            {
                if ((Vertical0[Position.Figures[10].a[i]>>3]&Position.WhitePawn)==0) cost+=KingOpenFile;
                else cost+=KingSemiOpenFile;
            }
        }
    }
    //end of new
    //king cost
    unsigned long long WhiteKing=(King[Position.Figures[11].a[0]]&Position.WhitePawn);
    if (Position.Figures[11].a[0]<48&&Position.pos[Position.Figures[11].a[0]+16]==1) cost+=KingPawn/2;  //new
    unsigned long long BlackKing=(King[Position.Figures[12].a[0]]&Position.BlackPawn);
    if (Position.Figures[11].a[0]>=16&&Position.pos[Position.Figures[12].a[0]-16]==2) cost-=KingPawn/2;  //new
    if (Position.KingWhite==false&&Position.QueenWhite==false&&Position.Figures[10].size>0&&Position.Figures[11].a[0]<=7)
    {
        cost+=__builtin_popcountll(WhiteKing)*KingPawn;
        unsigned long long enemyPawn=(nvert[Position.Figures[11].a[0]&7]&Position.BlackPawn&white_half);
        cost+=PawnStorm[__builtin_popcountll(enemyPawn)];
    }
    if (Position.KingBlack==false&&Position.QueenBlack==false&&Position.Figures[9].size>0&&Position.Figures[12].a[0]>=56)
    {
        cost-=__builtin_popcountll(BlackKing)*KingPawn;
        unsigned long long enemyPawn=(nvert[Position.Figures[12].a[0]&7]&Position.BlackPawn&black_half);
        cost-=PawnStorm[__builtin_popcountll(enemyPawn)];
    };
    if (Position.Figures[9].size+Position.Figures[10].size==0)
    {
        cost-=King0[Position.Figures[11].a[0]];
        cost+=King1[Position.Figures[11].a[0]];
        cost+=King0[Position.Figures[12].a[0]];
        cost-=King1[Position.Figures[12].a[0]];
    }
    return cost;
}

int PosCost(const Board &Position, int alpha, int beta)
{
    if (__builtin_popcountll(Position.AllBlack|Position.AllWhite)==2) return 0;
    if (__builtin_popcountll(Position.AllBlack|Position.AllWhite)==3)
    {
        if (Position.Figures[3].size>0||Position.Figures[4].size>0||Position.Figures[5].size>0||Position.Figures[6].size>0) return 0;
    }
    if (__builtin_popcountll(Position.AllBlack|Position.AllWhite)==4)
    {
        if (Position.Figures[3].size==2||Position.Figures[4].size==2) return 0;
    }
    int cost=get_material_cost(Position, alpha, beta);
    if (Position.Move==false)
    {
        if (cost<alpha-300||cost>beta+300) return cost;
    }
    else
    {
        if ((-cost)<alpha-300||(-cost)>beta+300) return (-cost);
    }
    cost+=get_positional_cost(Position, alpha, beta);
    /*if (!Position.Move) cost+=MovePriority;
    else cost-=MovePriority;*/
    if (Position.Move) cost*=-1;
    return cost;
}

