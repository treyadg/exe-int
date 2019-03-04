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

#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <iostream>

#include <ExecutablesDigests.hpp>

void ExecutablesDigests::Insert(const std::string &Path, std::uintmax_t Size)
{
	ExecutableFileInfo fileInfo;
	fileInfo.Size = Size;
	fileInfo.Digest = CalculateSHA256Digest(Path);

	mExecutableFiles.insert(std::make_pair(Path, fileInfo));
}

std::string ExecutablesDigests::CalculateSHA256Digest(const std::string &Path)
{
	std::string sha256Digest;

	CryptoPP::SHA256 hashFunction;
	CryptoPP::FileSource(Path.c_str(), true,
						 new CryptoPP::HashFilter(hashFunction, new CryptoPP::HexEncoder(new CryptoPP::StringSink(sha256Digest), false)));

	return sha256Digest;
}

bool ExecutablesDigests::SaveToFile(const std::string &Path)
{
	std::ofstream outputFile(Path, std::ofstream::out);
	if (!outputFile.is_open()) return false;

	for (auto const & [path, executableFileInfo] : mExecutableFiles)
	{
		outputFile << executableFileInfo.Digest << '\t' << path << std::endl;
	}

	outputFile.close();
	return true;
}

bool ExecutablesDigests::LoadFromFile(const std::string &Path)
{
	std::string line;
	std::ifstream inputFile(Path);
	if (!inputFile.is_open()) return false;
	while (getline(inputFile, line))
	{
		auto pos = line.find('\t');
		auto digest = line.substr(0, pos);
		auto path = line.substr(pos + 1);

		ExecutableFileInfo fileInfo;
		fileInfo.Size = 0;
		fileInfo.Digest = digest;

		mExecutableFiles.insert(std::make_pair(path, fileInfo));
	}
	inputFile.close();
	return true;
}

bool ExecutablesDigests::Contains(const std::string &Path)
{
	return (mExecutableFiles.find(Path) != mExecutableFiles.end());
}

bool ExecutablesDigests::Verify(const std::string &Path)
{
	if (!Contains(Path)) return false;
	std::string originalDigest = mExecutableFiles.at(Path).Digest;
	std::string currentDigest = CalculateSHA256Digest(Path);
	return (currentDigest == originalDigest);
}

void ExecutablesDigests::Paths(std::list<std::string> &List)
{
	List.clear();
	for (const auto &entry: mExecutableFiles)
		List.push_back(entry.first);
}
