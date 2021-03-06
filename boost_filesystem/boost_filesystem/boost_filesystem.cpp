// boost_filesystem.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <string>
#include <boost/regex.hpp>
#include <map>
#include <vector>
using namespace boost::filesystem;


// Функция разбивает строку на вектор строк по разделителю
std::vector<std::string> explode(const std::string& str, const char& ch)
{
	std::string next;
	std::vector<std::string> result;

	for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
	{
		if (*it == ch) 
		{
			if (!next.empty()) 
			{
				result.push_back(next);
				next.clear();
			}
		}
		else 
		{
			next += *it;
		}
	}
	if (!next.empty())
		result.push_back(next);
	return result;
}

// Функция проверяет, что название файла соответствует заданному
bool check_entry(const directory_entry& x)
{
	const boost::regex my_filter("balance_[0-9]{8}_[0-9]{8}.txt");
	
	if (!boost::filesystem::is_regular_file(x.status()))
		return false;

	path pth = x.path();
	std::string  s = pth.filename().string();

	if (!boost::regex_match(s, my_filter, boost::regex_constants::match_default))
		return false;
	return true;
}

// Функция находит все файлы и выбираетв вектор нужные
std::vector<directory_entry> find_entries(path pth)
{
	std::vector<directory_entry> out = std::vector<directory_entry>();
	try
	{
		for (const directory_entry& x : recursive_directory_iterator(pth))
		{
			if (is_symlink(x.path()))
			{
				if (exists(x.path()))
				{
					for (const directory_entry& x : recursive_directory_iterator(x.path()))
					{
						if(check_entry(x))
							out.push_back(x);
					}
				}
			}
			else
			{
				if (check_entry(x))
					out.push_back(x);
			}
		}
	}
	catch (const filesystem_error&) {}

	return out;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	
	std::map <std::string, std::vector<std::string>> dict;
	path p;

	// если не задана папака, то ищемв текущей
	if (argc < 2)
	{
		p = ".";
	}
	else
	{
		p = argv[1];
	}

	std::vector<directory_entry> entries = find_entries(p);
	// если файлы найдены, то обрабатыем их
	if (entries.size() > 0)
	{
		for (auto &x : entries)
		{
			std::string p = x.path().parent_path().string();
			std::string new_mod_s;

			std::string::size_type pos = p.rfind("\\");

			if (pos != std::string::npos)
			{
				new_mod_s = p.substr(pos + 1);
			}

			dict[new_mod_s].push_back(x.path().filename().string());
		}
	}
	else
	{
		std::cout << "Файлы не найдены" << std::endl;
		return 0;
	}
	// выводим список файлов с указанием брокера
	for (std::map<std::string, std::vector<std::string>>::iterator it = dict.begin();
		it != dict.end(); ++it)
	{
		std::string p = it->first;
		std::vector<std::string> f = it->second;
		for(auto &n : f)
			std::cout << p << " " << n << std::endl;
	}

	std::cout << std::endl;

	// выводим последний фал для данного брокера и счета
	for (std::map<std::string, std::vector<std::string>>::iterator it = dict.begin();
		it != dict.end(); ++it)
	{
		std::map <std::string, std::vector<std::string>> dct;
		std::string p = it->first;
		std::vector<std::string> f = it->second;

		for (auto& i : f)
		{
			std::vector<std::string> ch = explode(i, '_');
			dct[ch[1]].push_back(ch[2]);
		}

		for (std::map<std::string, std::vector<std::string>>::iterator it = dct.begin();
			it != dct.end(); ++it)
		{
			std::string date = it->second[it->second.size() - 1].substr(0, it->second[it->second.size() - 1].size() - 4);
			
			std::cout << "broker:" << p << " " 
				<< "account:" <<it->first << " " 
				<< "files:" << it->second.size() << " "
				<< "lastdate:" << date << std::endl;
		}
	}
	
	return 0;
}
