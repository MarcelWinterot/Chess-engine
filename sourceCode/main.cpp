#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#include <iostream>
#include <string>
#include <vector>
#include "thc.h"
#include <unordered_map>

struct HashEntry {
    std::string fen;
    short int evaluation{};
    char depth{};
};

int findEvaluation(const std::unordered_map<int, HashEntry>& hashMap, const std::string& fen, int minDepth){
    for (const auto& entry : hashMap) {
        if (entry.second.fen == fen && entry.second.depth >= minDepth) {
            return entry.second.evaluation;
        }
    }
    return 404;
}

void insertEntry(std::unordered_map<int, HashEntry>& hashMap, const std::string& fen, short evaluation, char depth) {
    int newKey = static_cast<int>(hashMap.size()) + 1;
    HashEntry newEntry = {fen,evaluation, depth};
    hashMap[newKey] = newEntry;
}

void DisplayPosition( thc::ChessRules &cr)
{
    std::string s = cr.ToDebugStr();
    std::cout << s.c_str() << std::endl;
}

std::vector<thc::Move> GetLegalMoves( thc::ChessRules &cr )
{
    std::vector<thc::Move> moves, checks, captures, attacks;
    std::vector<bool> check, mate, stalemate;
    cr.GenLegalMoveList(  moves, check, mate, stalemate );
    for (int i = 0; i < moves.size(); i++) {
        if (check[i]) {
            checks.push_back(moves[i]);
        } else if (cr.squares[moves[i].dst] != ' ') {
            captures.push_back(moves[i]);
        } else {
            attacks.push_back(moves[i]);
        }
    }
    moves.clear();
    moves.insert(moves.end(), checks.begin(), checks.end());
    moves.insert(moves.end(), captures.begin(), captures.end());
    moves.insert(moves.end(), attacks.begin(), attacks.end());
    return moves;
}

int alphaBeta(thc::ChessRules &cr, char depth, short int alpha, short int beta, bool maximizingPlayer){
    if (depth == 0){
        return cr.Evaluate();
    }

    if (maximizingPlayer) {
        short int maxEval = -1000;
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            short int eval = alphaBeta(cr_copy, depth - 1, alpha, beta, false);
            maxEval = std::max(static_cast<short int>(maxEval), static_cast<short int>(eval));
            alpha = std::max(static_cast<short int>(alpha), static_cast<short int>(eval));
            if (beta <= alpha){
                break;
            }
        }
        return maxEval;
    } else {
        short int minEval = 1000;
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            short int eval = alphaBeta(cr_copy, depth - 1, alpha, beta, true);
            minEval = std::min(static_cast<short int>(minEval), static_cast<short int>(eval));
            beta = std::min(static_cast<short int>(beta), static_cast<short int>(eval));
            if (beta <= alpha){
                break;
            }
        }
        return minEval;
    }
}

thc::Move findBestMove(thc::ChessRules &cr, char depth, bool isWhite, std::unordered_map<int, HashEntry>& hashMap){
    if (isWhite){
        short int maxEval = -1000;
        thc::Move bestMove = thc::Move();
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            std::string fen = cr_copy.ForsythPublish();
            short int eval = findEvaluation(hashMap, fen, depth);

            if (eval == 404){
                eval = alphaBeta(cr_copy, depth - 1, -1000, 1000, false);
                insertEntry(hashMap, fen, eval, depth - 1);
            }

            if (eval == 1000) {
                std::string mv_txt = move.NaturalOut(&cr);
                std::cout << mv_txt.c_str();
                return move;
            }

            if (eval > maxEval){
                maxEval = eval;
                bestMove = move;
            }
        }
        std::string mv_txt = bestMove.NaturalOut(&cr);
        std::cout << mv_txt.c_str();
        return bestMove;
    }
    else {
        short int maxEval = 1000;
        thc::Move bestMove = thc::Move();
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);

            std::string fen = cr_copy.ForsythPublish();
            short int eval = findEvaluation(hashMap, fen, depth);

            if (eval == 404){
                eval = alphaBeta(cr_copy, depth - 1, -1000, 1000, true);
                insertEntry(hashMap, fen, eval, depth - 1);
            }

            if (eval == -1000) {
                std::string mv_txt = move.NaturalOut(&cr);
                std::cout << mv_txt.c_str();
                return move;
            }
            if (eval < maxEval){
                maxEval = eval;
                bestMove = move;
            }
        }
        std::string mv_txt = bestMove.NaturalOut(&cr);
        std::cout << mv_txt.c_str();
        return bestMove;
    }
}

int main()
{
    thc::ChessRules cr;
    cr.Forsyth("1q2br1k/1p6/1P1N1n1p/p1P1pB2/P2b2p1/B5K1/3Q4/5R2 w - - 0 1");
    DisplayPosition(cr);
    std::unordered_map<int, HashEntry> hashMap;
    hashMap.clear();
    findBestMove(cr, 10, true, hashMap);
    return 0;
}

// TODO:
// 1. Add transposition tables with zobrist hashing - working but without the hashing
// 2. Add iterative deepening
// 3. Add quiescence search
// 4. Add opening book
// 5. Add endgame table bases