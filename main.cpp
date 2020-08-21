#include "rules.h"
#include "analyze.h"
#include "position_cost.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;

mt19937_64 rand0(time(nullptr));

Board Position;

Board oldPosition;

string s0="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int wtime=0;
int btime=0;
int winc=0;
int binc=0;
int movestogo=0;
bool infinite=false;

int parse_int(string s)
{
    int ans=0;
    for (int i=0; i<(int)s.size(); i++)
    {
        ans*=10;
        ans+=(s[i]-'0');
    }
    return ans;
}

int move_time_uci=0, fixed_depth=0;

void parse(string t1, string t2)
{
    if (t1=="wtime") wtime=parse_int(t2);
    if (t1=="btime") btime=parse_int(t2);
    if (t1=="winc") winc=parse_int(t2);
    if (t1=="binc") binc=parse_int(t2);
    if (t1=="movestogo") movestogo=parse_int(t2);
    if (t1=="infinite") infinite=true;
    if (t1=="movetime")
    {
        infinite=false;
        move_time_uci=parse_int(t2);
    }
    if (t1=="depth")
    {
        infinite=true;
        fixed_depth=parse_int(t2);
    }
}

extern TripleHash Triple[(1<<12)];

extern int HList[100000];

inline int get_index(unsigned long long c)
{
    return __builtin_ctzll(c);
}

long long AllCount=0;

bool stop_it=false;

volatile bool is_analyzing=false;

string parse_move(Move mv)
{
    string ans="";
    ans.push_back(get_index(mv.src)%8+'a');
    ans.push_back(get_index(mv.src)/8+'1');
    ans.push_back(get_index(mv.dst)%8+'a');
    ans.push_back(get_index(mv.dst)/8+'1');
    if (Position.pos[get_index(mv.src)]!=mv.fgr)
    {
        if (mv.fgr==1) ans+="p";
        if (mv.fgr==2) ans+="p";
        if (mv.fgr==3) ans+="n";
        if (mv.fgr==4) ans+="n";
        if (mv.fgr==5) ans+="b";
        if (mv.fgr==6) ans+="b";
        if (mv.fgr==7) ans+="r";
        if (mv.fgr==8) ans+="r";
        if (mv.fgr==9) ans+="q";
        if (mv.fgr==10) ans+="q";
        if (mv.fgr==11) ans+="k";
        if (mv.fgr==12) ans+="k";
    }
    return ans;
}

Move bestmove;

extern HashData HashTable[(1<<22)];

int time_need;

bool startpos=false;

Board Pos_null;

unsigned long long e4=0, d4=0, nf3=0, c4=0, mnull=0, e2e4=0, e2e4nf3=0;

extern TripleHash SmallTriple[(1<<16)];

extern int Cost_pos[64][13];

extern string debug_info;

int MAX_N=0;

bool wcapture=false;

void Run_Analyze()
{
    is_analyzing=true;
    if (infinite==false&&move_time_uci==-1)
    {
        if (Position.Zobrist==mnull)
        {
            is_analyzing=false;
            int chance=rand0()%588000;
            if (chance<248000)
            {
                cout<<"bestmove e2e4"<<endl;
                return;
            }
            if (chance<248000+228000)
            {
                cout<<"bestmove d2d4"<<endl;
                return;
            }
            if (chance<248000+228000+67000)
            {
                cout<<"bestmove g1f3"<<endl;
                return;
            }
            cout<<"bestmove c2c4"<<endl;
            return;
        }
        if (Position.Zobrist==e4)
        {
            is_analyzing=false;
            int chance=rand0()%223000;
            if (chance<110000)
            {
                //cout<<"bestmove c7c5"<<endl;
                cout<<"bestmove e7e5"<<endl;
                return;
            }
            if (chance<110000+63000)
            {
                cout<<"bestmove e7e5"<<endl;
                return;
            }
            if (chance<110000+63000+30000)
            {
                cout<<"bestmove e7e6"<<endl;
                return;
            }
            //cout<<"bestmove c7c6"<<endl;
            cout<<"bestmove e7e6"<<endl;
            return;
        }
        if (Position.Zobrist==d4)
        {
            is_analyzing=false;
            int chance=rand0()%199000;
            if (chance<140000)
            {
                cout<<"bestmove g8f6"<<endl;
                return;
            }
            cout<<"bestmove d7d5"<<endl;
            return;
        }
        if (Position.Zobrist==nf3)
        {
            is_analyzing=false;
            int chance=rand0()%51000;
            if (chance<32000)
            {
                cout<<"bestmove g8f6"<<endl;
                return;
            }
            cout<<"bestmove d7d5"<<endl;
            return;
        }
        if (Position.Zobrist==c4)
        {
            is_analyzing=false;
            int chance=rand0()%41000;
            if (chance<12000)
            {
                cout<<"bestmove g8f6"<<endl;
                return;
            }
            if (chance<12000+9000)
            {
                cout<<"bestmove e7e5"<<endl;
                return;
            }
            if (chance<12000+9000+7000)
            {
                cout<<"bestmove e7e6"<<endl;
                return;
            }
            if (chance<12000+9000+7000+6000)
            {
                cout<<"bestmove c7c5"<<endl;
                return;
            }
            if (chance<12000+9000+7000+6000+4000)
            {
                //cout<<"bestmove c7c6"<<endl;
                cout<<"bestmove e7e5"<<endl;
                return;
            }
            cout<<"bestmove g8f6"<<endl;
            return;
        }
        if (Position.Zobrist==e2e4)
        {
            is_analyzing=false;
            cout<<"bestmove g1f3"<<endl;
            return;
        }
        if (Position.Zobrist==e2e4nf3)
        {
            is_analyzing=false;
            int chance=rand0()%3;
            if (chance==0) cout<<"bestmove g8f6"<<endl;
            else cout<<"bestmove b8c6"<<endl;
            return;
        }
    }
    auto Start0=steady_clock().now();
    time_need=0;
    int prev_depth=0;
    if (infinite==false)
    {
        if (startpos)
        {
            if (Position.move_now<=20)
            {
                if (!Position.Move) time_need=wtime/40;
                else time_need=btime/40;
            }
            else
            if (Position.move_now<=40)
            {
                if (!Position.Move) time_need=wtime/20;
                else time_need=btime/20;
            }
            else
            {
                if (!Position.Move) time_need=wtime/20;
                else time_need=btime/20;
            }
        }
        else
        {
            if (!Position.Move) time_need=wtime/25;
            else time_need=btime/25;
        }
        if (Position.move_now>60)
        {
            if (!Position.Move) time_need+=wtime/50;
            else time_need+=btime/50;
        }
        if (startpos==false||Position.move_now>10)
        {
            if (!Position.Move)
            {
                if (wtime<=1000*60) time_need=min(time_need,wtime/30);
                if (wtime<=100*60) time_need=min(time_need,wtime/50);
            }
            else
            {
                if (btime<=1000*60) time_need=min(time_need,btime/30);
                if (btime<=100*60) time_need=min(time_need,btime/50);
            }
        }
        if (!Position.Move) time_need+=winc;
        else time_need+=binc;
        if (!Position.Move) time_need=min(time_need,wtime-1);
        else time_need=min(time_need,btime-1);
    }
    if (move_time_uci>=0) time_need=move_time_uci;
    for (int i=0; i<(1<<16); i++)
        SmallTriple[i].size=0;
    Move first_search_move={0,0,0};
    MList AllMoves;
    generate_captures(Position, AllMoves);
    generate_moves(Position, AllMoves);
    bool got_move=false;
    for (int i=1; i<=99; i++)
    {
        MAX_N=i+10;
        MList chain;
        AllCount=0;
        auto start=steady_clock().now();
        int ans=Main_Analyze(Position, -30000, 30000, i*4, chain, i*4, first_search_move, 0, 0, false, false, wcapture, infinite, Start0);
        if (stop_it) break;
        got_move=true;
        bestmove=chain[chain.size-1];
        auto finish=steady_clock().now();
        prev_depth=duration_cast<milliseconds>(finish-Start0).count();
        cout<<"info depth "<<i<<" time "<<duration_cast<milliseconds>(finish-Start0).count()
        <<" nodes "<<AllCount<<" score cp "<<ans<<" pv ";
        /*if (chain.size==0)
        {
            cout<<"info depth 1 time 1 nodes 1 score cp 0 pv"<<debug_info<<'\n';
        }*/
        MadeMove u0[chain.size];
        for (int i=chain.size-1; i>=0; i--)
        {
            cout<<parse_move(chain[i])<<" ";
            u0[i]=make_move(Position, chain[i]);
        }
        first_search_move=chain[chain.size-1];
        cout<<endl;
        for (int i=0; i<chain.size; i++)
        {
            unmake_move(Position, u0[i]);
        }
        Position=oldPosition;
        if (move_time_uci==-1)
        {
            if (infinite==false&&prev_depth*2>=time_need) break;
            if (infinite==false&&wcapture==true&&prev_depth*3>=time_need) break;
        }
        if (i==fixed_depth) break;
    }
    if (!got_move) bestmove=AllMoves[0];
    Position=oldPosition;
    cout<<"bestmove "<<parse_move(bestmove)<<endl;
    is_analyzing=false;
}

string words[100000];

int main()
{
    cout<<"Seagull 2.2"<<endl;
    cout<<"A free UCI chess engine for playing rapid and classical chess. Also can be used for analyzing real-played matches."<<endl;
    cout<<"What is new in this version:"<<endl;
    cout<<"* Fixed futility pruning and limited razoring."<<endl;
    cout<<"* Advanced position cost."<<endl;
    cout<<"* Added \"go depth\" and \"go movetime\" commands."<<endl;
    cout<<"* Author: Wind_Eagle. Build date: 21.08.2020"<<endl;
    cout<<endl;
    cout<<"From the flight of the seagull"<<endl;
    cout<<"Come the spread claws of the eagle."<<endl;
    cout<<"-- ELP, song \"knife-edge\"."<<endl;
    initialize();
    initialize_position_cost();
    cout<<endl;
    cout<<"Engine is ready to work."<<endl;
    string FEN;
    FEN="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    parse_from_FEN(FEN, Pos_null);
    mnull=Pos_null.Zobrist;
    FEN="rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    parse_from_FEN(FEN, Pos_null);
    e4=Pos_null.Zobrist;
    FEN="rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1";
    parse_from_FEN(FEN, Pos_null);
    d4=Pos_null.Zobrist;
    FEN="rnbqkbnr/pppppppp/8/8/8/5N2/PPPPPPPP/RNBQKB1R b KQkq - 1 1";
    parse_from_FEN(FEN, Pos_null);
    nf3=Pos_null.Zobrist;
    FEN="rnbqkbnr/pppppppp/8/8/2P5/8/PP1PPPPP/RNBQKBNR b KQkq c3 0 1";
    parse_from_FEN(FEN, Pos_null);
    c4=Pos_null.Zobrist;
    FEN="rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2";
    parse_from_FEN(FEN, Pos_null);
    e2e4=Pos_null.Zobrist;
    FEN="rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
    parse_from_FEN(FEN, Pos_null);
    e2e4nf3=Pos_null.Zobrist;
    /*string input=s0;
    parse_from_FEN(input, Position);
    MList chain;
    for (int i=1; i<=9; i++)
    {
        cout<<"Analyze for i = "<<i<<" is: "<<Main_Analyze(Position, -30000, 30000, i, chain)<<endl;
        cout<<AllCount<<endl;
        for (int i=chain.size-1; i>=0; i--)
        {
            cout<<get_index(chain[i].src)<<" "<<get_index(chain[i].dst)<<" "<<chain[i].fgr<<endl;
        }
        chain.size=0;
        AllCount=0;
        cout<<endl;
    }*/
    string UCI_input;
    while (true)
    {
        getline(cin,UCI_input);
        if (UCI_input=="uci")
        {
            cout<<"id name Seagull 2.2 - Shiny Jolteon Edition"<<endl;
            cout<<"id author Wind_Eagle"<<endl;
            cout<<"uciok"<<endl;
            continue;
        }
        if (UCI_input=="isready")
        {
            cout<<"readyok"<<endl;
            continue;
        }
        if (UCI_input=="stop")
        {
            stop_it=true;
            do
            {

            }
            while (is_analyzing);
            continue;
        }
        if (UCI_input=="quit")
        {
            stop_it=true;
            do
            {

            }
            while (is_analyzing);
            return 0;
        }
        int siz=0;
        int pos=0;
        string cur="";
        while (pos<(int)UCI_input.size())
        {
            if (UCI_input[pos]==' ')
            {
                if (cur.size()>0)
                {
                    words[siz]=cur;
                    siz++;
                    cur="";
                }
                pos++;
                continue;
            }
            cur+=UCI_input[pos];
            pos++;
        }
        if (cur.size()>0)
        {
            words[siz]=cur;
            siz++;
            cur="";
        }
        if (words[0]=="position")
        {
            wcapture=false;
            string cur_FEN="";
            int pos_moves=0;
            if (words[1]=="startpos") cur_FEN="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
            else
            {
                for (int i=2; i<siz; i++)
                {
                    if (words[i]=="moves")
                    {
                        pos_moves=i;
                        break;
                    }
                    else cur_FEN+=words[i];
                    cur_FEN+=" ";
                }
            }
            for (int i=2; i<siz; i++)
            {
                if (words[i]=="moves")
                {
                    pos_moves=i;
                    break;
                }
            }
            parse_from_FEN(cur_FEN, Position);
            for (int i=0; i<(1<<12); i++)
            {
                Triple[i].size=0;
            }
            Triple[Position.Zobrist&((1<<12)-1)].insert(Position.Zobrist);
            if (words[pos_moves]=="moves")
            {
                for (int i=pos_moves+1; i<siz; i++)
                {
                    Move mv;
                    int src=0;
                    int dst=0;
                    int fgr=0;
                    src=((words[i][1]-'1')*8)+(words[i][0]-'a');
                    dst=((words[i][3]-'1')*8)+(words[i][2]-'a');
                    fgr=Position.pos[src];
                    if (words[i].size()==5)
                    {
                        if (words[i][4]=='p') if (fgr%2==1) fgr=1; else fgr=2;
                        if (words[i][4]=='n') if (fgr%2==1) fgr=3; else fgr=4;
                        if (words[i][4]=='b') if (fgr%2==1) fgr=5; else fgr=6;
                        if (words[i][4]=='r') if (fgr%2==1) fgr=7; else fgr=8;
                        if (words[i][4]=='q') if (fgr%2==1) fgr=9; else fgr=10;
                        if (words[i][4]=='k') if (fgr%2==1) fgr=11; else fgr=12;
                    }
                    mv=Move{(1ULL<<src),(1ULL<<dst),fgr};
                    int q1=Position.Material;
                    make_move(Position, mv);
                    int q2=Position.Material;
                    if (q1!=q2)
                    {
                        if (wcapture) wcapture=false;
                        else wcapture=true;
                    }
                    else
                    {
                        wcapture=false;
                    }
                }
            }
            oldPosition=Position;
            if (cur_FEN=="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") startpos=true;
            else startpos=false;
            /*if (HashTable[Position.Zobrist&((1<<22)-1)].FullHash!=Position.Zobrist)
            {
                for (int o=0; o<(1<<22); o++)
                {
                    HashTable[o].depth=-1;
                }
            }*/
        }
        if (words[0]=="go")
        {
            stop_it=false;
            wtime=0;
            btime=0;
            winc=0;
            binc=0;
            movestogo=0;
            move_time_uci=-1;
            fixed_depth=-1;
            infinite=false;
            int pos=1;
            while (pos<siz)
            {
                parse(words[pos], words[pos+1]);
                if (words[pos]=="infinite") pos++;
                else pos+=2;
            }
            thread t(Run_Analyze);
            t.detach();
        }
    }
}

