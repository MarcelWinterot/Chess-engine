//
// Created by Marcel on 16/07/2023.
//

#include "endgameTable.h"
#include <iostream>
#include <string>
#include "thc.h"
#include <array>

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