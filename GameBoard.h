#pragma once
#include "BitArray.h"

class GameBoard {
public:
	GameBoard(uint32_t chuckSideSize, uint32_t chunksPerRow);
	~GameBoard();

	void setPoint(uint32_t x, uint32_t y, bool value);
	bool getPoint(int x, int y);

	// TODO: returns the next status (alive/dead) of a point on the gameboard
	bool nextPointStatus(int x, int y);

private:
	class Chunk;
	friend class Chunk;

	const uint32_t c_sideLength;
	const uint32_t c_chunkSideSize;
	const uint32_t c_chunksPerRow;
	BitArray m_points;
	Chunk*** m_chunks;

	// helper function used by nextPointStatus
	uint8_t countNeighbors(uint32_t x, uint32_t y);
	
	uint32_t xyToIndex(int x, int y);
};

class GameBoard::Chunk {

public:
	Chunk(GameBoard& gb, uint32_t offsetX, uint32_t offsetY);

	uint8_t countNeighbors(uint32_t x, uint32_t y);
	bool nextPointStatus(int x, int y);

	void processChunk();
	void writeInner(); // Are we writting inner after
	void copyOuter(); // Or are we copying outer before processing

private:
	GameBoard m_gb;
	const uint32_t c_offsetX;
	const uint32_t c_offsetY;
	bool m_processNextFrame = false;
};