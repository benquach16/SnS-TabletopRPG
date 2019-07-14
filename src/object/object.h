// An object that can be placed on a level

#pragma once

struct vector2d
{
	int x = 0;
	int y = 0;
	vector2d(int x, int y) : x(x), y(y) {}
};

class Object
{
public:
	Object();
	virtual ~Object();
	vector2d getPosition() const { return m_position; }
	void setPosition(int x, int y) { m_position.x = x; m_position.y = y; }
	void moveDown() { m_position.y++; }
	void moveUp() { m_position.y--; }
	void moveLeft() { m_position.x--; }
	void moveRight() { m_position.x++; }
protected:
	vector2d m_position;

};
