////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////


//////////////
// INCLUDES //
//////////////
#include <iostream>
#include <fstream>
using namespace std;


//////////////
// TYPEDEFS //
//////////////
typedef struct
{
    float x, y, z;
}VertexType;

typedef struct
{
    int vIndex1, vIndex2, vIndex3;
    int tIndex1, tIndex2, tIndex3;
    int nIndex1, nIndex2, nIndex3;
}FaceType;


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
void GetModelFilename(char*);
bool ReadFileCounts(char*, int&, int&, int&, int&);
bool LoadDataStructures(char*, int, int, int, int);


//////////////////
// MAIN PROGRAM //
//////////////////
int main()
{
    bool result;
    char filename[256];
    int vertexCount, textureCount, normalCount, faceCount;
    char garbage;


    // Read in the name of the model file.
    GetModelFilename(filename);

    // Read in the number of vertices, tex coords, normals, and faces so that the data structures can be initialized with the exact sizes needed.
    result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
    if (!result)
    {
        return -1;
    }

    // Display the counts to the screen for information purposes.
    cout << endl;
    cout << "Vertices: " << vertexCount << endl;
    cout << "UVs:      " << textureCount << endl;
    cout << "Normals:  " << normalCount << endl;
    cout << "Faces:    " << faceCount << endl;

    // Now read the data from the file into the data structures and then output it in our model format.
    result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
    if (!result)
    {
        return -1;
    }

    // Notify the user the model has been converted.
    cout << "\nFile has been converted." << endl;
    cout << "\nDo you wish to exit (y/n)? ";
    cin >> garbage;

    return 0;
}


void GetModelFilename(char* filename)
{
    bool done;
    ifstream fin;


    // Loop until we have a file name.
    done = false;
    while (!done)
    {
        // Ask the user for the filename.
        cout << "Enter model filename: ";

        // Read in the filename.
        cin >> filename;

        // Attempt to open the file.
        fin.open(filename);

        if (fin.good())
        {
            // If the file exists and there are no problems then exit since we have the file name.
            done = true;
        }
        else
        {
            // If the file does not exist or there was an issue opening it then notify the user and repeat the process.
            fin.clear();
            cout << endl;
            cout << "File " << filename << " could not be opened." << endl << endl;
        }
    }

    return;
}


bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
    ifstream fin;
    char input;


    // Initialize the counts.
    vertexCount = 0;
    textureCount = 0;
    normalCount = 0;
    faceCount = 0;

    // Open the file.
    fin.open(filename);

    // Check if it was successful in opening the file.
    if (fin.fail() == true)
    {
        return false;
    }

    // Read from the file and continue to read until the end of the file is reached.
    fin.get(input);
    while (!fin.eof())
    {
        // If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
        if (input == 'v')
        {
            fin.get(input);
            if (input == ' ') { vertexCount++; }
            if (input == 't') { textureCount++; }
            if (input == 'n') { normalCount++; }
        }

        // If the line starts with 'f' then increment the face count.
        if (input == 'f')
        {
            fin.get(input);
            if (input == ' ') { faceCount++; }
        }

        // Otherwise read in the remainder of the line.
        while (input != '\n')
        {
            fin.get(input);
        }

        // Start reading the beginning of the next line.
        fin.get(input);
    }

    // Close the file.
    fin.close();

    return true;
}


bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount)
{
    VertexType *vertices, *texcoords, *normals;
    FaceType *faces;
    ifstream fin;
    int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
    char input, input2;
    ofstream fout;


    // Initialize the four data structures.
    vertices = new VertexType[vertexCount];
    if (!vertices)
    {
        return false;
    }

    texcoords = new VertexType[textureCount];
    if (!texcoords)
    {
        return false;
    }

    normals = new VertexType[normalCount];
    if (!normals)
    {
        return false;
    }

    faces = new FaceType[faceCount];
    if (!faces)
    {
        return false;
    }

    // Initialize the indexes.
    vertexIndex = 0;
    texcoordIndex = 0;
    normalIndex = 0;
    faceIndex = 0;

    // Open the file.
    fin.open(filename);

    // Check if it was successful in opening the file.
    if (fin.fail() == true)
    {
        return false;
    }

    // Read in the vertices, texture coordinates, and normals into the data structures.
    // Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
    fin.get(input);
    while (!fin.eof())
    {
        if (input == 'v')
        {
            fin.get(input);

            // Read in the vertices.
            if (input == ' ')
            {
                fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

                // Invert the Z vertex to change to left hand system.
                vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
                vertexIndex++;
            }

            // Read in the texture uv coordinates.
            if (input == 't')
            {
                fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

                // Invert the V texture coordinates to left hand system.
                texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
                texcoordIndex++;
            }

            // Read in the normals.
            if (input == 'n')
            {
                fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

                // Invert the Z normal to change to left hand system.
                normals[normalIndex].z = normals[normalIndex].z * -1.0f;
                normalIndex++;
            }
        }

        // Read in the faces.
        if (input == 'f')
        {
            fin.get(input);
            if (input == ' ')
            {
                // Read the face data in backwards to convert it to a left hand system from right hand system.
                fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
                    >> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
                    >> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
                faceIndex++;
            }
        }

        // Read in the remainder of the line.
        while (input != '\n')
        {
            fin.get(input);
        }

        // Start reading the beginning of the next line.
        fin.get(input);
    }

    // Close the file.
    fin.close();

    // Open the output file.
    fout.open("model.txt");

    // Write out the file header that our model format uses.
    fout << "Vertex Count: " << (faceCount * 3) << endl;
    fout << endl;
    fout << "Data:" << endl;
    fout << endl;

    // Now loop through all the faces and output the three vertices for each face.
    for (int i = 0; i < faceIndex; i++)
    {
        vIndex = faces[i].vIndex1 - 1;
        tIndex = faces[i].tIndex1 - 1;
        nIndex = faces[i].nIndex1 - 1;

        fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
            << texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
            << normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

        vIndex = faces[i].vIndex2 - 1;
        tIndex = faces[i].tIndex2 - 1;
        nIndex = faces[i].nIndex2 - 1;

        fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
            << texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
            << normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

        vIndex = faces[i].vIndex3 - 1;
        tIndex = faces[i].tIndex3 - 1;
        nIndex = faces[i].nIndex3 - 1;

        fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
            << texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
            << normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;
    }

    // Close the output file.
    fout.close();

    // Release the four data structures.
    if (vertices)
    {
        delete[] vertices;
        vertices = 0;
    }
    if (texcoords)
    {
        delete[] texcoords;
        texcoords = 0;
    }
    if (normals)
    {
        delete[] normals;
        normals = 0;
    }
    if (faces)
    {
        delete[] faces;
        faces = 0;
    }

    return true;
}

//// ModelFileParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
//
//#include <iostream>
//#include <fstream>
//#include <Assertion.h>
//#include <vector>
//
//struct Float3
//{
//	float x, y, z;
//};
//
//struct Float2
//{
//	float x, y;
//};
//
//struct FaceType
//{
//	unsigned int vIndex1, vIndex2, vIndex3;
//	unsigned int tIndex1, tIndex2, tIndex3;
//	unsigned int nIndex1, nIndex2, nIndex3;
//};
//
//void OpenSourceModelFile(std::ifstream& file)
//{
//	HARDASSERT(!file.is_open(), "Should only have unopened file at this point.");
//	bool fileOpened = false;
//
//	std::string filename;
//
//	while (!fileOpened)
//	{
//		std::cout << "Enter the source model filename" << std::endl;
//		std::cin >> filename;
//
//		file.open(filename);
//
//		fileOpened = !file.fail();
//
//		if (!fileOpened)
//		{
//			std::cout << "Could not open file. Check the location or name" << std::endl;
//		}
//	}
//}
//
//void OpenDestinationModelFile(std::ofstream& file)
//{
//	HARDASSERT(!file.is_open(), "Should only have unopened file at this point.");
//	bool fileOpened = false;
//
//	std::string filename;
//
//	while (!fileOpened)
//	{
//		std::cout << "Enter the destination model filename [Format : <name>.txt]" << std::endl;
//		std::cin >> filename;
//
//		file.open(filename, std::ios_base::out);
//
//		fileOpened = !file.fail();
//
//		if (!fileOpened)
//		{
//			std::cout << "Could not open file. Check the location or name" << std::endl;
//		}
//	}
//}
//
//void ConvertSourceModelFileToSimpleFileFormat(std::ifstream& sourceFile, std::ofstream& destinationSimpleFile)
//{
//	HARDASSERT(sourceFile.is_open() && destinationSimpleFile.is_open(), "Source and destination files should be open at this moment");
//
//	std::vector<FaceType> faces;
//	std::vector<Float3> vertice, normals;
//	std::vector<Float2> textureCoordinates;
//
//	char input;
//	unsigned int vertexIndex = 0, tectureCoordIndex = 0, normalIndex = 0, faceIndex = 0;
//	while (!sourceFile.eof())
//	{
//		sourceFile.get(input);
//
//		//The first input for each line must be the line type specifier
//		switch (input)
//		{
//			case 'v':
//			{
//				sourceFile.get(input);
//
//				switch (input)
//				{
//					case ' ': //Vertex
//					{
//						Float3 vertex;
//						sourceFile >> vertex.x >> vertex.y >> vertex.z;
//						vertex.z *= -1; //Invert the sign of the z vertex to turn this into a left handed coord system
//						vertice.push_back(vertex);
//					}
//					break;
//
//					case 'n': //Normals
//					{
//						Float3 normal;
//						sourceFile >> normal.x >> normal.y >> normal.z;
//						normal.z *= -1; //Invert the sign of the z vertex to turn this into a left handed coord system
//						normals.push_back(normal);
//					}
//					break;
//
//					case 't': //Texture
//					{
//						Float2 textureCoord;
//						sourceFile >> textureCoord.x >> textureCoord.y;
//						textureCoord.y = 1.0f - textureCoord.y; //Invert the v coord to left handed system
//						textureCoordinates.push_back(textureCoord);
//					}
//					break;
//
//					default:
//					SOFTASSERT(false, "Should not have an unhandled char here");
//					break;
//				}
//
//				
//			}
//			break;
//
//			case 'f': //Faces
//			{
//				sourceFile.get(input);
//				if (input == ' ')
//				{
//					//Read in back to front to ensure that coordinate system is converted
//					FaceType face;
//					sourceFile >> face.vIndex3 >> input >> face.tIndex3 >> input >> face.nIndex3
//								>> face.vIndex2 >> input >> face.tIndex2 >> input >> face.nIndex2
//								 >> face.vIndex1 >> input >> face.tIndex1 >> input >> face.nIndex1;
//					faces.push_back(face);
//				}
//			}
//			break;
//
//			default:
//			break;
//		}
//
//		do
//		{
//			sourceFile.get(input);
//		} while (input != '\n'); //Read till the next line
//	}
//
//	//Write to dest file
//	destinationSimpleFile << "Vertex Count: " << vertice.size() << std::endl << std::endl;
//	destinationSimpleFile << "Data:" << std::endl << std::endl;
//
//	for (const FaceType& face : faces)
//	{
//		unsigned int vIndex = face.vIndex1 - 1;
//		unsigned int tIndex = face.tIndex1 - 1;
//		unsigned int nIndex = face.nIndex1 - 1;
//
//		destinationSimpleFile << vertice[vIndex].x << ' ' << vertice[vIndex].y << ' ' << vertice[nIndex].z << ' '
//								<< textureCoordinates[tIndex].x << ' ' << textureCoordinates[tIndex].y << ' '
//								  << normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;
//
//		vIndex = face.vIndex2 - 1;
//		tIndex = face.tIndex2 - 1;
//		nIndex = face.nIndex2 - 1;
//
//		destinationSimpleFile << vertice[vIndex].x << ' ' << vertice[vIndex].y << ' ' << vertice[nIndex].z << ' '
//			<< textureCoordinates[tIndex].x << ' ' << textureCoordinates[tIndex].y << ' '
//			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;
//
//		vIndex = face.vIndex3 - 1;
//		tIndex = face.tIndex3 - 1;
//		nIndex = face.nIndex3 - 1;
//
//		destinationSimpleFile << vertice[vIndex].x << ' ' << vertice[vIndex].y << ' ' << vertice[nIndex].z << ' '
//			<< textureCoordinates[tIndex].x << ' ' << textureCoordinates[tIndex].y << ' '
//			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;
//	}
//
//	//Display stats
//	std::cout << "Vertices added : " << vertice.size() << std::endl;
//	std::cout << "Texture Coords added : " << textureCoordinates.size() << std::endl;
//	std::cout << "Normals added : " << normals.size() << std::endl;
//	std::cout << "Faces added : " << faces.size() << std::endl;
//}
//
//int main()
//{
//    std::ifstream sourceModelFile;
//	std::ofstream destinationModelFile;
//
//	OpenSourceModelFile(sourceModelFile);
//
//	OpenDestinationModelFile(destinationModelFile);
//
//	ConvertSourceModelFileToSimpleFileFormat(sourceModelFile, destinationModelFile);
//
//	sourceModelFile.close();
//	destinationModelFile.close();
//}
//
//// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
//// Debug program: F5 or Debug > Start Debugging menu
//
//// Tips for Getting Started: 
////   1. Use the Solution Explorer window to add/manage files
////   2. Use the Team Explorer window to connect to source control
////   3. Use the Output window to see build output and other messages
////   4. Use the Error List window to view errors
////   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
////   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
