/*
Copyright 2019, Tarek Reyad
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <iostream>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <cstring>

#include <ExecutablesFinder.hpp>

const std::uintmax_t MINIMUM_EXE_SIZE = 255;

const char MZ_SIGNATURE[] { 0x5A, 0x4D };
const char PE_SIGNATURE[] { 'P', 'E', 0, 0 };
const char ELF_SIGNATURE[] { 0x7F, 'E', 'L', 'F' };

ExecutablesFinder::ExecutablesFinder(std::string Path):
	mPath(std::move(Path))
{

}

bool ExecutablesFinder::isExecutable(const std::string &Path, bool &Error)
{
	Error = false;

	std::ifstream inputFile(Path, std::ios::binary);
	if (inputFile.is_open())
	{
		inputFile.unsetf(std::ios::skipws);

		char buffer[MINIMUM_EXE_SIZE];
		inputFile.read(buffer, sizeof(buffer));

		if (inputFile)
		{
			bool executable = (memcmp(buffer, MZ_SIGNATURE, sizeof(MZ_SIGNATURE)) == 0);
			executable |= (memcmp(buffer, PE_SIGNATURE, sizeof(PE_SIGNATURE)) == 0);
			executable |= (memcmp(buffer, ELF_SIGNATURE, sizeof(ELF_SIGNATURE)) == 0);
			inputFile.close();
			return executable;
		}

		inputFile.close();
		Error = true;
		return false;
	}

	Error = true;
	return false;
}

bool ExecutablesFinder::Run(const std::function<void (std::string Path, std::uintmax_t Size, bool Error)> &Callback)
{
	std::error_code ec;
	bool error, executable;

	for (const auto &entry: std::filesystem::recursive_directory_iterator(mPath))
	{
		if (!entry.is_regular_file()) continue;
		std::uintmax_t size = entry.file_size(ec);
		if (ec)
		{
			Callback(entry.path(), 0, true);
			continue;
		}

		if (size < MINIMUM_EXE_SIZE)
			continue;

		executable = isExecutable(entry.path(), error);
		if (error) Callback(entry.path(), size, true);
		else if (executable) Callback(entry.path(), size, false);
	}

	return true;
}
