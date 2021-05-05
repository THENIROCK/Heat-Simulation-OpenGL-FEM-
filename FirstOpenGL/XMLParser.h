#pragma once
#include <string>

class XMLParser
{
public:
	static void parsePoints(std::string file, std::string frame);
	static void parseTemperatures(std::string file, std::string frame);
	static void parseIndices(std::string file, std::string frame);
	static void parseXMLFile(std::string file, std::string frame);
};