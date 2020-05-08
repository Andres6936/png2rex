// Design by Joan Andrés.

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <boost/process.hpp>
#include <boost/process/io.hpp>

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
	using VectorString = std::vector<std::string>;

	fs::path images = L"../Test/Images/";
	fs::directory_entry directory(images);
	std::vector<fs::path> filenames;
	std::vector<fs::path> filenamesPNG;
	std::vector<fs::path> filenamesMD;
	std::vector<std::string> filenameOutput;
	std::pair<VectorString, VectorString> sha256sum;

	if (directory.exists() and directory.is_directory())
	{
		for (const fs::directory_entry& entry : fs::directory_iterator(directory))
		{
			filenames.push_back(entry.path());
		}

		for (const fs::path& file: filenames)
		{
			if (file.extension() == ".png")
			{
				filenamesPNG.push_back(file);
				filenamesMD.push_back(fs::path(file).replace_extension(".md"));
				filenameOutput.push_back(file.filename().string() + ".xp");
			}
		}

		const std::string executable = "Gall";

		for (int i = 0; i < filenamesPNG.size(); ++i)
		{
			boost::process::system(executable, filenamesPNG[i].string(), filenameOutput[i]);
		}

		for (const fs::path& file: filenamesMD)
		{
			std::ifstream reader(file);

			if (reader.good())
			{
				std::string fileSha256sum;
				reader >> fileSha256sum;
				sha256sum.first.push_back(file.string());
				sha256sum.second.push_back(fileSha256sum);
			}
			else
			{
				std::cerr << "Can not open the file " << file.string() << "\n";
			}
		}

		const std::string commandSha256sum = "/usr/bin/sha256sum";

		for (const std::string& filename : filenameOutput)
		{
			fs::directory_entry file(filename);

			if(file.exists() and file.is_regular_file())
			{
				boost::process::ipstream readerPipe;
				boost::process::child child(commandSha256sum, file.path().string(), boost::process::std_out > readerPipe);
				child.wait();
				std::string result;
				readerPipe >> result;
				std::cout << result << "\n";
				result.clear();
				child.terminate();
			}
			else
			{
				std::cerr << "The file <" << file.path().string() << "> no exist\n";
			}
		}
	}
	else
	{
		std::cerr << "Error, the directory " << directory.path().string() << "not exist";
	}
}