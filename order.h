#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <chrono>

struct Order {
    uint64_t id;          // Internal tracker
    std::string symbol;   // Ticker, e.g., "AAPL"
    bool is_buy;          // true for BUY, false for SELL
    double price;         // Limit price
    int quantity;         // Number of shares
    long long timestamp;  // Epoch time in nanoseconds

    // Constructor 
    Order(uint64_t _id, std::string _symbol, bool _is_buy, double _price, int _quantity)
        : id(_id), symbol(_symbol), is_buy(_is_buy), price(_price), quantity(_quantity) {
        // Auto-generate timestamp on creation
        timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::high_resolution_clock::now().time_since_epoch()
                    ).count();
    }
};

#endif