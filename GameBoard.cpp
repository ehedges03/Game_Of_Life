#include "pch.h"
#include "GameBoard.h"
#include "WrappedPoint.h"
#include <iostream>

/*
GameBoard method definitions
*/

GameBoard::GameBoard(uint32_t chunkSideSize, uint32_t chunksPerRow) :
	m_cells(BitArray(chunkSideSize* chunkSideSize* chunksPerRow* chunksPerRow)),
	c_sideLength(chunkSideSize * chunksPerRow), c_chunkWidth(chunkSideSize),
	c_chunksPerRow(chunksPerRow) {

	uint32_t sideMax = chunkSideSize - 1;

	m_variants[0] = { 0, 0, sideMax, sideMax, 1 };
	m_variants[1] = { sideMax, 0, sideMax, 0, 0 };
	m_variants[2] = { 0, sideMax, 0, sideMax, 1 };
	m_variants[3] = { sideMax, sideMax, 0, 0, 0 };

	uint32_t currX = 0;
	uint32_t currY = 0;
	m_chunks = new Chunk**[chunksPerRow];
	for (uint32_t i = 0; i < chunksPerRow; i++) {
		m_chunks[i] = new Chunk*[chunksPerRow];
		for (uint32_t j = 0; j < chunksPerRow; j++) {
			// TODO: Should I instead just pass copies of the variant so it does not have to dereference
			m_chunks[i][j] = new Chunk(*this, m_variants[(i % 2) * 2 + (j % 2)], currX, currY);
			currX += chunkSideSize;
		}
		currX = 0;
		currY += chunkSideSize;
	}
}

GameBoard::~GameBoard() {
	for (uint32_t i = 0; i < c_chunksPerRow; i++) {
		for (uint32_t j = 0; j < c_chunksPerRow; j++) {
			delete m_chunks[i][j];
		}
		delete m_chunks[i];
	}
	delete m_chunks;
}

void GameBoard::setPoint(uint32_t x, uint32_t y, bool value) {
	m_cells.set(xyToIndex(x, y), value);
}

bool GameBoard::getPoint(int x, int y) {
	return m_cells.get(xyToIndex(x, y));
}

uint32_t GameBoard::xyToIndex(int x, int y) {

	WrappedPoint wp({x, y}, {c_sideLength, c_sideLength});
	return wp.x() + wp.y() * c_sideLength;

}

uint8_t GameBoard::countNeighbors(uint32_t x, uint32_t y) {
	// Would it be faster or slower to check if the value is 4 to avoid checking all pixels
	// The if statements could lead to a slower processing time

	// counts neighbors row by row
	uint8_t count = 0;
	uint32_t leftPoint = xyToIndex(x - 1, y - 1);
	count += m_cells.get(leftPoint);
	count += m_cells.get(leftPoint + 1);
	count += m_cells.get(leftPoint + 2);
	leftPoint += c_sideLength;
	count += m_cells.get(leftPoint);
	count += m_cells.get(leftPoint + 2);
	leftPoint += c_sideLength;
	count += m_cells.get(leftPoint);
	count += m_cells.get(leftPoint + 1);
	return count + m_cells.get(leftPoint + 2);
}

/*
Chunk method definitions
*/

GameBoard::Chunk::Chunk(GameBoard& gb, const Variant& variant, uint32_t offsetX, uint32_t offsetY) :
	m_gb(gb),
	c_offsetX(offsetX),
	c_offsetY(offsetY),
	m_variant(variant),
	m_border(BitArray(gb.c_chunkWidth * 2)) {
	std::cout << "X: " << offsetX << " Y: " << offsetY << " test: " << m_variant.ySide << ' ' << m_variant.xSide << '\n';
}

bool GameBoard::Chunk::calcNextCellStatus(int x, int y) {
	x += c_offsetX; y += c_offsetY;
	bool alive = m_gb.getPoint(x, y);
	uint8_t neighbors = m_gb.countNeighbors(x, y);

	return neighbors == 3 || (alive && neighbors == 2);
}

void GameBoard::Chunk::processChunk() {
	uint32_t maxwh = m_gb.c_chunkWidth;    // compiler will take this out and it cleans up the code a little
	BitArray rowBuffers[2] = { BitArray(maxwh), BitArray(maxwh) };
	uint8_t hotBuffer = 1;
	int y, x;

	// put the first row into the buffer
	for (x = 0; x < maxwh; x++) {
		rowBuffers[0].set(x, calcNextCellStatus(x, 0));
	}

	for (y = 0; y < maxwh; y++) {
		// write upcoming line into the buffer (y+1)
		if ((y + 1) < maxwh) {
			for (x = 0; x < maxwh; x++) {
				rowBuffers[hotBuffer].set(x, calcNextCellStatus(x, y + 1));
			}
		}
		
		hotBuffer ^= 1;    // switch active buffer before writing 

		// write hot buffer the the board (y)
		for (x = 0; x < maxwh; x++) {
			bool value = rowBuffers[hotBuffer].get(x);
			if (!addedToBorderBuffer(x, y, value)) {
				m_gb.setPoint(x + c_offsetX, y + c_offsetY, value);
			}
		}
		// row y is written to the board
	}
}

void GameBoard::Chunk::writeBorder() {
	uint32_t i;
	for (i = 0; i < m_gb.c_chunkWidth; i++) {
		m_gb.setPoint(i + c_offsetX, m_variant.ySide + c_offsetY, m_border.get(i));
	}
	for (; i < m_border.size() - 1; i++) {
		m_gb.setPoint(m_variant.xSide + c_offsetX, i - m_gb.c_chunkWidth + m_variant.offset, m_border.get(i));
	}
	m_gb.setPoint(m_variant.xCorner + c_offsetX, m_variant.yCorner + c_offsetY, m_border.get(i));
}

bool GameBoard::Chunk::addedToBorderBuffer(int x, int y, bool value) {
	bool added = false;
	if (y == m_variant.ySide) {
		m_border.set(x, value);
		added = true;
	}
	else if (x == m_variant.xSide) {
		m_border.set(y + m_gb.c_chunkWidth - m_variant.offset, value);
		added = true;
	}
	else if (x == m_variant.xCorner && y == m_variant.yCorner) {
		m_border.set(m_gb.c_chunkWidth * 2 - 1, value);
		added = true;
	}

	return added;
}