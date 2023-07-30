#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>

// Chess libraries, and utils
#include "thc.h"
#include "bookMoves.h"
#include "evaluation.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

struct hashEntry{
    int depth;
    short eval;
};

const unsigned long long amountOfRows = 4320000000;
std::unordered_map<unsigned long, hashEntry> transpositionTable;

void storeHashEntry(thc::ChessRules& cr, int depth, short eval){
    if (depth >= 5){
        uint64_t hash = cr.HashCalculate();
        hash = hash;
        hashEntry entry = {depth, eval};
        transpositionTable[hash] = entry;
    }
}

short getEvalFromHash(long long& hash, int minDepth){
    hashEntry result = transpositionTable[hash];
    if (result.depth >= minDepth && result.depth > 4){
        return result.eval;
    }
    return 404;
}

std::vector<thc::Move> GetLegalMoves(thc::ChessRules& cr)
{
    std::vector<thc::Move> moves, checks, captures, normal;
    std::vector<bool> check, mate, stalemate;
    cr.GenLegalMoveList(moves, check, mate, stalemate);
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

std::vector<thc::Move> GetLegalMoves(thc::ChessRules& cr, thc::Move firstMove)
{
    std::vector<thc::Move> moves, checks, captures, normal;
    std::vector<bool> check, mate, stalemate;
    cr.GenLegalMoveList(moves, check, mate, stalemate);
    moves.erase(std::remove(moves.begin(), moves.end(), firstMove), moves.end());
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
    moves.push_back(firstMove);
    moves.insert(moves.end(), checks.begin(), checks.end());
    moves.insert(moves.end(), captures.begin(), captures.end());
    moves.insert(moves.end(), normal.begin(), normal.end());
    return moves;
}

short calculateExtension(thc::ChessRules& cr, thc::Move& move, int depth, int maxDepth){
    if (cr.squares[move.dst] != ' '){
        if (depth + 1 > maxDepth){
            return 0;
        }
        return 1;
    }
    return 0;
}

short alphaBeta(thc::ChessRules &cr, int depth, short alpha, short beta, bool maximizingPlayer, thc::Move bestMoveLastIteration, int maxDepth){
    if (depth == 0){
        return Evaluation::evaluate(cr);
    }

    short bestEval = maximizingPlayer ? -1000 : 1000;

    for (auto &move : GetLegalMoves(cr, bestMoveLastIteration)) {
        thc::ChessRules cr_copy = cr;
        cr_copy.PlayMove(move);

        long long hash = cr_copy.HashCalculate();
        short eval = getEvalFromHash(hash, depth - 2);
        if (eval == 404) {
            eval = alphaBeta(cr_copy, depth - 1, alpha, beta, !maximizingPlayer, bestMoveLastIteration, maxDepth);
            storeHashEntry(cr_copy, depth - 1, eval);
        }

        if (maximizingPlayer){
            bestEval = std::max<short>(bestEval, eval);
            alpha = std::max<short>(alpha, bestEval);
        }
        else {
            bestEval = std::min<short>(bestEval, eval);
            beta = std::min<short>(beta, bestEval);
        }

        if (beta <= alpha){
            break;
        }
    }
    return bestEval;
}

thc::Move findBestMove(thc::ChessRules& cr, short maxDepth, bool isWhite) {
    thc::Move bestMove{};
    for (int depth = 1; depth <= maxDepth; depth++) {
        std::cout << "Depth: " << depth << std::endl;
        short bestValue = isWhite ? -1000 : 1000;
        thc::Move bestMoveLastIteration = bestMove;
        std::vector<thc::Move> moves = depth == 1 ? GetLegalMoves(cr) : GetLegalMoves(cr, bestMoveLastIteration);

        for (auto& move : moves) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);

            long long hash = cr_copy.HashCalculate();
            short eval = getEvalFromHash(hash, depth - 1);
            if (eval == 404) {
                eval = alphaBeta(cr_copy, depth - 1, -1000, 1000, !isWhite, bestMoveLastIteration, depth + 3);
                storeHashEntry(cr_copy, depth - 1, eval);
            }

            std::cout << move.NaturalOut(&cr).c_str() << ": " << eval << std::endl; // for debugging purposes

            if ((isWhite && eval > bestValue) || (!isWhite && eval < bestValue)) {
                bestValue = eval;
                bestMove = move;
            }
        }
        std::cout << "Best move so far : " << bestMove.NaturalOut(&cr).c_str() << "\n";
    }

    std::cout << bestMove.NaturalOut(&cr).c_str() << std::endl;
    return bestMove;
}

[[noreturn]] void playGame(){
    bool isWhite = true;
    thc::ChessRules cr;
    thc::Move mv{};
    std::string move, games, currentGame = "1. ";
    readInGames(games);
    while (true){
        if (isWhite){
            std::cout << "Enter your move: ";
            std::cin >> move;
            currentGame += move + " ";
            mv.NaturalIn(&cr, move);
            cr.PlayMove(mv);
            isWhite = false;
            std::cout << cr.ToDebugStr() << std::endl;
        }
        else {
            move = openingBook(currentGame, games);
            if (move == "404"){
                mv = findBestMove(cr, 4, isWhite);
            }
            else {
                mv.NaturalIn(&cr, move);
            }
            cr.PlayMove(mv);
            isWhite = true;
            std::cout << cr.ToDebugStr() << std::endl;
        }
    }
}

int main()
{
    playGame();
}
// TODO:
// 1. Fix the hash table
// 2. Fix the extensions
// 3. Add NNUE as the evaluation

#pragma clang diagnostic pop