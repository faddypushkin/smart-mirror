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
if [ ! -d "./arch/tools/sc-dt" ]; then
	echo "Install toolchain..."

	cd ./arch/tools
	wget -q "https://drive.syntacore.com/s/8Atb2bcYjZS8eHE/download?path=/2025/2025.12&files=sc-dt-2025.12.tar.gz" -O sc-dt-toolchain.tar.gz
	tar -xzf sc-dt-toolchain.tar.gz -C ./
	rm -f sc-dt-toolchain.tar.gz
	cd ../../
else
	echo "Toolchain is already installed"
fi
echo ""

echo ""
echo "--------------------------------------"
echo "Development Workspace Ready [✔]"
echo "--------------------------------------"
