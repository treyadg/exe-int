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
#include <unistd.h>

#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>

#include <ExecutablesFinder.hpp>
#include <ExecutablesDigests.hpp>

static const int MINIMUM_ARGS = 6;

void PrintHelp()
{
	std::cout << "ExeInt v1.0" << std::endl
			  << std::endl
			  << "Usage:" << std::endl
			  << " ExeInt [OPERATION] -p [PATH] -i [Input/Output .int file]" << std::endl
			  << std::endl;

	std::cout << "Available Operations:" << std::endl
			  << " create\tCreate .int file from the provided path" << std::endl
			  << " verify\tVerify .int file for the provided path" << std::endl
			  << " reset\tReset the directory from the provided .int file" << std::endl
			  << std::endl;

	std::cout << "Options:" << std::endl
			  << " -p\t" << "Directory path" << std::endl
			  << " -i\t" << "Input/Output .int file path" << std::endl
			  << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc < MINIMUM_ARGS)
	{
		PrintHelp();
		return -1;
	}

	const char *operation = argv[1];
	char *directoryPath = nullptr;
	char *intFilePath = nullptr;
	bool reset = false;

	int opt;
	while ((opt = getopt(argc, argv, ":p:i:r")) != -1)
	{
		switch (opt)
		{
			case 'p':
				directoryPath = optarg;
				break;

			case 'i':
				intFilePath = optarg;
				break;

			case 'r':
				reset = true;
				break;

			case ':':
				std::cerr << opt << " option needs a value" << std::endl;
				break;

			case '?':
				std::cerr << "unknown option: " << optopt << std::endl;
				break;
		}
	}

	if (intFilePath == nullptr || directoryPath == nullptr)
	{
		PrintHelp();
		return -1;
	}

	if (strcasecmp(operation, "create") == 0)
	{
		ExecutablesDigests executablesDigests;
		ExecutablesFinder executablesFinder(directoryPath);
		executablesFinder.Run([&](const std::string &Path, uintmax_t Size, bool Error) {
			if (!Error)
				executablesDigests.Insert(Path, Size);
			else
				std::cerr << "Error: Unable to access '" << Path << '\'' << std::endl;
		});
		executablesDigests.SaveToFile(std::string(intFilePath));
	}
	else if (strcasecmp(operation, "verify") == 0)
	{
		ExecutablesDigests executablesDigests;
		executablesDigests.LoadFromFile(std::string(intFilePath));

		std::list<std::string> newExecutableFiles;
		std::list<std::string> modifiedExecutableFiles;
		std::list<std::string> missingExecutableFiles;

		executablesDigests.Paths(missingExecutableFiles);

		ExecutablesFinder executablesFinder(directoryPath);
		executablesFinder.Run([&](const std::string &Path, uintmax_t Size, bool Error) {
			(void) Size;

			if (!Error)
			{
				if (!executablesDigests.Contains(Path))
					newExecutableFiles.push_back(Path);
				else
				{
					missingExecutableFiles.remove(Path);
					if (!executablesDigests.Verify(Path))
						modifiedExecutableFiles.push_back(Path);
				}
			}
			else
				std::cerr << "Error: Unable to access '" << Path << '\'' << std::endl;
		});

		if (!newExecutableFiles.empty())
		{
			std::cout << "New Executable files:" << std::endl;
			for (const auto &path: newExecutableFiles)
			{
				std::cout << " " << path << std::endl;
				if (reset)
				{
					// Terminate process
					// Delete file
				}
			}
			std::cout << std::endl;
		}

		if (!modifiedExecutableFiles.empty())
		{
			std::cout << "Modified Executable files:" << std::endl;
			for (const auto &path: modifiedExecutableFiles)
			{
				std::cout << " " << path << std::endl;
				if (reset)
				{
					// Terminate process
					// Delete file
					// Download New
				}
			}
			std::cout << std::endl;
		}

		if (!missingExecutableFiles.empty())
		{
			std::cout << "Missing Executable files:" << std::endl;
			for (const auto &path: missingExecutableFiles)
			{
				std::cout << " " << path << std::endl;
				if (reset)
				{
					// Download new
				}
			}
			std::cout << std::endl;
		}
	}
	else
	{
		std::cerr << "Error, invalid operation parameter" << std::endl;
		return -1;
	}

	return 0;
}
