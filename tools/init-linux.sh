#!/usr/bin/env bash

echo ""
echo "--------------------------------------"
echo "Initializing Development Workspace [×]"
echo "--------------------------------------"
echo ""

echo ""
echo "Installing git submodules"
git submodule update --init --recursive
echo ""

echo ""
echo "--------------------------------------"
echo "Installing utilities"
echo "--------------------------------------"
echo ""

echo "Checking the name of the Linux distribution..."
if [ -f /etc/os-release ]; then
	. /etc/os-release
fi

echo ""
echo "Installing Linux utilities..."
echo "(arch and debian are supported)"
if [ "$ID" = "arch" ]; then
	sudo pacman -S wget tar make python3
elif [ "$ID" = "debian" ]; then
	sudo apt install wget tar make python3
fi
echo ""

echo ""
echo "--------------------------------------"
echo "Development Workspace Ready [✔]"
echo "--------------------------------------"
