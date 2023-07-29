//
// Created by Marcel on 16/07/2023.
//

#ifndef CHESS_ENGINE_ENDGAMETABLE_H
#define CHESS_ENGINE_ENDGAMETABLE_H

#include <string>

std::string convertToCurlReadableFen(const std::string& fen);
std::string getMoveFromCurl(const std::string& answer);
std::string endgameTable(const std::string& fen);
void readInGames(std::string& fileContent);
std::string openingBook(std::string &currentGame, std::string &games);


#endif //CHESS_ENGINE_ENDGAMETABLE_H
