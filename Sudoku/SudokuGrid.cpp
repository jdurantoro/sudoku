#include "SudokuGrid.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

CSudokuGrid::CSudokuGrid():
	allValues(allValuesAscii, allValuesAscii + sizeof(allValuesAscii))
{
}

CSudokuGrid::~CSudokuGrid()
{
}

/**
 * Reads a Sudoku from a normal text file. 
 *
 * The text file should be build by 9 rows with 9 characters each followed by an End of Line char. 
 * Each character is either a number (1-9) or some other character to symbolize an empty box. 
 * Stores the read Sudoku in a suitable data structure
 */
bool CSudokuGrid::readGrid(const std::string & fileName)
{
	std::ifstream file(fileName);
	int rowId = 0;

	if (file.is_open()) {

		std::string line;

		while (getline(file, line) && (rowId < 9)) {

			std::istringstream iss(line);
			int colId = 0;
			char value;

			while ((iss >> value) && (colId < 9)) {
				// '1' = 49, '9' = 57
				if ((value >= 49) && (value <= 57)) {
					pushBack(rowId, colId, value);
				}
				else {
					notAssigned(rowId, colId);
				}

				colId++;
			}

			if (colId < 9) {

				std::cerr << "Wrong number of columns in row " << rowId << std::endl;
				return 1;
			}

			rowId++;
		}

		if (rowId < 9) {

			std::cerr << "Less number of rows than expected: " << rowId << std::endl;
			return 1;
		}

		file.close();
		print();
	}

	return true;
}

/**
 * Inserts a new value at the end of a cell list
 */
void CSudokuGrid::pushBack(uint16_t rowId, uint16_t colId, char value)
{
	assert(rowId < NROF_ROWS);
	assert(colId < NROF_COLS);
	assert(value >= 49); assert(value <= 57); // '1' = 49, '9' = 57

	cells[rowId][colId].push_back(value);
}

/**
 * Reduces list of a cell to its first value
 */
void CSudokuGrid::assign(const uint16_t rowId, const uint16_t colId, const char value)
{
	assert(rowId < NROF_ROWS);
	assert(colId < NROF_COLS);
	assert(value >= 49); assert(value <= 57); // '1' = 49, '9' = 57

	cells[rowId][colId].assign(1, value);
}

/**
 * Inserts all possible values in the cell list
 */
void CSudokuGrid::notAssigned(const uint16_t rowId, const uint16_t colId)
{
	assert(rowId < NROF_ROWS);
	assert(colId < NROF_COLS);

	cells[rowId][colId].assign(allValues.begin(), allValues.end());
}

/**
 * Removes candidates equal to already assigned values within a row recursively
 */
uint32_t CSudokuGrid::checkRow(const uint16_t rowId)
{
	assert(rowId < NROF_ROWS);
	
	uint32_t result = 0;

	std::vector<char> val;
	std::vector<cellPos_t> candPos;

	for (uint16_t colId = 0; colId < NROF_COLS; colId++) {

		if (1 == cells[rowId][colId].size()) {
			val.push_back(cells[rowId][colId].front());
		}
		else {
			candPos.push_back({rowId, colId});
		}
	}

	if (NROF_COLS == val.size())
		return 0;

	result = remove(val, candPos);

	if (result) {
		result += checkRow(rowId);
	}

	return result;
}

/**
 * Removes candidates equal to already assigned values within a row recursively for all rows in the grid
 */
uint32_t CSudokuGrid::checkRows(const uint16_t rowFirstId, const uint16_t rowLastId)
{
	assert(rowFirstId < NROF_ROWS);
	assert(rowLastId < NROF_ROWS);
	assert(rowFirstId < rowLastId);

	uint32_t result = 0;

	for (uint16_t rowId = rowFirstId; rowId <= rowLastId; rowId++) {
		result += checkRow(rowId);
	}

	return result;
}

/**
 * Removes candidates equal to already assigned values within a column recursively
 */
uint32_t CSudokuGrid::checkColumn(const uint16_t colId)
{
	assert(colId < NROF_COLS);
	
	uint32_t result = 0;

	std::vector<char> val;
	std::vector<cellPos_t> candPos;

	for (uint16_t rowId = 0; rowId < NROF_ROWS; rowId++) {

		if (1 == cells[rowId][colId].size()) {
			val.push_back(cells[rowId][colId].front());
		}
		else {
			candPos.push_back({rowId, colId});
		}
	}

	if (NROF_ROWS == val.size())
		return 0;

	result = remove(val, candPos);

	if (result) {
		result += checkColumn(colId);
	}

	return result;
}

/**
 * Removes candidates equal to already assigned values within a column recursively for all columns in the grid
 */
uint32_t CSudokuGrid::checkColumns(const uint16_t colFirstId, const uint16_t colLastId)
{
	assert(colFirstId < NROF_COLS);
	assert(colLastId < NROF_COLS);
	assert(colFirstId < colLastId);

	uint32_t result = 0;

	for (uint16_t colId = colFirstId; colId <= colLastId; colId++) {
		result += checkColumn(colId);
	}

	return result;
}

/**
 * Removes candidates equal to already assigned values within a box recursively
 */
uint32_t CSudokuGrid::checkBox(const uint16_t bandId, const uint16_t stackId)
{
	assert(bandId < NROF_BANDS);
	assert(stackId < NROF_STACKS);

	uint32_t result = 0;

	std::vector<char> val;
	std::vector<cellPos_t> candPos;

	for (uint16_t rowId = 0; rowId < (NROF_ROWS / NROF_BANDS); rowId++) {

		for (uint16_t colId = 0; colId < (NROF_COLS / NROF_STACKS); colId++) {

			const cellPos_t cellPos{(uint16_t)(bandId * (NROF_ROWS / NROF_BANDS) + rowId), (uint16_t)(stackId * (NROF_COLS / NROF_STACKS) + colId)};

			if (1 == cells[cellPos.rowId][cellPos.colId].size()) {
				val.push_back(cells[cellPos.rowId][cellPos.colId].front());
			}
			else {
				candPos.push_back({ cellPos.rowId, cellPos.colId });
			}
		}
	}

	if (NROF_ROWS == val.size())
		return 0;

	result = remove(val, candPos);

	if (result) {
		result += checkBox(bandId, stackId);
	}

	return result;
}

/**
 * Removes candidates equal to already assigned values within a box recursively for all boxes in the grid
 */
uint32_t CSudokuGrid::checkBoxes(const uint16_t bandFirstId, const uint16_t bandLastId, const uint16_t stackFirstId, const uint16_t stackLastId)
{
	assert(bandFirstId < NROF_BANDS);
	assert(bandLastId < NROF_BANDS);
	assert(bandFirstId < bandLastId);

	assert(stackFirstId < NROF_STACKS);
	assert(stackLastId < NROF_STACKS);
	assert(stackFirstId < stackLastId);

	uint32_t result = 0;

	for (uint16_t bandId = bandFirstId; bandId <= bandLastId; bandId++) {

		for (uint16_t stackId = stackFirstId; stackId <= stackLastId; stackId++) {

			result += checkBox(bandId, stackId);
			result += hiddenSingleBox(bandId, stackId);
		}
	}

	return result;
}

/**
 * Also known as 'scanning'. Searches for a cell within the given box that contains a candidate that 
 * only appears one in the box but it is not aasigned yet
 */
uint32_t CSudokuGrid::hiddenSingleBox(const uint16_t bandId, const uint16_t stackId)
{
	assert(bandId < NROF_BANDS);
	assert(stackId < NROF_STACKS);

	uint32_t result = 0;

	std::list<char> cand;
	std::vector<cellPos_t> candPos;

	sumBox(bandId, stackId, cand, candPos);

	if (0 == cand.size())
		return 0;

	std::list<char> candUnique = cand;
	candUnique.sort();
	candUnique.unique();

	for (std::list<char>::iterator it = candUnique.begin(); it != candUnique.end(); ++it) {
	
		int count = std::count(cand.begin(), cand.end(), *it);
		if (1 == count) {

			const int distance = std::distance(cand.begin(), std::find(cand.begin(), cand.end(), *it));
			const cellPos_t cellPos = candPos[distance];

			cells[cellPos.rowId][cellPos.colId].clear();
			cells[cellPos.rowId][cellPos.colId].push_back(*it);

			result++;
		}
	}

	return result;
}

/**
 * Also known as 'intersection'. If a candidate occurs twice or three times in just one row, 
 * then we can remove it from the other cells of the band.
 */
uint32_t CSudokuGrid::checkBand(const uint16_t bandId)
{
	assert(bandId < NROF_BANDS);

	uint32_t result = 0;

	for (uint16_t stackId = 0; stackId < NROF_STACKS; stackId++) {

		uint32_t resultStack = 0;

		std::list<char> cand;
		std::vector<cellPos_t> candPos;

		sumBox(bandId, stackId, cand, candPos);

		std::list<char> candUnique = cand;
		candUnique.sort();
		candUnique.unique();

		for (std::list<char>::iterator it = candUnique.begin(); it != candUnique.end(); ++it) {

			std::list<uint16_t> candRowId;
			std::list<char>::iterator candIt = cand.begin();

			do {
				candIt = std::find(candIt, cand.end(), *it);
				if (cand.end() == candIt)
					break;

				const int distance = std::distance(cand.begin(), candIt);
				candRowId.push_back(candPos[distance].rowId);

			} while (++candIt != cand.end());

			candRowId.unique();
			if (1 == candRowId.size()) {				
				resultStack += removeInRow(candRowId.front(), stackId, *it);
			}
		}

		if (resultStack) {

			resultStack += checkBox(bandId, (stackId + 1) % 3);
			resultStack += checkBox(bandId, (stackId + 2) % 3);
		}

		resultStack += result;
	}

	return result;
}

/**
 * Intersection applied to all the bands of the grid sequentially
 */
uint32_t CSudokuGrid::checkBands(const uint16_t bandFirstId, const uint16_t bandLastId)
{
	assert(bandFirstId < NROF_BANDS);
	assert(bandLastId < NROF_BANDS);
	assert(bandFirstId < bandLastId);

	uint32_t result = 0;

	for (uint16_t bandId = bandFirstId; bandId <= bandLastId; bandId++)
		result += checkBand(bandId);

	return result;
}

/**
 * Also known as 'intersection'. If a candidate occurs twice or three times in just one column,
 * then we can remove it from the other cells of the stack.
 */
uint32_t CSudokuGrid::checkStack(const uint16_t stackId)
{
	assert(stackId < NROF_STACKS);

	uint32_t result = 0;

	for (uint16_t bandId = 0; bandId < NROF_BANDS; bandId++) {

		uint32_t resultBand = 0;

		std::list<char> cand;
		std::vector<cellPos_t> candPos;

		sumBox(bandId, stackId, cand, candPos);

		std::list<char> candUnique = cand;
		candUnique.sort();
		candUnique.unique();

		for (std::list<char>::iterator it = candUnique.begin(); it != candUnique.end(); ++it) {

			std::list<uint16_t> candColId;
			std::list<char>::iterator candIt = cand.begin();

			do {
				candIt = std::find(candIt, cand.end(), *it);
				if (cand.end() == candIt)
					break;

				const int distance = std::distance(cand.begin(), candIt);
				candColId.push_back(candPos[distance].colId);

			} while (++candIt != cand.end());

			candColId.unique();
			if (1 == candColId.size()) {
				resultBand += removeInCol(candColId.front(), bandId, *it);
			}
		}

		if (resultBand) {

			resultBand += checkBox((bandId + 1) % 3, stackId);
			resultBand += checkBox((bandId + 2) % 3, stackId);
		}

		resultBand += result;
	}

	return result;
}

/**
 * Intersection applied to all the stacks of the grid sequentially
 */
uint32_t CSudokuGrid::checkStacks(const uint16_t stackFirstId, const uint16_t stackLastId)
{
	assert(stackFirstId < NROF_STACKS);
	assert(stackLastId < NROF_STACKS);
	assert(stackFirstId < stackLastId);

	uint32_t result = 0;

	for (uint16_t stackId = stackFirstId; stackId <= stackLastId; stackId++)
		result += checkStack(stackId);

	return result;
}

/**
 * Performs all previous analysis (basic) techniques in other to remove candidates from the cells iterativelly
 * and checks for the validity of the grid
 */
int CSudokuGrid::checkGrid(uint32_t &iter)
{
	int retVal = VALID_NOT_SOLVED;
	uint32_t result;

	do {
		result = 0;

		result += checkRows();
		result += checkColumns();
		result += checkBoxes();
		result += checkBands();
		result += checkStacks();

		iter++;

	} while (result);

	//print();
	const bool solved = isSolved();

	if (solved) {

		std::cout << "Puzzle solved!" << std::endl;
		retVal = VALID_SOLVED;
		
		print();
	}
	else {

		//dump();
		//std::cout << "Not solved :(" << std::endl;

		if (false == IsGridValid()) {

			//std::cout << "Invalid Grid!" << std::endl;
			retVal = NOT_VALID;
		}
	}

	return retVal;
}

/**
 * Solves the Sudoku grid first by trying to use the analysis (basic) techniques. In case these do not solve the grid,
 * all possible combinations are checked in a brute force approach that finally gives the solution to the grid
 */
int CSudokuGrid::Solve()
{
	uint32_t iter = 0;
	int retVal = VALID_NOT_SOLVED;

	retVal = checkGrid(iter);

	if (VALID_NOT_SOLVED == retVal) {
		retVal = bruteForce(iter);
	}

	return iter;
}

/**
 * Verifies if the Sudoku grid is solved
 */
bool CSudokuGrid::isSolved()
{
	bool result = true;

	for (uint32_t rowId = 0; rowId < NROF_ROWS; rowId++) {

		for (uint32_t colId = 0; colId < NROF_COLS; colId++) {
			
			if (1 < cells[rowId][colId].size()) {
				
				return false;
			}
		}
	}
	return IsGridValid();
}

/**
 * Prints the Sudoku grid layout in boxes
 */
int CSudokuGrid::print()
{
	std::cout << endl;
	for (uint16_t rowId = 0; rowId < NROF_ROWS; rowId++) {

		if (0 == (rowId % 3)) {
			std::cout << endl;
		}

		for (uint16_t stackId = 0; stackId < NROF_STACKS; stackId++) {

			std::cout << (char)((1 == cells[rowId][stackId*NROF_STACKS + 0].size()) ? cells[rowId][stackId*NROF_STACKS + 0].front() : 46) << " "
				      << (char)((1 == cells[rowId][stackId*NROF_STACKS + 1].size()) ? cells[rowId][stackId*NROF_STACKS + 1].front() : 46) << " "
				      << (char)((1 == cells[rowId][stackId*NROF_STACKS + 2].size()) ? cells[rowId][stackId*NROF_STACKS + 2].front() : 46) << " " 
				      << "\t";
		}
		
		std::cout << endl;
	}

	std::cout << endl;
	return 0;
}

/**
 * Prints all cell lists. This allows to see all non assigned candidates
 */
void CSudokuGrid::dump()
{
	std::cout << endl;

	for (uint16_t bandId = 0; bandId < (NROF_ROWS / NROF_BANDS); bandId++) {

		for (uint16_t stackId = 0; stackId < (NROF_COLS / NROF_STACKS); stackId++) {
			dumpBox(bandId, stackId);

			std::cout << endl;
		}
	}
}

/**
 * Prints cell's list
 */
void CSudokuGrid::dumpCell(const uint16_t rowId, const uint16_t colId) 
{
	assert(rowId < NROF_ROWS);
	assert(colId < NROF_COLS);

	std::cout << "[" << rowId << "]" << "[" << colId << "] : ";

	for (std::list<char>::iterator it = cells[rowId][colId].begin(); it != cells[rowId][colId].end(); ++it)
		std::cout << ' ' << *it;

	std::cout << endl;
}

/**
 * Prints all cells lists within a row
 */
void CSudokuGrid::dumpRow(const uint16_t rowId)
{
	assert(rowId < NROF_ROWS);

	for (uint16_t colId = 0; colId < NROF_COLS; colId++) {
		dumpCell(rowId, colId);
	}
}

/**
 * Prints all cells lists within a column
 */
void CSudokuGrid::dumpCol(const uint16_t colId)
{
	assert(colId < NROF_ROWS);

	for (uint16_t rowId = 0; rowId < NROF_ROWS; rowId++) {
		dumpCell(rowId, colId);
	}
}

/**
 * Prints all cells lists within a box
 */
void CSudokuGrid::dumpBox(const uint16_t bandId, const uint16_t stackId)
{
	assert(bandId < NROF_BANDS);
	assert(stackId < NROF_STACKS);

	for (uint16_t rowId = 0; rowId < (NROF_ROWS / NROF_BANDS); rowId++) {

		for (uint16_t colId = 0; colId < (NROF_COLS / NROF_STACKS); colId++) {

			const uint16_t _rowId = bandId * (NROF_ROWS / NROF_BANDS) + rowId;
			const uint16_t _colId = stackId * (NROF_COLS / NROF_STACKS) + colId;
			
			dumpCell(_rowId, _colId);
		}
	}
}

/**
 * Locates the box with less combined candidates ('best box') and the cell within the box with less candidates ('best cell'). 
 * Then, it will assign the first candidate for the 'best cell' as its value and will try to solve the grid by appliying 
 * the basic techniques. In case it is not solved but it is still a valid grid, it will continue recursively the procedure till
 * the grid is solved or invalid.
 */
int CSudokuGrid::bruteForce(uint32_t &iter)
{
	size_t size;
	uint16_t bandId, stackId;
	uint16_t rowId, colId;
	
	CSudokuGrid gridCpy;
	std::list<char> cellCpy;
	std::list<char>::iterator it;

	int retVal = VALID_NOT_SOLVED;


	bestBox(bandId, stackId, size);
	bestCell(bandId, stackId, rowId, colId, size);

	cellCpy = getCell(rowId, colId);
		
	for (std::list<char>::iterator it = cellCpy.begin(); it != cellCpy.end(); ++it) {

		gridCpy = *this;
		gridCpy.assign(rowId, colId, *it);

		retVal = gridCpy.checkGrid(iter);

		if (VALID_SOLVED == retVal) {
			return retVal;
		}

		if (VALID_NOT_SOLVED == retVal) {

			//gridCpy.print();
			gridCpy.bruteForce(iter);
		}
	}
		
	return retVal;
}

/**
 * Return the list of candidates for a given cell
 */
std::list<char> CSudokuGrid::getCell(const uint16_t rowId, const uint16_t colId) const
{
	return cells[rowId][colId];
}

/**
 * Overloading of '=' operator for the CSudokuGrid class 
 */
CSudokuGrid CSudokuGrid::operator=(const CSudokuGrid & grid)
{
	for (uint16_t rowId = 0; rowId < NROF_ROWS; rowId++) {

		for (uint16_t colId = 0; colId < NROF_COLS; colId++) {
			cells[rowId][colId] = grid.getCell(rowId, colId);
		}
	}
	return *this;
}

/**
 * Verifies that a given row still fulfills the Sudoku rules
 */
bool CSudokuGrid::IsRowValid(const uint16_t rowId)
{
	assert(rowId < NROF_ROWS);

	bool result = true;
	std::list<char> val;

	for (uint16_t colId = 0; colId < NROF_COLS; colId++) {

		if(1 == cells[rowId][colId].size()){
			val.push_back(cells[rowId][colId].front());
		}
	}

	val.sort();
	for (std::list<char>::iterator it = val.begin(); it != val.end(); ++it) {

		int count = std::count(it, val.end(), *it);
		if (1 < count) {

			//std::cout << "Wrong Row(" << rowId << ") " << *it << " appears " << count << " times" << endl;
			result = false;
		}
	}

	return result;
}

/**
 * Verifies that a given column still fulfills the Sudoku rules
 */
bool CSudokuGrid::IsColValid(const uint16_t colId)
{
	assert(colId < NROF_COLS);

	bool result = true;
	std::list<char> val;

	for (uint16_t rowId = 0; rowId < NROF_COLS; rowId++) {

		if (1 == cells[rowId][colId].size()) {
			val.push_back(cells[rowId][colId].front());
		}
	}

	val.sort();
	for (std::list<char>::iterator it = val.begin(); it != val.end(); ++it) {

		int count = std::count(it, val.end(), *it);
		if (1 < count) {

			//std::cout << "Wrong Col(" << colId << ") " << *it << " appears " << count << " times" << endl;
			result = false;
		}
	}

	return result;
}

/**
 * Verifies that a given box still fulfills the Sudoku rules
 */
bool CSudokuGrid::IsBoxValid(const uint16_t bandId, const uint16_t stackId)
{
	assert(bandId < NROF_BANDS);
	assert(stackId < NROF_STACKS);

	bool result = true;
	std::list<char> val;

	for (uint16_t rowId = 0; rowId < (NROF_ROWS / NROF_BANDS); rowId++) {

		for (uint16_t colId = 0; colId < (NROF_COLS / NROF_STACKS); colId++) {

			const cellPos_t cellPos{ (uint16_t)(bandId * (NROF_ROWS / NROF_BANDS) + rowId), (uint16_t)(stackId * (NROF_COLS / NROF_STACKS) + colId) };

			if (1 == cells[cellPos.rowId][cellPos.colId].size()) 
				val.push_back(cells[cellPos.rowId][cellPos.colId].front());
		}
	}

	val.sort();
	for (std::list<char>::iterator it = val.begin(); it != val.end(); ++it) {

		int count = std::count(it, val.end(), *it);
		if (1 < count) {

			//std::cout << "Wrong Box(" << bandId << ", " << stackId << ") " << *it << " appears " << count << " times" << endl;
			result = false;
		}
	}

	return result;
}

/**
 * Verifies that the grid still fulfills the Sudoku rules
 */
bool CSudokuGrid::IsGridValid()
{
	bool result = true;

	for(uint16_t rowId = 0; rowId < NROF_ROWS; rowId++) {
		
		if (false == IsRowValid(rowId)) {
			result = false;
		}
	}

	for (uint16_t colId = 0; colId < NROF_COLS; colId++) {

		if (false == IsColValid(colId)) {
			result = false;
		}
	}

	for (uint16_t bandId = 0; bandId < NROF_BANDS; bandId++) {

		for (uint16_t stackId = 0; stackId < NROF_STACKS; stackId++) {

			if(false == IsBoxValid(bandId, stackId)) {
				result = false;
			}
		}
	}

	return result;
}

/**
 * Removes all candidates provided in argument 'val' which coordinates are provided in 'candPos' from the grid
 */
uint32_t CSudokuGrid::remove(std::vector<char> &val, std::vector<cellPos_t> &candPos)
{
	uint32_t result = 0;

	for (std::vector<cellPos_t>::iterator itId = candPos.begin(); itId != candPos.end(); ++itId) {

		const uint16_t rowId = (*itId).rowId;
		const uint16_t colId = (*itId).colId;

		for (std::vector<char>::iterator it = val.begin(); it != val.end(); ++it) {

			if (1 < cells[rowId][colId].size()) {

				cells[rowId][colId].remove(*it);

				if (1 == cells[rowId][colId].size()) {
					result++;
				}
				else {
					cells[rowId][colId].sort();
				}
			}
		}
	}

	return result;
}

/**
 * Removes all candidates equal to 'val' from a given row 
 */
uint32_t CSudokuGrid::removeInRow(const uint16_t rowId, const uint16_t stackId, const char val)
{
	assert(rowId < NROF_ROWS);
	assert(stackId < NROF_STACKS);

	uint32_t result = 0;

	for (uint16_t colId = 0; colId < NROF_COLS; colId++) {

		if ((colId / NROF_STACKS) != stackId) {

			const size_t size = cells[rowId][colId].size();
			if (1 == size)
				continue;

			cells[rowId][colId].remove(val);

			if(cells[rowId][colId].size() < size)
				result++;
		}
	}

	return result;
}

/**
 * Removes all candidates equal to 'val' from a given column 
 */
uint32_t CSudokuGrid::removeInCol(const uint16_t colId, const uint16_t bandId, const char val)
{
	assert(colId < NROF_COLS);
	assert(bandId < NROF_BANDS);

	uint32_t result = 0;

	for (uint16_t rowId = 0; rowId < NROF_ROWS; rowId++) {

		if ((rowId / NROF_BANDS) != bandId) {

			const size_t size = cells[rowId][colId].size();
			if (1 == size)
				continue;

			cells[rowId][colId].remove(val);

			if (cells[rowId][colId].size() < size)
				result++;
		}
	}

	return result;
}

/**
 *  Provides all candidates and their coordinates from a given box
 */
void CSudokuGrid::sumBox(const uint16_t bandId, const uint16_t stackId, std::list<char>& cand, std::vector<cellPos_t>& candPos) 
{
	for (uint16_t rowId = 0; rowId < (NROF_ROWS / NROF_BANDS); rowId++) {

		for (uint16_t colId = 0; colId < (NROF_COLS / NROF_STACKS); colId++) {

			const cellPos_t cellPos{ (uint16_t)(bandId * (NROF_ROWS / NROF_BANDS) + rowId), (uint16_t)(stackId * (NROF_COLS / NROF_STACKS) + colId) };

			if (1 < cells[cellPos.rowId][cellPos.colId].size()) {

				std::list<char> &cell = cells[cellPos.rowId][cellPos.colId];
				cand.insert(cand.end(), cell.begin(), cell.end());

				for (std::list<char>::iterator it = cell.begin(); it != cell.end(); ++it) {
					candPos.push_back({ cellPos.rowId, cellPos.colId });
				}
			}
		}
	}
}

/**
 * Provides the box with the less candidates 
 */
void CSudokuGrid::bestBox(uint16_t &bestBandId, uint16_t &bestStackId, size_t &bestSize)
{
	bestBandId = 0;
	bestStackId = 0;
	bestSize = NROF_ROWS;

	for (uint16_t bandId = 0; bandId < NROF_BANDS; bandId++) {

		for (uint16_t stackId = 0; stackId < NROF_STACKS; stackId++) {

			std::list<char> cand;
			std::vector<cellPos_t> candPos;

			sumBox(bandId, stackId, cand, candPos);
			
			if (0 == cand.size()) {
				continue;
			}

			std::list<char> candUnique = cand;
			candUnique.sort();
			candUnique.unique();

			const size_t size = candUnique.size();
			if(size < bestSize)
			{
				bestBandId = bandId;
				bestStackId = stackId;
				bestSize = size;
			}
		}
	}
}

/**
 * Provides the cell within box with the less candidates 
 */
void CSudokuGrid::bestCell(const uint16_t bandId, const uint16_t stackId, uint16_t & bestRowId, uint16_t & bestColId, size_t & bestSize)
{
	assert(bandId < NROF_BANDS);
	assert(stackId < NROF_STACKS);

	bestRowId = 0;
	bestColId = 0;
	bestSize = NROF_BANDS;

	for (uint16_t rowId = 0; rowId < (NROF_ROWS / NROF_BANDS); rowId++) {

		for (uint16_t colId = 0; colId < (NROF_COLS / NROF_STACKS); colId++) {

			const cellPos_t cellPos{ (uint16_t)(bandId * (NROF_ROWS / NROF_BANDS) + rowId), (uint16_t)(stackId * (NROF_COLS / NROF_STACKS) + colId) };
			const size_t size = cells[cellPos.rowId][cellPos.colId].size();

			if (1 == size) {
				continue;
			}
			
			if (size < bestSize) {

				bestRowId = rowId;
				bestColId = colId;
				bestSize = size;
			}
		}
	}

	bestRowId += bandId * (NROF_ROWS / NROF_BANDS);
	bestColId += stackId * (NROF_COLS / NROF_STACKS);
}
