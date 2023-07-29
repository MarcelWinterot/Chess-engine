//
// Created by Marcel on 30/07/2023.
//

#ifndef CHESS_ENGINE_EVALUATION_H
#define CHESS_ENGINE_EVALUATION_H

#include <unordered_map>
#include <array>
#include "thc.h"

class Evaluation {
private:
    static short evaluateMaterial(const thc::ChessRules& board);
    static short pieceSquareTables(const thc::ChessRules& board);
    static short mobility(thc::ChessRules& board);
    static int calculateMobilityForSide(thc::ChessRules& board, char side);

public:
    static short evaluate(thc::ChessRules& board);
};

#endif //CHESS_ENGINE_EVALUATION_H
