// ModelFileParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <Assertion.h>
#include <vector>

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
	std::vector<Float3> vertice, normals;
	std::vector<Float2> textureCoordinates;

	char input;
	unsigned int vertexIndex = 0, tectureCoordIndex = 0, normalIndex = 0, faceIndex = 0;
	while (!sourceFile.eof())
	{
		sourceFile.get(input);

		//The first input for each line must be the line type specifier
		switch (input)
		{
			case 'v':
			{
				sourceFile.get(input);

				switch (input)
				{
					case ' ': //Vertex
					{
						Float3 vertex;
						sourceFile >> vertex.x >> vertex.y >> vertex.z;
						vertex.z *= -1; //Invert the sign of the z vertex to turn this into a left handed coord system
						vertice.push_back(vertex);
					}
					break;

					case 'n': //Normals
					{
						Float3 normal;
						sourceFile >> normal.x >> normal.y >> normal.z;
						normal.z *= -1; //Invert the sign of the z vertex to turn this into a left handed coord system
						normals.push_back(normal);
					}
					break;

					case 't': //Texture
					{
						Float2 textureCoord;
						sourceFile >> textureCoord.x >> textureCoord.y;
						textureCoord.y = 1.0f - textureCoord.y; //Invert the v coord to left handed system
						textureCoordinates.push_back(textureCoord);
					}
					break;

					default:
					SOFTASSERT(false, "Should not have an unhandled char here");
					break;
				}

				
			}
			break;

			case 'f': //Faces
			{
				sourceFile.get(input);
				if (input == ' ')
				{
					//Read in back to front to ensure that coordinate system is converted
					FaceType face;
					sourceFile >> face.vIndex3 >> input >> face.tIndex3 >> input >> face.nIndex3
								>> face.vIndex2 >> input >> face.tIndex2 >> input >> face.nIndex2
								 >> face.vIndex1 >> input >> face.tIndex1 >> input >> face.nIndex1;
					faces.push_back(face);
				}
			}
			break;

			default:
			{
				do
				{ 
					sourceFile.get(input);
				} while (input != '\n'); //Read till the next line
			}
			break;
		}

	}

	//Display diagnostics
	std::cout << "Vertices added : " << vertice.size() << std::endl;
	std::cout << "Texture Coords added : " << textureCoordinates.size() << std::endl;
	std::cout << "Normals added : " << normals.size() << std::endl;
	std::cout << "Faces added : " << faces.size() << std::endl;
}

int main()
{
    std::ifstream sourceModelFile;
	std::ofstream destinationModelFile;

	OpenSourceModelFile(sourceModelFile);

	OpenDestinationModelFile(destinationModelFile);

	ConvertSourceModelFileToSimpleFileFormat(sourceModelFile, destinationModelFile);

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
