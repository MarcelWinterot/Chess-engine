#include <iostream>
#include <string>
#include <vector>
#include "thc.h"
#include <unordered_map>

struct HashEntry {
    std::string fen;
    short evaluation{};
    short depth{};
};

short findEvaluation(const std::unordered_map<long long, HashEntry>& hashMap, const std::string& fen, short minDepth){
    for (const auto& entry : hashMap) {
        if (entry.second.fen == fen && entry.second.depth >= minDepth) {
            return entry.second.evaluation;
        }
    }
    return 404;
}

void insertEntry(std::unordered_map<long long, HashEntry>& hashMap, const std::string& fen, short evaluation, short depth) {
    long long newKey = static_cast<long long>(hashMap.size()) + 1;
    HashEntry newEntry = {fen,evaluation, depth};
    hashMap[newKey] = newEntry;
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

short alphaBeta(thc::ChessRules &cr, short depth, short alpha, short beta, bool maximizingPlayer, std::unordered_map<long long, HashEntry> &hashMap){
    if (depth == 0){
        return evaluateMaterial(cr);
    }

    if (maximizingPlayer) {
        short maxEval = -1000;
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            short eval = findEvaluation(hashMap, cr_copy.ForsythPublish(), depth - 1);
            if (eval == 404){
                eval = alphaBeta(cr_copy, depth - 1, alpha, beta, false, hashMap);
                insertEntry(hashMap, cr_copy.ForsythPublish(), eval, depth - 1);
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
            short eval = findEvaluation(hashMap, cr_copy.ForsythPublish(), depth - 1);
            if (eval == 404){
                eval = alphaBeta(cr_copy, depth - 1, alpha, beta, true, hashMap);
                insertEntry(hashMap, cr_copy.ForsythPublish(), eval, depth - 1);
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

thc::Move findBestMove(thc::ChessRules &cr, short depth, bool isWhite, std::unordered_map<long long, HashEntry>& hashMap){
    thc::Move bestMove = thc::Move();

    short bestValue = isWhite ? -1000 : 1000;

    for (auto &move : GetLegalMoves(cr)) {
        thc::ChessRules cr_copy = cr;
        cr_copy.PlayMove(move);

        std::string fen = cr_copy.ForsythPublish();
        short eval = findEvaluation(hashMap, fen, depth);

        if (eval == 404){
            eval = alphaBeta(cr_copy, depth - 1, -1000, 1000, !isWhite, hashMap);
            insertEntry(hashMap, fen, eval, depth - 1);
        }

        std::cout << move.NaturalOut(&cr).c_str() << ": " << eval << "\n";

        if((isWhite && eval > bestValue) || (!isWhite && eval < bestValue)){
            bestValue = eval;
            bestMove = move;
        }
    }
    std::string mv_txt = bestMove.NaturalOut(&cr);
    std::cout << "Move: " << mv_txt.c_str() << ", Eval" << ((isWhite) ? " Max: " : " Min: ") << bestValue << "\n";

    return bestMove;
}



int main()
{
    thc::ChessRules cr;
    cr.Forsyth("r3kb1r/pp3pp1/1np1pn1p/4N2P/2PP1B2/6N1/qP2QPP1/2KRR3 w kq - 0 2");
    std::unordered_map<long long, HashEntry> hashMap;
    hashMap.clear();
    std::cout << hashMap.size() << std::endl;
    findBestMove(cr, 8, true, hashMap);
    return 0;
}

// TODO:
// 1. Add transposition tables with zobrist hashing - working but without the hashing
// 2. Add iterative deepening
// 3. Add quiescence search
// 4. Add opening book
// 5. Add endgame table bases
