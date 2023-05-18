#include "pch.h"
#include "GameBoard.h"
#include "WrappedPoint.h"
#include <iostream>

/*
GameBoard method definitions
*/

GameBoard::GameBoard(uint32_t chunkSideSize, uint32_t chunksPerRow) :
	m_points(BitArray(chunkSideSize* chunkSideSize* chunksPerRow* chunksPerRow)),
	c_sideLength(chunkSideSize * chunksPerRow), c_chunkSideSize(chunkSideSize),
	c_chunksPerRow(chunksPerRow) {

	uint32_t currX = 0;
	uint32_t currY = 0;
	m_chunks = (Chunk***) new Chunk*[chunksPerRow * chunksPerRow];
	for (uint32_t i = 0; i < chunksPerRow; i++) {
		for (uint32_t j = 0; j < chunksPerRow; j++) {
			m_chunks[i][j] = new Chunk(*this, currX, currY);
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
	}

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

GameBoard::Chunk::Chunk(GameBoard& gb, uint32_t offsetX, uint32_t offsetY) :
	m_gb(gb),
	c_offsetX(offsetX),
	c_offsetY(offsetY) {}

// TODO: is this needed, or is Chunk::nextPointStatus sufficient
uint8_t GameBoard::Chunk::countNeighbors(uint32_t x, uint32_t y) {
	x += c_offsetX; y += c_offsetY;
	return m_gb.countNeighbors(x, y);
}

bool GameBoard::Chunk::nextPointStatus(int x, int y) {
	x += c_offsetX; y += c_offsetY;
	return m_gb.nextPointStatus(x, y);
}