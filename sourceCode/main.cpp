#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

// Chess libraries, and utils
#include "thc.h"
#include "bookMoves.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

struct hashEntry{
    int depth;
    short eval;
};

const unsigned long long amountOfRows = 720000000;
std::unordered_map<long long, hashEntry> transpositionTable;

void storeHashEntry(thc::ChessRules& cr, int depth, short eval){
    uint64_t hash = cr.HashCalculate();
    hash = hash/6;
    hashEntry entry = {depth, eval};
    transpositionTable[hash] = entry;
}

short getEvalFromHash(long long& hash, int minDepth){
    hashEntry result = transpositionTable[hash];
    if (result.depth >= minDepth && result.depth > 4){
        return result.eval;
    }
    return 404;
}

short evaluateMaterial(const thc::ChessRules& board) {
    short score = 0;
    static const std::unordered_map<char, short> pieceValues = {
            {'P', 1}, {'N', 3}, {'B', 3}, {'R', 5}, {'Q', 9},
            {'p', -1}, {'n', -3}, {'b', -3}, {'r', -5}, {'q', -9}
    };

    for (int i = 0; i < 64; i++) {
        auto square = static_cast<thc::Square>(i);
        char piece = board.squares[square];
        auto it = pieceValues.find(piece);
        if (it != pieceValues.end()) {
            score += it->second;
        }
    }

    return score;
}

void DisplayPosition( thc::ChessRules &cr)
{
    std::string s = cr.ToDebugStr();
    std::cout << s.c_str() << std::endl;
}

std::vector<thc::Move> GetLegalMoves( thc::ChessRules &cr )
{
    std::vector<thc::Move> moves, checks, captures, normal;
    std::vector<bool> check, mate, stalemate;
    cr.GenLegalMoveList(  moves, check, mate, stalemate );
    for (int i = 0; i < moves.size(); i++) {
        if (check[i]) {
            checks.push_back(moves[i]);
        } else if (cr.squares[moves[i].dst] != ' ') {
            captures.push_back(moves[i]);
        } else {
            normal.push_back(moves[i]);
        }
    }
    moves.clear();
    moves.insert(moves.end(), checks.begin(), checks.end());
    moves.insert(moves.end(), captures.begin(), captures.end());
    moves.insert(moves.end(), normal.begin(), normal.end());
    return moves;
}

short alphaBeta(thc::ChessRules &cr, int depth, short alpha, short beta, bool maximizingPlayer){
    if (depth == 0){
        return evaluateMaterial(cr);
    }

    if (maximizingPlayer) {
        short maxEval = -1000;
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            long long hash = cr_copy.HashCalculate();
            hash = hash/6;

            short eval = getEvalFromHash(hash, depth - 1);
            if (eval==404){
                eval = alphaBeta(cr_copy, depth - 1, alpha, beta, false);
                storeHashEntry(cr_copy, depth - 1, eval);
            }

            maxEval = std::max<short>(maxEval, eval);
            alpha = std::max<short>(alpha, eval);

            if (beta <= alpha){
                break;
            }

        }
        return maxEval;
    } else {
        short minEval = 1000;
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            long long hash = cr_copy.HashCalculate();
            hash = hash/6;

            short eval = getEvalFromHash(hash, depth - 1);
            if (eval==404){
                eval = alphaBeta(cr_copy, depth - 1, alpha, beta, true);
                storeHashEntry(cr_copy, depth - 1, eval);
            }

            minEval = std::min<short>(minEval, eval);
            beta = std::min<short>(beta, eval);
            if (beta <= alpha){
                break;
            }
        }
        return minEval;
    }
}

thc::Move findBestMove(thc::ChessRules& cr, char maxDepth, bool isWhite, int maximumTime) {
    thc::Move bestMove{};
    short bestValue = isWhite ? -1000 : 1000;

    std::cout << "Starting search\n";

    auto endTime = std::chrono::steady_clock::now() + std::chrono::seconds(maximumTime);

    for (int depth = 1; depth <= maxDepth; depth++) {
        std::cout << "Depth: " << depth << std::endl;

        for (auto& move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            long long hash = cr_copy.HashCalculate();
            hash = hash / 6;

            short eval = getEvalFromHash(hash, depth - 3);
            if (eval == 404) {
                eval = alphaBeta(cr_copy, depth - 1, -1000, 1000, !isWhite);
                storeHashEntry(cr_copy, depth - 1, eval);
            }

            if ((isWhite && eval > bestValue) || (!isWhite && eval < bestValue)) {
                bestValue = eval;
                bestMove = move;
            }
        }

        if (std::chrono::steady_clock::now() >= endTime)
            break;

        std::cout << bestMove.NaturalOut(&cr).c_str() << ", eval" << bestValue << "\n";
    }

    std::cout << bestMove.NaturalOut(&cr).c_str() << std::endl;
    return bestMove;
}


int main()
{
//    thc::ChessRules cr;
//    cr.Forsyth("8/8/4k3/8/2KQ4/8/8/8 w - - 0 1");
//    transpositionTable.reserve(amountOfRows);
//    DisplayPosition(cr);
//    findBestMove(cr, 40, true, 60);
    thc::ChessRules cr;
    cr.Forsyth("8/4k3/8/8/8/8/6q1/3K4 b - - 1 1");
    thc::Move mv{};
    mv.NaturalIn(&cr, endgameTable("8/4k3/8/8/8/8/6q1/3K4 b - - 1 1"));
    cr.PlayMove(mv);
    std::cout << cr.ToDebugStr() << std::endl;
    return 0;
}
// TODO:
// 1. Add quiescence search
// 2. Implement all the function inside findBestMove function

// NEEDS IMPROVEMENT:
// Iterative deepening, need to add the best found move to the top of move ordering list

// DONE:
// Transposition table
// Endgame table base
// Opening book

#pragma clang diagnostic pop