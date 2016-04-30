#pragma once

#include <list>
#include <cassert>
#include <vector>


#define NROF_ROWS (9)
#define NROF_COLS (9)

#define NROF_BANDS (3)
#define NROF_STACKS (3)

enum { NOT_VALID = -1, VALID_NOT_SOLVED = 0, VALID_SOLVED = 1};

class CSudokuGrid
{
public:
	CSudokuGrid();
	~CSudokuGrid();

	CSudokuGrid operator= (const CSudokuGrid &grid);

	bool readGrid(const std::string &fileName);

	void assign(const uint16_t rowId, const uint16_t colId, const char value);
	std::list<char> getCell(const uint16_t rowId, const uint16_t colId) const;
	

	uint32_t checkRow(const uint16_t rowId);
	uint32_t checkRows(const uint16_t rowFirstId = 0, const uint16_t rowLastId = (NROF_ROWS-1));
	uint32_t checkColumn(const uint16_t colId);
	uint32_t checkColumns(const uint16_t colFirstId = 0, const uint16_t colLastId = (NROF_COLS-1));

	uint32_t checkBox(const uint16_t bandId, const uint16_t stackId);
	uint32_t checkBoxes(const uint16_t bandFirstId = 0, const uint16_t bandLastId = (NROF_BANDS - 1), const uint16_t stackFirstId = 0, const uint16_t stackLastId = (NROF_STACKS - 1));

	uint32_t hiddenSingleBox(const uint16_t bandId, const uint16_t stackId);

	uint32_t checkBand(const uint16_t bandId);
	uint32_t checkBands(const uint16_t bandFirstId = 0, const uint16_t bandLastId = (NROF_BANDS - 1));
	uint32_t checkStack(const uint16_t stackId);
	uint32_t checkStacks(const uint16_t stackFirstId = 0, const uint16_t stackLastId = (NROF_STACKS - 1));
	
	int checkGrid(uint32_t &iter);
	int bruteForce(uint32_t &iter);

	int Solve();
	bool isSolved();

	bool IsRowValid(const uint16_t rowId);
	bool IsColValid(const uint16_t colId);
	bool IsBoxValid(const uint16_t bandId, const uint16_t stackId);
	bool IsGridValid();

	int print();

	void dump();
	void dumpCell(const uint16_t rowId, const uint16_t colId);
	void dumpRow(const uint16_t rowId);
	void dumpCol(const uint16_t colId);
	void dumpBox(const uint16_t bandId, const uint16_t stackId);

	

private:
	std::list<char> cells[NROF_ROWS][NROF_COLS];

	const char allValuesAscii[NROF_ROWS] = { 49, 50, 51, 52, 53, 54, 55, 56, 57 }; // '1' = 49, '9' = 57
	const std::list<char> allValues;

private:
	typedef struct { uint16_t rowId; uint16_t colId; } cellPos_t;

	void pushBack(const uint16_t rowId, const uint16_t colId, const char value);
	void notAssigned(const uint16_t rowId, const uint16_t colId);

	uint32_t remove(std::vector<char> &assigned, std::vector<cellPos_t> &nonAssignedPos);
	uint32_t removeInRow(const uint16_t rowId, const uint16_t stackId, const char val);
	uint32_t removeInCol(const uint16_t colId, const uint16_t bandId, const char val);
	
	void sumBox(const uint16_t bandId, const uint16_t stackId, std::list<char> &cand, std::vector<cellPos_t> &candPos);
	
	void bestBox(uint16_t &bestBandId, uint16_t &bestStackId, size_t &bestSize);
	void bestCell(const uint16_t bandId, const uint16_t stackId, uint16_t &bestRowId, uint16_t &bestColId, size_t &bestSize);

};

