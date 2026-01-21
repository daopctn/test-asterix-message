#!/bin/bash
# Simple test script to run client and server

echo "Starting ASTERIX TCP Server in background..."
./build/bin/asterix-tcp-server &
SERVER_PID=$!

# Wait for server to start
sleep 1

echo ""
echo "Running Simple Client..."
echo "========================================"
./build/bin/simple-client

echo ""
echo "========================================"
echo "Test completed. Stopping server..."
kill $SERVER_PID 2>/dev/null

echo "Done!"
