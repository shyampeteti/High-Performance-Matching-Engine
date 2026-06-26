from flask import Flask, request, jsonify, render_template_string
import subprocess
import threading
import json

app = Flask(__name__)

# 1. Boot up your C++ Matching Engine in the background
engine = subprocess.Popen(
    ['./engine'], 
    stdin=subprocess.PIPE, 
    stdout=subprocess.PIPE, 
    text=True, 
    bufsize=1
)

trade_logs = []

# 2. Background thread to constantly read the C++ engine's output
def read_engine_output():
    for line in iter(engine.stdout.readline, ''):
        if line:
            try:
                # Parse the JSON string from C++
                log_data = json.loads(line.strip())
                trade_logs.append(log_data)
            except:
                trade_logs.append({"raw": line.strip()})

threading.Thread(target=read_engine_output, daemon=True).start()

# 3. The Frontend HTML/JS Dashboard (Slick Dark Mode)
HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Matching Engine Dashboard</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #121212; color: #ffffff; padding: 20px; }
        .container { max-width: 800px; margin: auto; }
        .panel { background-color: #1e1e1e; padding: 20px; border-radius: 8px; margin-bottom: 20px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); }
        .input-group { display: flex; gap: 10px; margin-bottom: 15px; }
        input, select { padding: 10px; border: 1px solid #333; border-radius: 4px; background-color: #2d2d2d; color: white; width: 100%; }
        button { padding: 10px 20px; border: none; border-radius: 4px; font-weight: bold; cursor: pointer; color: white; width: 100px; }
        .btn-buy { background-color: #00c853; }
        .btn-buy:hover { background-color: #00e676; }
        .btn-sell { background-color: #d50000; }
        .btn-sell:hover { background-color: #ff1744; }
        .log-box { background-color: #000; padding: 15px; border-radius: 4px; height: 300px; overflow-y: auto; font-family: monospace; }
        .log-trade { color: #00e676; }
        .log-rested { color: #ffab00; }
    </style>
</head>
<body>
    <div class="container">
        <h2>⚡ High-Performance Matching Engine</h2>
        
        <div class="panel">
            <h3>Place Order</h3>
            <div class="input-group">
                <select id="action">
                    <option value="BUY">BUY</option>
                    <option value="SELL">SELL</option>
                </select>
                <input type="text" id="symbol" value="AAPL" placeholder="Symbol">
                <input type="number" id="price" value="150" placeholder="Price">
                <input type="number" id="quantity" value="10" placeholder="Quantity">
            </div>
            <button class="btn-buy" id="submitBtn" onclick="submitOrder()">Submit</button>
        </div>

        <div class="panel">
            <h3>Engine Live Feed</h3>
            <div class="log-box" id="logs"></div>
        </div>
    </div>

    <script>
        // Update button color based on action
        document.getElementById('action').addEventListener('change', function(e) {
            const btn = document.getElementById('submitBtn');
            btn.className = e.target.value === 'BUY' ? 'btn-buy' : 'btn-sell';
        });

        async function submitOrder() {
            const data = {
                action: document.getElementById('action').value,
                symbol: document.getElementById('symbol').value.toUpperCase(),
                price: document.getElementById('price').value,
                quantity: document.getElementById('quantity').value
            };
            
            await fetch('/order', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(data)
            });
        }

        // Poll for new logs from the C++ engine every 500ms
        setInterval(async () => {
            const res = await fetch('/logs');
            const logs = await res.json();
            const logBox = document.getElementById('logs');
            logBox.innerHTML = logs.map(log => {
                if (log.trade) return `<div class="log-trade">⚡ TRADE EXECUTED: ${log.trade.quantity} shares @ $${log.trade.price} (Buyer: ${log.trade.buyer_id}, Seller: ${log.trade.seller_id})</div>`;
                if (log.rested) return `<div class="log-rested">⏳ ORDER RESTED: ${log.rested.side} ${log.rested.qty} shares @ $${log.rested.price} (ID: ${log.rested.id})</div>`;
                return `<div>${JSON.stringify(log)}</div>`;
            }).reverse().join('');
        }, 500);
    </script>
</body>
</html>
"""

@app.route('/')
def home():
    return render_template_string(HTML_TEMPLATE)

# 4. API to pipe web form data into the C++ executable
@app.route('/order', methods=['POST'])
def place_order():
    data = request.json
    command = f"{data['action']},{data['symbol']},{data['price']},{data['quantity']}\n"
    
    engine.stdin.write(command)
    engine.stdin.flush() # Force it into C++ std::cin immediately
    
    return jsonify({"status": "sent"})

# 5. API to fetch logs for the UI
@app.route('/logs')
def get_logs():
    return jsonify(trade_logs)

if __name__ == '__main__':
    print("🚀 Server running at http://localhost:5000")
    app.run(port=5000, debug=False, use_reloader=False)