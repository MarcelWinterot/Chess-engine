//
// Created by Marcel on 30/07/2023.
//

#include <unordered_map>
#include <array>
#include "thc.h"


class Evaluation {
    static short evaluateMaterial(const thc::ChessRules& board) {
        short score = 0;
        static const std::unordered_map<char, short> pieceValues = {
                {'P', 100}, {'N', 300}, {'B', 325}, {'R', 500}, {'Q', 900},
                {'p', -100}, {'n', -300}, {'b', -325}, {'r', -500}, {'q', -900}
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

    static short pieceSquareTables(const thc::ChessRules& board) {
        static const std::unordered_map<char, std::array<short, 64>> pieceSquareTable = {
                {'P', std::array<short, 64>{
                        0,  0,  0,  0,  0,  0,  0,  0,
                        50, 50, 50, 50, 50, 50, 50, 50,
                        10, 10, 20, 30, 30, 20, 10, 10,
                        5,  5, 10, 25, 25, 10,  5,  5,
                        0,  0,  0, 20, 20,  0,  0,  0,
                        5, -5,-10,  0,  0,-10, -5,  5,
                        5, 10, 10,-20,-20, 10, 10,  5,
                        0,  0,  0,  0,  0,  0,  0,  0
                }},
                {'N', std::array<short, 64>{
                        -50,-40,-30,-30,-30,-30,-40,-50,
                        -40,-20,  0,  0,  0,  0,-20,-40,
                        -30,  0, 10, 15, 15, 10,  0,-30,
                        -30,  5, 15, 20, 20, 15,  5,-30,
                        -30,  0, 15, 20, 20, 15,  0,-30,
                        -30,  5, 10, 15, 15, 10,  5,-30,
                        -40,-20,  0,  5,  5,  0,-20,-40,
                        -50,-40,-30,-30,-30,-30,-40,-50,
                }},
                {'B', std::array<short, 64>{
                        -20,-10,-10,-10,-10,-10,-10,-20,
                        -10,  0,  0,  0,  0,  0,  0,-10,
                        -10,  0,  5, 10, 10,  5,  0,-10,
                        -10,  5,  5, 10, 10,  5,  5,-10,
                        -10,  0, 10, 10, 10, 10,  0,-10,
                        -10, 10, 10, 10, 10, 10, 10,-10,
                        -10,  5,  0,  0,  0,  0,  5,-10,
                        -20,-10,-10,-10,-10,-10,-10,-20,
                }},
                {'R', std::array<short, 64>{
                        0,  0,  0,  0,  0,  0,  0,  0,
                        5, 10, 10, 10, 10, 10, 10,  5,
                        -5,  0,  0,  0,  0,  0,  0, -5,
                        -5,  0,  0,  0,  0,  0,  0, -5,
                        -5,  0,  0,  0,  0,  0,  0, -5,
                        -5,  0,  0,  0,  0,  0,  0, -5,
                        -5,  0,  0,  0,  0,  0,  0, -5,
                        0,  0,  0,  5,  5,  0,  0,  0
                }},
                {'Q', std::array<short, 64>{
                        -20,-10,-10, -5, -5,-10,-10,-20,
                        -10,  0,  0,  0,  0,  0,  0,-10,
                        -10,  0,  5,  5,  5,  5,  0,-10,
                        -5,  0,  5,  5,  5,  5,  0, -5,
                        0,  0,  5,  5,  5,  5,  0, -5,
                        -10,  5,  5,  5,  5,  5,  0,-10,
                        -10,  0,  5,  0,  0,  0,  0,-10,
                        -20,-10,-10, -5, -5,-10,-10,-20
                }},
                {'K', std::array<short, 64>{
                        -30,-40,-40,-50,-50,-40,-40,-30,
                        -30,-40,-40,-50,-50,-40,-40,-30,
                        -30,-40,-40,-50,-50,-40,-40,-30,
                        -30,-40,-40,-50,-50,-40,-40,-30,
                        -20,-30,-30,-40,-40,-30,-30,-20,
                        -10,-20,-20,-20,-20,-20,-20,-10,
                        20, 20,  0,  0,  0,  0, 20, 20,
                        20, 30, 10,  0,  0, 10, 30, 20
                }}
        };

        short score = 0;
        for (int i = 0; i < 64; i++) {
            auto square = static_cast<thc::Square>(i);
            char piece = board.squares[square];
            auto it = pieceSquareTable.find(toupper(piece));
            if (it != pieceSquareTable.end()) {
                if (islower(piece)) {
                    score -= it->second[63 - i];
                } else {
                    score += it->second[i];
                }
            }
        }

        return score;
    }

    static short mobility(thc::ChessRules& board) {
        int whiteMobility = calculateMobilityForSide(board, 'w');
        int blackMobility = calculateMobilityForSide(board, 'b');
        return static_cast<short>(whiteMobility - blackMobility);
    }

    static int calculateMobilityForSide(thc::ChessRules& board, char side) {
        int mobility = 0;
        std::vector<thc::Move> moves;
        board.GenLegalMoveList(moves);

        for (const auto& move : moves) {
            char piece = board.squares[move.src];
            if ((side == 'w' && std::isupper(piece)) || (side == 'b' && std::islower(piece))) {
                mobility++;
            }
        }

        return mobility;
    }

public:
    static short evaluate(thc::ChessRules& board){
        short material = evaluateMaterial(board);
        short pieceSquare = pieceSquareTables(board);
        short mobilityEval = mobility(board);
        return material + pieceSquare + mobilityEval;
    }
};
