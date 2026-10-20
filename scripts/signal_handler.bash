#!/bin/bash

set -euo pipefail

# defining colors
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

# Capture ctrl + c (SIGINT) and exists the code properly
Handler(){
    echo -e "${MAGENTA}Exiting${NC}"
    exit 0;
}
