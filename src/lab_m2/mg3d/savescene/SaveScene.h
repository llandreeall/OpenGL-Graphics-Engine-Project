#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include "../GameObject.h"
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/detail/file_parser_error.hpp>                                 

using std::cout;
using std::endl;
using std::string;


static class SaveScene
{
public:
	static void WriteFile(std::vector<GameObject*> roots, string path, glm::vec3 light);
	static std::vector<GameObject*> LoadScene(string path, int& id, std::unordered_map<std::string, Mesh*> meshes);
};

