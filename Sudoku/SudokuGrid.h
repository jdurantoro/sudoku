#pragma once

#include <list>
#include <cassert>
#include <vector>


#define NROF_ROWS (9)
#define NROF_COLS (9)

#define NROF_BANDS (3)
#define NROF_STACKS (3)

// Resulting states of Sudoku solver
enum { NOT_VALID = -1, VALID_NOT_SOLVED = 0, VALID_SOLVED = 1};

// Levels of difficulty for Sudoku grid generation
enum { EASY = 0, MEDIUM = 1, HARD = 2, SAMURAI = 3};
#define NROF_LEVELS (SAMURAI + 1)

// Generatr Mask
// Masks to be applied to Sudoku grids depending on difficulty
//  '1' represents the value will be not masked
//  '0' will mask the value
const uint8_t GEN_MASK[NROF_LEVELS][NROF_ROWS][NROF_COLS] = {

	{{ 1,1,0,0,0,0,0,1,1 }, // easy
	 { 1,0,0,1,1,1,0,0,1 },
	 { 0,0,0,0,0,0,0,0,0 },
	 { 0,1,1,1,0,1,1,1,0 },
	 { 0,0,0,1,0,1,0,0,0 },
	 { 0,1,1,1,0,1,1,1,0 },
	 { 0,0,0,0,0,0,0,0,0 },
	 { 1,0,0,1,1,1,0,0,1 },
	 { 1,1,0,0,0,0,0,1,1 }},

	{{ 1,0,0,0,1,0,0,0,1 }, // medium
	 { 1,0,0,1,1,1,0,0,1 },
	 { 0,0,1,0,0,0,1,0,0 },
	 { 0,1,0,0,1,0,0,1,0 },
	 { 0,0,0,1,0,1,0,0,0 },
	 { 0,1,0,0,1,0,0,1,0 },
	 { 0,0,1,0,0,0,1,0,0 },
	 { 1,0,0,1,1,1,0,0,1 },
	 { 1,0,0,0,1,0,0,0,1 }},

	{{ 0,1,1,1,0,0,1,0,0 }, // hard
	 { 1,0,0,0,1,0,0,0,1 },
	 { 0,0,0,0,0,0,0,0,0 },
	 { 0,0,0,1,1,0,0,1,0 },
	 { 1,1,0,0,0,0,0,1,1 },
	 { 0,1,0,0,1,1,0,0,0 },
	 { 0,0,0,0,0,0,0,0,0 },
	 { 1,0,0,0,1,0,0,0,1 },
	 { 0,0,1,0,0,1,1,1,0 }},
	
	{{ 1,0,0,0,0,0,1,0,1 }, // samurai
	 { 0,0,1,1,0,0,1,0,0 },
	 { 0,0,0,1,0,0,0,1,0 },
	 { 0,1,0,1,0,1,0,0,0 },
	 { 1,0,0,0,0,0,0,0,1 },
	 { 0,0,0,1,0,1,0,1,0 },
	 { 0,1,0,0,0,1,0,0,0 },
	 { 0,0,1,0,0,1,1,0,0 },
	 { 1,0,1,0,0,0,0,0,1 }}
};

// Repeition Pattern
// For each position, it represent the number of times
// the value appears in the generated grid according to the associated GEN_MASK. 
// 
// In this case, the position does not imply natural order of values.
// That is, position 0 does not imply value '1', 
// position 1 does not imply value '2', ...
const uint8_t REP_PATT[NROF_LEVELS][NROF_ROWS] = {

	{1, 2, 3, 3, 4, 4, 5, 5, 5}, // easy

	{2, 2, 3, 3, 3, 3, 4, 4, 4}, // medium

	{1, 1, 2, 2, 3, 3, 4, 4, 4}, // hard

	{2, 2, 2, 2, 3, 3, 3, 3, 4}  // samurai
};

// List containing all possible values for a cell
const std::list<char> allValues({ 49, 50, 51, 52, 53, 54, 55, 56, 57 }); // '1' = 49, '9' = 57


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
	int Search(uint32_t &iter);

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

	bool generate(const uint8_t level = EASY);

private:
	std::list<char> m_cells[NROF_ROWS][NROF_COLS];

private:
	typedef struct { uint16_t rowId; uint16_t colId; } cellPos_t;

	void pushBack(const uint16_t rowId, const uint16_t colId, const char value);
	void notAssigned(const uint16_t rowId, const uint16_t colId);

	uint32_t remove(std::vector<char> &assigned, std::vector<cellPos_t> &nonAssignedPos);
	uint32_t removeInRow(const uint16_t rowId, const uint16_t stackId, const char val);
	uint32_t removeInCol(const uint16_t colId, const uint16_t bandId, const char val);
	
	void sumBox(const uint16_t bandId, const uint16_t stackId, std::list<char> &cand, std::vector<cellPos_t> &candPos);
	
	void searchBox(uint16_t &bestBandId, uint16_t &bestStackId, size_t &bestSize);
	void searchCell(const uint16_t bandId, const uint16_t stackId, uint16_t &bestRowId, uint16_t &bestColId, size_t &bestSize);

};

