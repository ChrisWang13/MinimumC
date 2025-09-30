#!/bin/bash

# Demo script for MinimumC visualization feature
# This script shows the visualization of different C programs

echo "================================"
echo "MinimumC Visualization Demo"
echo "================================"
echo ""
echo "To enable visualization, set VISUALIZE to 1 in main.c and rebuild:"
echo "  #define VISUALIZE 1"
echo ""
echo "Press Enter to continue..."
read

echo ""
echo "Example 1: Simple return statement"
echo "-----------------------------------"
./MinimumC "int main() { return 42; }" 2>&1 | head -50

echo ""
echo ""
echo "Press Enter for next example..."
read

echo ""
echo "Example 2: Variable assignment and arithmetic"
echo "----------------------------------------------"
./MinimumC "int main() { int x; x = 10; return x + 5; }" 2>&1 | head -80

echo ""
echo ""
echo "Press Enter for next example..."
read

echo ""
echo "Example 3: Function with parameters"
echo "------------------------------------"
./MinimumC "int add(int x, int y) { return x + y; } int main() { return add(3, 7); }" 2>&1 | head -100

echo ""
echo ""
echo "Press Enter for next example..."
read

echo ""
echo "Example 4: Control flow (if statement)"
echo "---------------------------------------"
./MinimumC "int main() { int x; x = 5; if (x > 3) return 1; else return 0; }" 2>&1 | head -100

echo ""
echo "================================"
echo "Demo complete!"
echo "================================"