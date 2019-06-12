#pragma once

enum class eLength : unsigned char
{
	Hand,
	Short,
	Medium,
	Long,
	Extended,
	Extreme,
	Ludicrous
};

class Weapon
{
public:
	Weapon();
	~Weapon();
private:
	eLength m_length;
	
};
