#include "pch.h"
#include "GameBoard.h"
#include "WrappedPoint.h"
#include <iostream>

/*
GameBoard method definitions
*/

GameBoard::GameBoard(uint32_t chunkSideSize, uint32_t chunksPerRow) :
	m_points(BitArray(chunkSideSize* chunkSideSize* chunksPerRow* chunksPerRow)),
	c_sideLength(chunkSideSize * chunksPerRow), c_chunkWidth(chunkSideSize),
	c_chunksPerRow(chunksPerRow) {

	uint32_t sideMax = chunkSideSize - 1;

	m_varients[0] = { 0, 0, sideMax, sideMax, 1 };
	m_varients[1] = { sideMax, 0, sideMax, 0, 0 };
	m_varients[2] = { 0, sideMax, 0, sideMax, 1 };
	m_varients[3] = { sideMax, sideMax, 0, 0, 0 };

	uint32_t currX = 0;
	uint32_t currY = 0;
	m_chunks = new Chunk**[chunksPerRow];
	for (uint32_t i = 0; i < chunksPerRow; i++) {
		m_chunks[i] = new Chunk*[chunksPerRow];
		for (uint32_t j = 0; j < chunksPerRow; j++) {
			// TODO: Should I instead just pass copies of the varient so it does not have to dereference
			m_chunks[i][j] = new Chunk(*this, m_varients[(i % 2) * 2 + (j % 2)], currX, currY);
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
	m_points.set(xyToIndex(x, y), value);
}

bool GameBoard::getPoint(int x, int y) {
	return m_points.get(xyToIndex(x, y));
}

uint32_t GameBoard::xyToIndex(int x, int y) {

	WrappedPoint wp({x, y}, {c_sideLength, c_sideLength});
	return wp.x() + wp.y() * c_sideLength;

}

bool GameBoard::nextPointStatus(int x, int y) {
	// TODO
	return false;
}

uint8_t GameBoard::countNeighbors(uint32_t x, uint32_t y) {
	// Would it be faster or slower to check if the value is 4 to avoid checking all pixels
	// The if statements could lead to a slower processing time

	// counts neighbors row by row
	uint8_t count = 0;
	uint32_t leftPoint = xyToIndex(x - 1, y - 1);
	count += m_points.get(leftPoint);
	count += m_points.get(leftPoint + 1);
	count += m_points.get(leftPoint + 2);
	leftPoint += c_sideLength;
	count += m_points.get(leftPoint);
	count += m_points.get(leftPoint + 2);
	leftPoint += c_sideLength;
	count += m_points.get(leftPoint);
	count += m_points.get(leftPoint + 1);
	return count + m_points.get(leftPoint + 2);
}

/*
Chunk method definitions
*/

GameBoard::Chunk::Chunk(GameBoard& gb, const Varient& varient, uint32_t offsetX, uint32_t offsetY) :
	m_gb(gb),
	c_offsetX(offsetX),
	c_offsetY(offsetY),
	m_varient(varient),
	m_border(BitArray(gb.c_chunkWidth * 2)) {
	std::cout << "X: " << offsetX << " Y: " << offsetY << " test: " << m_varient.ySide << ' ' << m_varient.xSide << '\n';
}

// TODO: is this needed, or is Chunk::nextPointStatus sufficient
uint8_t GameBoard::Chunk::countNeighbors(uint32_t x, uint32_t y) {
	x += c_offsetX; y += c_offsetY;
	return m_gb.countNeighbors(x, y);
}

bool GameBoard::Chunk::nextPointStatus(int x, int y) {
	x += c_offsetX; y += c_offsetY;
	return m_gb.nextPointStatus(x, y);
}

void GameBoard::Chunk::processChunk() {
	BitArray rows[2] = { BitArray(m_gb.c_chunkWidth), BitArray(m_gb.c_chunkWidth) };
	bool currRow = true;
	int y, x;
	
	for (x = 0; x < m_gb.c_chunkWidth; x++) {
		rows[0].set(x, countNeighbors(x, 0));
	}

	for (y = 1; y < m_gb.c_chunkWidth; y++) {
		for (x = 0; x < m_gb.c_chunkWidth; x++) {
			rows[currRow].set(y, countNeighbors(x, y));
		}
		currRow = !currRow;
		y--;
		for (x = 0; x < m_gb.c_chunkWidth; x++) {
			bool value = rows[currRow].get(x);
			if (!addedToBorderBuffer(x, y, value)) {
				m_gb.setPoint(x + c_offsetX, y + c_offsetY, value);
			}
		}
		y++;
	}
	currRow = !currRow;
	y--;
	for (x = 0; x < m_gb.c_chunkWidth; x++) {
		bool value = rows[currRow].get(x);
		if (!addedToBorderBuffer(x, y, value)) {
			m_gb.setPoint(x + c_offsetX, y + c_offsetY, value);
		}
	}
}

void GameBoard::Chunk::writeBorder() {
	uint32_t i;
	for (i = 0; i < m_gb.c_chunkWidth; i++) {
		m_gb.setPoint(i + c_offsetX, m_varient.ySide + c_offsetY, m_border.get(i));
	}
	for (; i < m_border.size() - 1; i++) {
		m_gb.setPoint(m_varient.xSide + c_offsetX, i - m_gb.c_chunkWidth + m_varient.offset, m_border.get(i));
	}
	m_gb.setPoint(m_varient.xCorner + c_offsetX, m_varient.yCorner + c_offsetY, m_border.get(i));
}

bool GameBoard::Chunk::addedToBorderBuffer(int x, int y, bool value) {
	bool added = false;
	if (y == m_varient.ySide) {
		m_border.set(x, value);
		added = true;
	}
	else if (x == m_varient.xSide) {
		m_border.set(y + m_gb.c_chunkWidth - m_varient.offset, value);
		added = true;
	}
	else if (x == m_varient.xCorner && y == m_varient.yCorner) {
		m_border.set(m_gb.c_chunkWidth * 2 - 1, value);
		added = true;
	}

	return added;
}