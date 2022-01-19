#pragma once
#include <string>
class Component
{
public:
	Component();

	std::string Name()
	{
		return name;
	}

	~Component();

protected:
	std::string name;

private:



};

