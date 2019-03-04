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

#ifndef EXECUTABLESDIGESTS_HPP
#define EXECUTABLESDIGESTS_HPP

#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>

struct ExecutableFileInfo {
	std::uintmax_t Size;
	std::string Digest;
};

class ExecutablesDigests {
public:
	ExecutablesDigests() = default;

	void Insert(const std::string &Path, std::uintmax_t Size);
	bool Contains(const std::string &Path);
	bool Verify(const std::string &Path);

	void Paths(std::list<std::string> &List);

	bool SaveToFile(const std::string &Path);
	bool LoadFromFile(const std::string &Path);

private:
	std::string CalculateSHA256Digest(const std::string &Path);

	std::map<std::string, ExecutableFileInfo> mExecutableFiles;
};

#endif
