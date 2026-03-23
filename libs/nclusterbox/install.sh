#!/bin/bash
set -e

# Get the directory where the script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Building nclusterbox..."
make

echo "Done. The nclusterbox binary and its man page are located at: $SCRIPT_DIR"
