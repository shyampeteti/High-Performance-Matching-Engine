#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <queue>
#include <vector>
#include <iostream>
#include "order.h"

// 1. Comparator for Buy Orders (Bids) - MAX HEAP
// We want the HIGHEST price at the top. If prices are equal, EARLIEST time wins.
struct BidCompare {
    bool operator()(const Order& a, const Order& b) const {
        if (a.price != b.price) return a.price < b.price; // Smaller price goes down
        return a.timestamp > b.timestamp;                 // Larger timestamp (newer) goes down
    }
};

// 2. Comparator for Sell Orders (Asks) - MIN HEAP
// We want the LOWEST price at the top. If prices are equal, EARLIEST time wins.
struct AskCompare {
    bool operator()(const Order& a, const Order& b) const {
        if (a.price != b.price) return a.price > b.price; // Larger price goes down
        return a.timestamp > b.timestamp;                 // Larger timestamp (newer) goes down
    }
};

class OrderBook {
private:
    std::priority_queue<Order, std::vector<Order>, BidCompare> bids;
    std::priority_queue<Order, std::vector<Order>, AskCompare> asks;

public:
    void process_order(Order& incoming) {
        if (incoming.is_buy) {
            // BUY ORDER: Match against the lowest ASKS
            while (!asks.empty() && incoming.quantity > 0) {
                Order best_ask = asks.top();
                
                // If our buy price is >= the lowest sell price, WE HAVE A TRADE!
                if (incoming.price >= best_ask.price) {
                    int trade_qty = std::min(incoming.quantity, best_ask.quantity);
                    
                    // Output the Trade JSON
                    std::cout << "{\"trade\": {\"price\": " << best_ask.price 
                              << ", \"quantity\": " << trade_qty 
                              << ", \"buyer_id\": " << incoming.id 
                              << ", \"seller_id\": " << best_ask.id << "}}\n";
                    
                    asks.pop(); // Remove the top ask
                    incoming.quantity -= trade_qty;
                    best_ask.quantity -= trade_qty;
                    
                    // If the sell order still has shares left, push it back
                    if (best_ask.quantity > 0) {
                        asks.push(best_ask);
                    }
                } else {
                    break; // The lowest ask is too expensive, stop matching.
                }
            }
            // If the buy order isn't completely filled, it "rests" in the book
            if (incoming.quantity > 0) {
                bids.push(incoming);
                std::cout << "{\"rested\": {\"id\": " << incoming.id << ", \"side\": \"BUY\", \"qty\": " << incoming.quantity << ", \"price\": " << incoming.price << "}}\n";
            }
            
        } else {
            // SELL ORDER: Match against the highest BIDS
            while (!bids.empty() && incoming.quantity > 0) {
                Order best_bid = bids.top();
                
                // If our sell price is <= the highest buy price, WE HAVE A TRADE!
                if (incoming.price <= best_bid.price) {
                    int trade_qty = std::min(incoming.quantity, best_bid.quantity);
                    
                    // Output the Trade JSON
                    std::cout << "{\"trade\": {\"price\": " << best_bid.price 
                              << ", \"quantity\": " << trade_qty 
                              << ", \"buyer_id\": " << best_bid.id 
                              << ", \"seller_id\": " << incoming.id << "}}\n";
                    
                    bids.pop();
                    incoming.quantity -= trade_qty;
                    best_bid.quantity -= trade_qty;
                    
                    if (best_bid.quantity > 0) {
                        bids.push(best_bid);
                    }
                } else {
                    break; // The highest bid is too low, stop matching.
                }
            }
            // If the sell order isn't completely filled, it "rests" in the book
            if (incoming.quantity > 0) {
                asks.push(incoming);
                std::cout << "{\"rested\": {\"id\": " << incoming.id << ", \"side\": \"SELL\", \"qty\": " << incoming.quantity << ", \"price\": " << incoming.price << "}}\n";
            }
        }
        std::cout << std::flush; // Critical for the Python wrapper to catch it instantly
    }
};

#endif