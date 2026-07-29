@echo off
echo === Clings Setup ===
echo.

echo [1/2] Installing Python dependencies...
pip install -e .
if errorlevel 1 (
    echo Failed to install Python dependencies
    pause
    exit /b 1
)

echo [2/2] Installing WASM compiler (npm)...
npm install
if errorlevel 1 (
    echo Failed to install npm dependencies
    pause
    exit /b 1
)

echo.
echo === Setup complete! ===
echo Run: clings
echo Or:  python -m clings
pause
