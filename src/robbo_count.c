/*******************************************************************************
Firenzina is a UCI chess playing engine by
Kranium (Norman Schmidt), Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
Rededication: To the memories of Giovanna Tornabuoni and Domenico Ghirlandaio.
Special thanks to: Norman Schmidt, Jose Maria Velasco, Jim Ablett, Jon Dart, Andrey Chilantiev, Quoc Vuong.
Firenzina is a clone of Fire 2.2 xTreme by Kranium (Norman Schmidt). 
Firenzina is a derivative (via Fire) of FireBird by Kranium (Norman Schmidt) 
and Sentinel (Milos Stanisavljevic). Firenzina is based (via Fire and FireBird)
on Ippolit source code: http://ippolit.wikispaces.com/
Ippolit authors: Yakov Petrovich Golyadkin, Igor Igorovich Igoronov,
and Roberto Pescatore 
Ippolit copyright: (C) 2009 Yakov Petrovich Golyadkin
Ippolit date: 92th and 93rd year from Revolution
Ippolit owners: PUBLICDOMAIN (workers)
Ippolit dedication: To Vladimir Ilyich
"This Russian chess ship is a truly glorious achievement of the
 October Revolution and Decembrists movement!"

Firenzina is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Firenzina is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see http://www.gnu.org/licenses/.
*******************************************************************************/

#ifndef robbo_count
#define robbo_count
#include "fire.h"
#include "robbo_count.c"
#include "white.h"
#else
#include "black.h"
#endif

static int CountEvasions(typePos *Position, uint64 *ep)
    {
    int king, count, pi, sq;
    uint64 att, ip, T, flee, U;
    *ep = 0;
    king = MyKingSq;
    att = MyKingCheck;
    sq = BSF(att);
    pi = Position->sq[sq];
    flee = (~OppAttacked) &(((pi == EnumOppP || pi == EnumOppK) ? AttK[king] : 0)
       | Evade(king, sq)) & (~Position->OccupiedBW);
    BitClear(sq, att);
    if (att)
        {
        sq = BSF(att);
        pi = Position->sq[sq];
        BitClear(sq, att);
        if (att)
            {
            *ep = -1;
            return 0;
            }
        return POPCNT(flee &(((pi == EnumOppP || pi == EnumOppK) ? AttK[king] : 0) | Evade(king, sq)));
        }
    count = POPCNT(flee);
    ip = InterPose(king, sq) & ~Position->OccupiedBW;
    if (!ip)
        return count;
    T = BitboardMyP & ~BitBoardSeventhRank & BackShift((ip & OppOccupied) ^ ip) & ~OppXray;
    count += POPCNT(T);
    T =
       BitboardMyP & BackShift2((ip & OppOccupied) ^ ip) & SecondRank & BackShift(~Position->OccupiedBW) & ~OppXray;
    T = ForwardShift(ForwardShift(T));
    count += POPCNT(T);
    *ep = T &(((BitboardOppP & FourthRankBitBoard) << 1) | ((BitboardOppP & FourthRankBitBoard) >> 1));
    for (U = BitboardMyN & ~OppXray; U; BitClear(sq, U))
        count += POPCNT(AttN[BSF(U)] & ip);
    for (U = BitboardMyB & ~OppXray; U; BitClear(sq, U))
        count += POPCNT(AttB(BSF(U)) & ip);
    for (U = BitboardMyR & ~OppXray; U; BitClear(sq, U))
        count += POPCNT(AttR(BSF(U)) & ip);
    for (U = BitboardMyQ & ~OppXray; U; BitClear(sq, U))
        count += POPCNT(AttQ(BSF(U)) & ip);
    return count;
    }
int MyCount(typePos *Position, uint64 *ep)
    {
    uint64 empty = ~Position->OccupiedBW, U, A, T;
    int b, count = 0;
    Mobility(Position);
    if (InCheck)
        return CountEvasions(Position, ep);
    for (U = BitboardMyN; U; BitClear(b, U))
        {
        b = BSF(U);
        if (OppXray & SqSet[b])
            continue;
        A = AttN[b] & empty;
        count += POPCNT(A);
        }
    for (U = BitboardMyB; U; BitClear(b, U))
        {
        b = BSF(U);
        A = AttB(b) & empty;
        if (OppXray & SqSet[b])
            A &= InterPose(MyKingSq, OppXrayTable[b]);
        count += POPCNT(A);
        }
    for (U = BitboardMyR; U; BitClear(b, U))
        {
        b = BSF(U);
        A = AttR(b) & empty;
        if (OppXray & SqSet[b])
            A &= InterPose(MyKingSq, OppXrayTable[b]);
        count += POPCNT(A);
        }
    for (U = BitboardMyQ; U; BitClear(b, U))
        {
        b = BSF(U);
        A = AttQ(b) & empty;
        if (OppXray & SqSet[b])
            A &= InterPose(MyKingSq, OppXrayTable[b]);
        count += POPCNT(A);
        }
    count += POPCNT(AttK[MyKingSq] & empty & ~OppAttacked);
    count +=
       POPCNT(BitboardMyP &(~BitBoardSeventhRank) & BackShift(empty) & (FileArray[File(MyKingSq)] | ~OppXray));
    T = BitboardMyP & SecondRank & BackShift(empty) & BackShift2(empty) & (FileArray[File(MyKingSq)] | ~OppXray);
    T = ForwardShift(ForwardShift(T));
    count += POPCNT(T);
    *ep = T &(((BitboardOppP & FourthRankBitBoard) << 1) | ((BitboardOppP & FourthRankBitBoard) >> 1));
    return count;
    }
