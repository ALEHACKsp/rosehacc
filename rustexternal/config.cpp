#include "config.h"
#include <exception>

Config::Config(std::string file_path)
{
	this->file_path = file_path;
}

void Config::SetupValue(std::string name, std::string& value)
{
	strings.emplace_back(std::make_unique<Value<std::string>>(name, &value));
}

void Config::SetupValue(std::string name, int& value)
{
	ints.emplace_back(std::make_unique<Value<int>>(name, &value));
}

void Config::SetupValue(std::string name, bool& value)
{
	bools.emplace_back(std::make_unique<Value<bool>>(name, &value));
}

void Config::SetupValue(std::string name, float& value)
{
	floats.emplace_back(std::make_unique<Value<float>>(name, &value));
}

void Config::SetupValue(std::string name, double& value)
{
	doubles.emplace_back(std::make_unique<Value<double>>(name, &value));
}

void Config::Save()
{
	std::ofstream file(file_path);

	for (const auto& i : strings)
		file << i->name << delimiter << *i->value << std::endl;

	for (const auto& i : ints)
		file << i->name << delimiter << *i->value << std::endl;

	for (const auto& i : bools)
		file << i->name << delimiter << (*i->value ? "true" : "false") << std::endl;

	for (const auto& i : floats)
		file << i->name << delimiter << *i->value << std::endl;

	for (const auto& i : doubles)
		file << i->name << delimiter << *i->value << std::endl;

	file.close();
}

void Config::Load()
{
	std::ifstream file(file_path);
	const auto pairs = FileToPairs(file);

	if (!pairs.empty())
	{
		int lines_parsed = 0;

		for (const auto& i : strings)
		{
			i->name = pairs[lines_parsed].first;
			*i->value = pairs[lines_parsed].second;

			lines_parsed++;
		}

		for (const auto& i : ints)
		{
			i->name = pairs[lines_parsed].first;
			*i->value = std::stoi(pairs[lines_parsed].second);

			lines_parsed++;
		}

		for (const auto& i : bools)
		{
			i->name = pairs[lines_parsed].first;
			const std::string value = pairs[lines_parsed].second;

			if (value == "true")
				*i->value = true;
			else if (value == "false")
				*i->value = false;
			else
				throw std::runtime_error("Config boolean value must be true or false");

			lines_parsed++;
		}

		for (const auto& i : floats)
		{
			i->name = pairs[lines_parsed].first;
			*i->value = std::stof(pairs[lines_parsed].second);

			lines_parsed++;
		}

		for (const auto& i : doubles)
		{
			i->name = pairs[lines_parsed].first;
			*i->value = std::stod(pairs[lines_parsed].second);

			lines_parsed++;
		}
	}

	file.close();
}

std::vector<std::pair<std::string, std::string>> Config::FileToPairs(std::ifstream& file)
{
	std::vector<std::pair<std::string, std::string>> pairs;

	for (std::string i; std::getline(file, i);)
	{
		const size_t delimiter_pos = i.find(delimiter);

		if (delimiter_pos != std::string::npos)
		{
			const std::string name = i.substr(0, delimiter_pos);
			const std::string value = i.substr(delimiter_pos + delimiter.length());

			pairs.emplace_back(name, value);
		}
	}

	return pairs;
}