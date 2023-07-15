#include <iostream>
#include <string>
#include <vector>
#include "thc.h"
#include <unordered_map>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

struct hashEntry{
    char depth;
    short eval;
};

const unsigned long long amountOfRows = 1150000000;
std::vector<hashEntry> transpositionTable(amountOfRows);

void storeHashEntry(thc::ChessRules& cr, char depth, short eval){
    uint64_t hash = cr.HashCalculate();
    hash = hash/4;
    hashEntry entry = {depth, eval};
    transpositionTable[hash] = entry;
}

short getEvalFromHash(long long& hash, char minDepth){
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

    for (short i = 0; i < 64; i++) {
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

short alphaBeta(thc::ChessRules &cr, char depth, short alpha, short beta, bool maximizingPlayer){
    if (depth == 0){
        return evaluateMaterial(cr);
    }

    if (maximizingPlayer) {
        short maxEval = -1000;
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            long long hash = cr_copy.HashCalculate();
            hash = hash/4;
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
            hash = hash/4;

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

thc::Move findBestMove(thc::ChessRules &cr, char depth, bool isWhite){
    thc::Move bestMove = thc::Move();

    short bestValue = isWhite ? -1000 : 1000;

    std::cout << "Starting search\n";
    for (auto &move : GetLegalMoves(cr)) {
        thc::ChessRules cr_copy = cr;
        cr_copy.PlayMove(move);
        long long hash = cr_copy.HashCalculate();
        hash = hash/4;

        short eval = getEvalFromHash(hash, depth - 1);
        if (eval==404){
            eval = alphaBeta(cr_copy, depth - 1, -1000, 1000, !isWhite);
            storeHashEntry(cr_copy, depth - 1, eval);
        }

        if((isWhite && eval > bestValue) || (!isWhite && eval < bestValue)){
            bestValue = eval;
            bestMove = move;
        }
        std::cout << "Move: " << move.NaturalOut(&cr).c_str() << ", eval" << eval << "\n";
    }
    std::string mv_txt = bestMove.NaturalOut(&cr);
    std::cout << "Move: " << mv_txt.c_str() << ", Eval" << bestValue << "\n";

    return bestMove;
}

int main()
{
    thc::ChessRules cr;
    cr.Forsyth("r3kb1r/pp3pp1/1np1pn1p/4N2P/2PP1B2/6N1/qP2QPP1/2KRR3 w kq - 0 2");
    transpositionTable.reserve(amountOfRows);
    DisplayPosition(cr);
    findBestMove(cr, 8, true);
    return 0;
}

// TODO:
// 1. Add transposition tables with zobrist hashing - working
// 2. Add iterative deepening - started working
// 3. Add quiescence search
// 4. Add opening book
// 5. Add endgame table bases

#pragma clang diagnostic pop