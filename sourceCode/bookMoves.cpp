//
// Created by Marcel on 16/07/2023.
//

#include "bookMoves.h"
#include <iostream>
#include <string>
#include "thc.h"
#include <array>
#include <fstream>

std::string convertToCurlReadableFen(const std::string& fen) {
    std::string result;
    result.reserve(fen.length());
    for (char c : fen) {
        result += (c == ' ') ? '_' : c;
    }
    return result;
}

std::string getMoveFromCurl(const std::string& answer) {
    size_t found = answer.find("uci") + 6;
    return answer.substr(found, answer.find('"', found) - found);
}

std::string endgameTable(const std::string& fen) {
    std::string command = "curl http://tablebase.lichess.ovh/standard?fen=";
    command += convertToCurlReadableFen(fen);

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error executing curl command." << std::endl;
        return "";
    }

    constexpr int bufferSize = 128;
    std::array<char, bufferSize> buffer{};

    std::string response;
    while (fgets(buffer.data(), bufferSize, pipe) != nullptr) {
        response += buffer.data();
    }

    pclose(pipe);

    return getMoveFromCurl(response);
}

void readInGames(std::string& fileContent){
    std::ifstream games("games.txt");

    if (games.is_open()) {
        std::string line;
        while (std::getline(games, line)) {
            fileContent += line + "\n";
        }

        games.close();
    } else {
        std::cout << "Failed to open the file." << std::endl;
    }
}

std::string openingBook(std::string &currentGame, std::string &games) {
    unsigned long long notFoundCode = 18446744073709551615;
    unsigned length = currentGame.length();
    readInGames(games);
    size_t location = games.find(currentGame);
    if (location != notFoundCode){
        std::string  moveToPlay = games.substr(length + location, 2);

        if (games[length + location + 2] != ' ') {
            moveToPlay += games[length + location + 2];
        }
        currentGame += moveToPlay + " ";

        return moveToPlay;
    }
//    We didn't find the game, so we return error code 404
    return "404";
}