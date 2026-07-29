"""Clings - Main entry point (CLI/GUI dual mode)."""

import sys
import threading
import webbrowser


def run_server(host: str = "127.0.0.1", port: int = 3000, open_browser: bool = True):
    """Start FastAPI server."""
    import uvicorn

    if open_browser:
        def open_after_delay():
            import time
            time.sleep(1.5)
            webbrowser.open(f"http://{host}:{port}")
        threading.Thread(target=open_after_delay, daemon=True).start()

    print(f"Clings server running at http://{host}:{port}")
    print("Press Ctrl+C to stop")

    from .api import app
    uvicorn.run(app, host=host, port=port, log_level="info")


def run_gui(host: str = "127.0.0.1", port: int = 3000):
    """Start desktop GUI with pywebview."""
    try:
        import webview
    except ImportError:
        print("Error: pywebview not installed. Install with: pip install pywebview")
        sys.exit(1)

    import uvicorn

    # Start API server in background thread
    def start_server():
        from .api import app
        uvicorn.run(app, host=host, port=port, log_level="warning")

    server_thread = threading.Thread(target=start_server, daemon=True)
    server_thread.start()

    # Wait for server to start
    import time
    time.sleep(1)

    # Create webview window
    webview.create_window(
        "Clings - C语言练习",
        url=f"http://{host}:{port}",
        width=1200,
        height=800,
        min_size=(800, 600),
        text_select=True,
    )
    webview.start(debug="--debug" in sys.argv)


def main():
    """Main entry point."""
    args = sys.argv[1:]

    # Parse arguments
    host = "127.0.0.1"
    port = 3000

    for i, arg in enumerate(args):
        if arg == "--host" and i + 1 < len(args):
            host = args[i + 1]
        elif arg == "--port" and i + 1 < len(args):
            port = int(args[i + 1])

    # GUI mode
    if "--gui" in args:
        run_gui(host, port)
    # Server mode (default)
    else:
        run_server(host, port, open_browser="--no-browser" not in args)


if __name__ == "__main__":
    main()
