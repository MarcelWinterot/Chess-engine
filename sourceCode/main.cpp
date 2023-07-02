#include <iostream>
#include <string>
#include <vector>
#include "thc.h"

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

void DisplayLegalMoves( thc::ChessRules &cr )
{
    std::vector<thc::Move> moves = GetLegalMoves(cr);
    for(auto & move : moves)
    {
        std::string mv_txt = move.NaturalOut(&cr);
        std::cout << mv_txt.c_str() << std::endl;
    }
}

int evaluateMaterial(const thc::ChessRules& board) {
    int score = 0;
    for (int i = 0; i < 64; i++) { // iterate over all squares on the board
        auto square = static_cast<thc::Square>(i);
        char piece = board.squares[square]; // get the piece on the square
        switch (piece) {
            case 'P': score += 1; break; // white pawn
            case 'N': score += 3; break; // white knight
            case 'B': score += 3; break; // white bishop
            case 'R': score += 5; break; // white rook
            case 'Q': score += 9; break; // white queen
            case 'p': score -= 1; break; // black pawn
            case 'n': score -= 3; break; // black knight
            case 'b': score -= 3; break; // black bishop
            case 'r': score -= 5; break; // black rook
            case 'q': score -= 9; break; // black queen
            default: break;
        }
    }
    return score;
}

int alphaBeta(thc::ChessRules &cr, int depth, int alpha, int beta, bool maximizingPlayer){
    if (depth == 0){
        return evaluateMaterial(cr);
    }

    if (maximizingPlayer) {
        int maxEval = -1000;
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            int eval = alphaBeta(cr_copy, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha){
                break;
            }
        }
        return maxEval;
    } else {
        int minEval = 1000;
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            int eval = alphaBeta(cr_copy, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha){
                break;
            }
        }
        return minEval;
    }
}

thc::Move findBestMove(thc::ChessRules &cr, int depth, bool isWhite){
    if (isWhite){
        int maxEval = -1000;
        thc::Move bestMove = thc::Move();
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            int eval = alphaBeta(cr_copy, depth - 1, -1000, 1000, false);

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
        int maxEval = 1000;
        thc::Move bestMove = thc::Move();
        for (auto &move : GetLegalMoves(cr)) {
            thc::ChessRules cr_copy = cr;
            cr_copy.PlayMove(move);
            int eval = alphaBeta(cr_copy, depth - 1, -1000, 1000, true);

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
    cr.Forsyth("2bnk2r/1r1p1p1p/3b1qp1/pp1Pp1N1/8/P2PB2Q/1PP2PPP/R3K2R w KQk - 0 17");
    DisplayPosition(cr);
    findBestMove(cr, 6, true);
}

// TODO:
// 1. Add transpostion tables with zobrist hashing
// 2. Add iterative deepening
// 3. Add quiescence search
// 4. Add opening book
// 5. Add endgame tablebases