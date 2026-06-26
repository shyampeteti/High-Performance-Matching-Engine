#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "order.h"
#include "orderbook.h"

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
    std::string line;
    uint64_t current_id = 1;
    OrderBook book; // Instantiate the Order Book Engine!

    // The infinite loop waiting for input
    while (std::getline(std::cin, line)) {
        if (line.empty() || line == "EXIT") break;

        // Expecting format: ACTION,SYMBOL,PRICE,QTY
        std::vector<std::string> tokens = split(line, ',');
        
        if (tokens.size() != 4) {
            std::cerr << "ERR: Invalid format. Use ACTION,SYMBOL,PRICE,QTY\n";
            continue;
        }

        bool is_buy = (tokens[0] == "BUY");
        std::string symbol = tokens[1];
        double price = std::stod(tokens[2]);
        int quantity = std::stoi(tokens[3]);

        // Create the order and immediately pass it to the engine for processing
        Order new_order(current_id++, symbol, is_buy, price, quantity);
        book.process_order(new_order);
    }

    return 0;
}