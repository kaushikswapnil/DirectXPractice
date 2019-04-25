// ModelFileParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <Assertion.h>

struct Float3
{
	float x, y, z;
};

struct Float2
{
	float x, y;
};

struct FaceType
{
	unsigned int vIndex1, vIndex2, vIndex3;
	unsigned int tIndex1, tIndex2, tIndex3;
	unsigned int nIndex1, nIndex2, nIndex3;
};

void OpenSourceModelFile(std::ifstream& file)
{
	HARDASSERT(!file.is_open(), "Should only have unopened file at this point.");
	bool fileOpened = false;

	std::string filename;

	while (!fileOpened)
	{
		std::cout << "Enter the source model filename" << std::endl;
		std::cin >> filename;

		file.open(filename);

		fileOpened = !file.fail();

		if (!fileOpened)
		{
			std::cout << "Could not open file. Check the location or name" << std::endl;
		}
	}
}

void OpenDestinationModelFile(std::ofstream& file)
{
	HARDASSERT(!file.is_open(), "Should only have unopened file at this point.");
	bool fileOpened = false;

	std::string filename;

	while (!fileOpened)
	{
		std::cout << "Enter the destination model filename [Format : <name>.txt]" << std::endl;
		std::cin >> filename;

		file.open(filename, std::ios_base::out);

		fileOpened = !file.fail();

		if (!fileOpened)
		{
			std::cout << "Could not open file. Check the location or name" << std::endl;
		}
	}
}

void ConvertSourceModelFileToSimpleFileFormat(std::ifstream& sourceFile, std::ofstream& destinationSimpleFile)
{
	std::vector<FaceType> faces;
	std::vector<Float3> vertices, normals;
	std::vector<Float2> textureCoordinates;

	std::string input;
	while (!sourceFile.eof())
	{
		sourceFile >> input;

		//The first input for each line must be the line type specifier
		switch (input)
		{
			case 'v':
			{
				Float3 vertice;
				sourceFile >> vertice.x >> vertice.y >> vertice.z;
			}
			break;
			default:
			{
				char c;
				do
				{ 
					sourceFile.get(c);
				} while (c != '\n'); //Read till the next line
			}
			break;
		}

	}
}

int main()
{
    std::ifstream sourceModelFile;
	std::ofstream destinationModelFile;

	OpenSourceModelFile(sourceModelFile);

	OpenDestinationModelFile(destinationModelFile);

	ConvertSourceModelFileToSimpleFileFormat(sourceModelFile, destinationModelFile)

	sourceModelFile.close();
	destinationModelFile.close();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
