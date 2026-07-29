"""
Clings Desktop - Standalone entry point for Nuitka packaging.
Starts FastAPI server + pywebview window as a single .exe.
"""
import os
import sys
import threading
import time
import webbrowser

# Ensure the package is importable when frozen
if getattr(sys, 'frozen', False):
    BASE_DIR = os.path.dirname(sys.executable)
else:
    BASE_DIR = os.path.dirname(os.path.abspath(__file__))

os.chdir(BASE_DIR)

def main():
    import uvicorn
    import webview

    host = "127.0.0.1"
    port = 3000

    # Parse args
    args = sys.argv[1:]
    for i, arg in enumerate(args):
        if arg == "--port" and i + 1 < len(args):
            port = int(args[i + 1])
        elif arg == "--host" and i + 1 < len(args):
            host = args[i + 1]

    # Import FastAPI app
    from clings.api import app

    # Start server in background thread
    def start_server():
        uvicorn.run(app, host=host, port=port, log_level="warning")

    server_thread = threading.Thread(target=start_server, daemon=True)
    server_thread.start()

    # Wait for server to be ready
    time.sleep(1.5)

    # Create pywebview window
    webview.create_window(
        "Clings - C语言练习",
        url=f"http://{host}:{port}",
        width=1200,
        height=800,
        min_size=(800, 600),
        text_select=True,
    )
    webview.start(debug="--debug" in args)


if __name__ == "__main__":
    main()
