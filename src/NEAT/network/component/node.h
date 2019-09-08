#pragma once

#include <string>

enum NodeType
{
	Sensor,
	Hidden,
	Output
};

//Represents a node gene
struct Node
{
	int index;
	NodeType type;
	std::string name; //Used for drawing in sensor and output nodes;
};