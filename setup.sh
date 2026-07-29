#!/bin/bash
echo "=== Clings Setup ==="
echo ""

echo "[1/2] Installing Python dependencies..."
pip install -e . || { echo "Failed"; exit 1; }

echo "[2/2] Installing WASM compiler (npm)..."
npm install || { echo "Failed"; exit 1; }

echo ""
echo "=== Setup complete! ==="
echo "Run: clings"
echo "Or:  python -m clings"
