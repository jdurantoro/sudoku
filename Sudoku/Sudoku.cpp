// Sudoku.cpp : Defines the entry point for the console application.
//

#include <string>
#include <iostream>

#include "SudokuGrid.h"

using namespace std;

static void show_usage(std::string name)
{
	std::cerr << "Usage: " << name << " <option(s)> [drive:][path]filename\n"
		<< "Options:\n"
		<< "\t-h,--help\t\tPrints usage\n"
		<< "\t-s,--solve filename\tSpecify the Sudoku puzzle to be solved"
		<< std::endl;
}

int main(int argc, char** argv)
{
	if (argc < 3) {

		show_usage(argv[0]);
		return 1;
	}

	CSudokuGrid grid;

	for (int i = 1; i < argc; ++i) {

		std::string arg = argv[i];

		if ((arg == "-h") || (arg == "--help")) {

			show_usage(argv[0]);
			return 0;
		}
		else if ((arg == "-s") || (arg == "--solve")) {

			if (i + 1 < argc) {

				const std::string filename = argv[++i];

				if (grid.readGrid(filename))
				{
					uint32_t iter = 0;

					iter = grid.Solve();

					std::cout << "Iterations: " << iter << std::endl;
				}
				else {

					std::cerr << "Unable to open file" << std::endl;
					return 1;
				}
			}
			else {

				std::cerr << "--solve option requires a filename." << std::endl;
				return 1;
			}
		}
		else {
			// ToDO
			// Generate Sudoku
		}
	}

	return 0;
}

