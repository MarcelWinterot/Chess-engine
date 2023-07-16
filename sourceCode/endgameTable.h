//
// Created by Marcel on 16/07/2023.
//

#ifndef CHESS_ENGINE_ENDGAMETABLE_H
#define CHESS_ENGINE_ENDGAMETABLE_H

#include <string>

std::string convertToCurlReadableFen(const std::string& fen);
std::string getMoveFromCurl(const std::string& answer);
std::string endgameTable(const std::string& fen);


#endif //CHESS_ENGINE_ENDGAMETABLE_H
