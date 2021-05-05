#include "XMLParser.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <rapidxml/rapidxml.hpp>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace rapidxml;


xml_document<> doc;
xml_node<>* root_node = NULL;
xml_node<>* grid_node = NULL;
xml_node<>* piece_node = NULL;


/*
* Good resource
* https://linuxhint.com/parse_xml_in_c__/
* 
*/

void XMLParser::parsePoints(string file, string frame)
{
    // Read the VTK file
    ifstream theFile(file);
    vector<char> buffer((istreambuf_iterator<char>(theFile)), 
        istreambuf_iterator<char>());
    buffer.push_back('\0');

    // Parse the buffer
    doc.parse<0>(&buffer[0]);

    // Find out the root node
    root_node = doc.first_node("VTKFile");
    grid_node = root_node->first_node("UnstructuredGrid");
    piece_node = grid_node->first_node("Piece");

    // temporary node
    xml_node<>* print_node = piece_node->first_node("Points");

    // create file
    ofstream MyFile("points-" + frame + ".txt");

    // write to file
    MyFile << print_node->first_node("DataArray")->value();

    // Close file
    MyFile.close();
}

void XMLParser::parseTemperatures(string file, string frame)
{
    // Read the sample.xml file
    ifstream theFile(file);
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');

    // Parse the buffer
    doc.parse<0>(&buffer[0]);

    // Find out the root node
    root_node = doc.first_node("VTKFile");
    grid_node = root_node->first_node("UnstructuredGrid");
    piece_node = grid_node->first_node("Piece");

    // temporary node
    xml_node<>* print_node = piece_node->first_node("PointData");

    // create file
    ofstream MyFile("UV-" + frame + ".txt");

    // write to file
    MyFile << print_node->first_node("DataArray")->value();

    // Close file
    MyFile.close();
}

void XMLParser::parseIndices(string file, string frame)
{
    // Read the sample.xml file
    ifstream theFile(file);
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');

    // Parse the buffer
    doc.parse<0>(&buffer[0]);

    // Find out the root node
    root_node = doc.first_node("VTKFile");
    grid_node = root_node->first_node("UnstructuredGrid");
    piece_node = grid_node->first_node("Piece");

    // temporary node
    xml_node<>* print_node = piece_node->first_node("Cells");

    // create file
    ofstream MyFile("connectivity-" + frame + ".txt");

    // write to file
    MyFile << print_node->first_node("DataArray")->value();

    // Close file
    MyFile.close();
}

void XMLParser::parseXMLFile(string file, string frame)
{
    cout << "\nParsing VTK data ("+file+")....." << endl;
    
    // change frame number to triple digit format
    // e.g. 000, 001, 146, 859
    
    parsePoints(file, frame);
    parseIndices(file, frame);
    parseTemperatures(file, frame);

    // Iterate over the student nodes
    //for (student_node; student_node; student_node = student_node->next_sibling())
    //{
    //    cout << student_node << endl;
    //    cout << "\nStudent Type =   " << student_node->first_attribute()->value();
    //    cout << endl;

    //    // Interate over the Student Names
    //    for (xml_node<>* student_name_node = student_node->first_node("Name"); student_name_node; student_name_node = student_name_node->next_sibling())
    //    {
    //        cout << "Student Name =   " << student_name_node->value();
    //        cout << endl;
    //    }
    //    cout << endl;
    //}

}