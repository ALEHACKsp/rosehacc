#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <memory>

class Config
{
public:
	Config(std::string file_path);

	void SetupValue(std::string name, std::string& value);
	void SetupValue(std::string name, int& value);
	void SetupValue(std::string name, bool& value);
	void SetupValue(std::string name, float& value);
	void SetupValue(std::string name, double& value);

	void Save();
	void Load();

private:
	std::string file_path;

	template<typename T>
	struct Value
	{
		Value(std::string name, T* value)
		{
			this->name = name;
			this->value = value;
		}

		std::string name;
		T* value;
	};

	std::vector<std::unique_ptr<Value<std::string>>> strings;
	std::vector<std::unique_ptr<Value<int>>> ints;
	std::vector<std::unique_ptr<Value<bool>>> bools;
	std::vector<std::unique_ptr<Value<float>>> floats;
	std::vector<std::unique_ptr<Value<double>>> doubles;

	const std::string delimiter = " = ";

	std::vector<std::pair<std::string, std::string>> FileToPairs(std::ifstream& file);
};