# High-Performance Order Matching Engine ⚡
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Flask](https://img.shields.io/badge/Flask-000000?style=for-the-badge&logo=flask&logoColor=white)
[![C++ CI Build Check](https://github.com/shyampeteti/order-matching-engine/actions/workflows/build.yml/badge.svg)](https://github.com/shyampeteti/order-matching-engine/actions/workflows/build.yml)


A real-time, high-frequency trading matching engine core written in **C++**, bridged to a **Python (Flask)** web dashboard. This project simulates the backend architecture of a financial exchange, pairing buy and sell orders with ultra-low latency.

## 🏗️ Architecture

1. **C++ Core Engine:** The algorithmic brain of the application. It utilizes strict Time-Price Priority matching logic.
   * **Bids (Buy Orders):** Maintained using a `std::priority_queue` structured as a Max-Heap (highest price at the top).
   * **Asks (Sell Orders):** Maintained using a `std::priority_queue` structured as a Min-Heap (lowest price at the top).
   * Achieves highly optimized $O(\log N)$ order insertion and matching resolution.
2. **Python Subprocess Bridge:** A Flask server acts as an API gateway, taking JSON payloads from the web UI and piping them directly into the standard input (`std::cin`) of the continuously running C++ executable.
3. **HTML/JS Dashboard:** A dark-mode user interface that polls the engine for live trades and resting orders, updating asynchronously.

## 🚀 Quick Start

### Prerequisites
* `g++` (GCC compiler)
* `python3` and `pip`
* `flask` (`pip install flask`)

### 1. Compile the C++ Engine
Compile the core with the `-O3` flag for maximum speed optimization:
g++ -O3 engine.cpp -o engine

### 2. Run the Web Server
Launch the Flask bridge:
python app.py

### 3. Trade
Open your browser and navigate to `http://localhost:5000` or `http://127.0.0.1:5000`. Submit matching buy and sell orders to watch the engine execute trades in real-time.

## 📂 File Structure
* `order.h` - Defines the `Order` struct with nanosecond timestamping.
* `orderbook.h` - Contains the Max/Min Heap comparators and the core matching loop logic.
* `engine.cpp` - The infinite loop C++ entry point that parses incoming string data.
* `app.py` - The Flask web server, HTML dashboard, and Subprocess thread manager.