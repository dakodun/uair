/* **************************************************************** **
**	
**	Copyright (c) 20XX Iain M. Crawford
**
**	This software is provided 'as-is', without any express or
**	implied warranty. In no event will the authors be held liable
**	for any damages arising from the use of this software.
**
**	Permission is granted to anyone to use this software for any
**	purpose, including commercial applications, and to alter it
**	and redistribute it freely, subject to the following
**	restrictions:
** 
**		1. The origin of this software must not be misrepresented;
**		   you must not claim that you wrote the original
**		   software. If you use this software in a product, an
**		   acknowledgment in the product documentation would be
**		   appreciated but is not required.
**
**		2. Altered source versions must be plainly marked as such,
**		   and must not be misrepresented as being the original
**		   software.
**
**		3. This notice may not be removed or altered from any
**		   source distribution.
** **************************************************************** */

#include "file.hpp"

#include <iostream>
#include <fstream>

#include "exception.hpp"

namespace uair {
File::File(const std::string & filename) {
	LoadFromFile(filename); // load the specified file
}

void File::LoadFromFile(const std::string & filename) {
	// open the file for reading
	std::ifstream file;
	file.open(filename.c_str(), std::ios::in);
	
	try {
		std::string line;
		
		if (file.is_open()) { // if we opened the file successfully
			while (file.good()) { // while the file is open and there is stuff to read
				getline(file, line); // get the next line from the file
				
				mBuffer.push_back(std::move(line)); // move the line into to buffer
			}
			
			file.close(); // clean up after ourselves
		}
		else {
			throw UairException("unable to open the file: " + filename); // throw an error
		}
	} catch (std::exception & e) {
		if (file.is_open()) { // if we've already opened the file
			file.close(); // clean up after ourselves
		}
		
		throw e;
	}
}

void File::SaveToFile(const std::string & filename) {
	// open the file for writing
	std::ofstream file;
	file.open(filename.c_str(), std::ios::out);
	
	try {
		if (file.is_open()) { // if we opened the file successfully
			for (auto iter = mBuffer.begin(); iter < mBuffer.end(); ++iter) { // for all lines in the buffer
				file << *iter << std::endl;; // write the current line to the file
			}
			
			file.close(); // clean up after ourselves
		}
		else {
			throw UairException("unable to open the file: " + filename); // throw an error
		}
	} catch (std::exception& e) {
		if (file.is_open()) { // if we've already opened the file
			file.close(); // clean up after ourselves
		}
		
		throw e;
	}
}
}