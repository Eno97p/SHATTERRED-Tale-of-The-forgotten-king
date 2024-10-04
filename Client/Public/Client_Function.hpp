#pragma once
#include "Client_Defines.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <tchar.h>
#include <filesystem>


//using namespace std;
namespace Client
{

	inline std::wstring string_to_wstring(const std::string& str)
	{
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}
	
	inline std::wstring const_char_to_wstring(const char* str)
	{
		std::string utf8_string = str;

		return string_to_wstring(utf8_string);
		//return std::wstring();
	
		
	}

	inline std::wstring Get_CurLevelName(unsigned int iLevelIndex)
	{
		const char* Name = Client::LevelNames[iLevelIndex];
		std::wstring wstrname(Name, Name + strlen(Name));

		return wstrname;
		

	}


	inline void Delete_File()
	{
		for(auto& p : g__Exit_Delete_FileList)
		{
			std::filesystem::remove(p);
			
		}
		
		g__Exit_Delete_FileList.clear();
	}

}