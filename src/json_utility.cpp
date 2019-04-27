/*
MIT License

Copyright (c) 2017 SAE Institute Switzerland AG

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <json_utility.h>

#include <fstream>
#include <string>
#include <iostream>


bool IsJsonValueNumeric(const json::value_type & jsonValue)
{
	return jsonValue.type() == json::value_t::number_float || 
		   jsonValue.type() == json::value_t::number_integer || 
		   jsonValue.type() == json::value_t::number_unsigned;
}

bool CheckJsonExists(const json & jsonObject, std::string parameterName)
{
	return jsonObject.find(parameterName) != jsonObject.end();
}

bool CheckJsonParameter(const json& jsonObject, std::string parameterName, json::value_t expectedType)
{
	return CheckJsonExists(jsonObject, parameterName) && jsonObject[parameterName].type() == expectedType;
}

bool CheckJsonNumber(const json& jsonObject, std::string parameterName)
{
	return CheckJsonParameter(jsonObject, parameterName, json::value_t::number_float) or
		   CheckJsonParameter(jsonObject, parameterName, json::value_t::number_integer) or
		   CheckJsonParameter(jsonObject, parameterName, json::value_t::number_unsigned);
}


std::unique_ptr<json> LoadJson(std::string jsonPath)
{
	std::ifstream jsonFile(jsonPath.c_str());
	if (jsonFile.peek() == std::ifstream::traits_type::eof())
	{
		{
			std::cerr << "[JSON ERROR] EMPTY JSON FILE at: " << jsonPath << "\n";
		}
		return nullptr;
	}
	std::unique_ptr<json> jsonContent = std::make_unique<json>();
	try
	{
		jsonFile >> *jsonContent;
	}
	catch (json::parse_error& e)
	{
		{
            std::cerr << "THE FILE: " << jsonPath << " IS NOT JSON\n" << e.what() << "\n";
		}
		return nullptr;
	}
	return jsonContent;
}

glm::vec3 ConvertVec3FromJson(const json& vec3Json)
{
    glm::vec3 vector3;
    vector3.x = vec3Json[0];
    vector3.y = vec3Json[1];
    vector3.z = vec3Json[2];
    return vector3;
}

