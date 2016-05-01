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
		<< "\t-s,--solve filename\tSpecify the Sudoku puzzle to be solved\n"
		<< "\t-g n ,--generate n\tGenerate a Sudoku puzzle of difficulty level 'n' (0=easy, 1=medium, 2=hard, 3=samurai)"
		<< std::endl;
}

int main(int argc, char** argv)
{
	if ((argc < 3) || (argc > 5)) {

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

					grid.solve(iter);

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
		else if ((arg == "-g") || (arg == "--generate")) {

			if (i + 1 < argc) {

				const std::string level = argv[++i];

				if ((level != "0") && (level != "1") && (level != "2") && (level != "3")) {

					std::cerr << "--generate option requires a difficulty level from 0 to 3 (0=easy, 1=medium, 2=hard, 3=samurai)." << std::endl;
					return 1;
				}

				std::cout << "Generating Sudoku level " << level << "" << std::endl;

				int result = 0;

				result = grid.generate((uint8_t)std::stoi(level));

				//std::cout << "Result: " << result << std::endl;
			}
			else {

				std::cerr << "--generate option requires a difficulty level from 0 to 3 (0=easy, 1=medium, 2=hard, 3=samurai)." << std::endl;
				return 1;
			}
		}
	}

	return 0;
}

