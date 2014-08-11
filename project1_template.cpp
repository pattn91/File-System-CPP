//============================================================================
// Name        : FileSystem53.cpp
// Author      : Patrick Nguyen - 28586045
// Author      : Bing Hui Feng - 78912993
// Author      : Kevin Pham - 51044146
// Version     : 
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <vector>

using namespace std;

class FileSystem53 {

	int B;  //Block length
	int K;  //Number of blocks for descriptor table
	char** desc_table;  // Descriptor Table (in memory).
	// This is aka cache. It's contents should be
	// maintained to be same as first K blocks in disk.
	// Descriptor table format:
	// +-------------------------------------+
	// | bitmap | dsc_0 | dsc_1 | .. | dsc_N |
	// +-------------------------------------+
	//   bitmap: Each bit represent a block in a disk. MAX_BLOCK_NO/8 bytes
	//   dsc_0 : Root directory descriptor
	//   dsc_i : i'th descriptor. Each descriptor is FILE_SIZE_FIELD+ARRAY_SIZE bytes long.

	// Filesystem format parameters:
	static const int FILE_SIZE_FIELD = 1;     // Size of file size field in bytes. Maximum file size allowed in this file system is 192.
	static const int ARRAY_SIZE = 3;          // The length of array of disk block numbers that hold the file contents.
	static const int DESCR_SIZE = FILE_SIZE_FIELD + ARRAY_SIZE;
	static const int MAX_FILE_NO = 14;        // Maximum number of files which can be stored by this file system.
	static const int MAX_BLOCK_NO = 64;       // Maximum number of blocks which can be supported by this file system.
	static const int MAX_BLOCK_NO_DIV8 = MAX_BLOCK_NO / 8;
	static const int MAX_FILE_NAME_LEN = 32;  // Maximum size of file name in byte.
	static const int MAX_OPEN_FILE = 3;       // Maximum number of files to open at the same time.
	static const int FILEIO_BUFFER_SIZE = 64; // Size of file io bufer
	static const int _EOF = -1;       // End-of-File

	char** ldisk;
	static const int l = 64;
	int* oftAllocation;
	 char OFTable[3][l + 2];


public:

	/* Constructor of this File system.
	*   1. Initialize IO system.
	*   2. Format it if not done.
	*/
	FileSystem53();

	// Open File Table(OFT).
	void OpenFileTable();

	// Allocate open file table
	int find_oft();

	//Deallocate
	void deallocate_oft(int index);

	/* Format file system.
	*   1. Initialize the first K blocks with zeros.
	*   2. Create root directory descriptor for directory file.
	* Parameter(s):
	*   none
	* Return:
	*   none
	*/
	void format();


	/* Read descriptor
	* Parameter(s):
	*    no: Descriptor number to read
	* Return:
	*    Return char[4] of descriptor
	*/
	char* read_descriptor(int no);


	/* Clear descriptor
	*   1. Clear descriptor entry
	*   2. Clear bitmap
	*   3. Write back to disk
	* Parameter(s):
	*    no: Descriptor number to clear
	* Return:
	*    none
	*/
	void clear_descriptor(int no);


	/* Write descriptor
	*   1. Update descriptor entry
	*   2. Mark bitmap
	*   3. Write back to disk
	* Parameter(s):
	*    no: Descriptor number to write
	*    desc: descriptor to write
	* Return:
	*    none
	*/
	void write_descriptor(int no, char* desc);


	/* Search for an unoccupied descriptor.
	* If ARRAY[0] is zero, this descriptor is not occupied.
	* Then it returns descriptor number.
	*/
	int find_empty_descriptor();


	/* Search for an unoccupied block.
	*   This returns the first unoccupied block in bitmap field.
	*   Return value -1 means all blocks are occupied.
	* Parameter(s):
	*    none
	* Return:
	*    Returns the block number
	*    -1 if not found
	*/
	int find_empty_block();


	/* Get one character.
	*    Returns the character currently pointed by the internal file position
	*    indicator of the specified stream. The internal file position indicator
	*    is then advanced to the next character.
	* Parameter(s):
	*    index (descriptor number of the file to be added.)
	* Return:
	*    On success, the character is returned.
	*    If a writing error occurs, EOF is returned.
	*/
	int fgetc(int index);


	/* Put one character.
	*    Writes a character to the stream and advances the position indicator.
	*    The character is written at the position indicated by the internal position
	*    indicator of the file, which is then automatically advanced by one.
	* Parameter(s):
	*    c: character to write
	*    index (descriptor number of the file to be added.)
	* Return:
	*    On success, the character written is returned.
	*    If a writing error occurs, -2 is returned.
	*/
	int fputc(int c, int index);


	/* Check for the end of file.
	* Parameter(s):
	*    index (descriptor number of the file to be added.)
	* Return:
	*    Return true if end-of-file reached.
	*/
	bool feof(int index);


	/* Search for a file
	* Parameter(s):
	*    index: index of open file table
	*    st: The name of file to search.
	* Return:
	*    index: An integer number position of found file entry.
	*    Return -1 if not found.
	*/
	int search_dir(int index, string symbolic_file_name);


	/* Clear a file entry from directory file
	*
	* Parameter(s):
	*    index: open file table index
	*    start_pos:
	*    length:
	* Return:
	*    none
	*/
	void delete_dir(int index, int start_pos, int len);


	/* File creation function:
	*    1. creates empty file with file size zero.
	*    2. makes/allocates descriptor.
	*    3. updates directory file.
	* Parameter(s):
	*    symbolic_file_name: The name of file to create.
	* Return:
	*    Return 0 for successful creation.
	*    Return -1 for error (no space in disk)
	*    Return -2 for error (for duplication)
	*/
	int create(string symbolic_file_name);


	/* Open file with descriptor number function:
	* Parameter(s):
	*    desc_no: descriptor number
	* Return:
	*    index: index if open file table if successfully allocated.
	*    Return -1 for error.
	*/
	int open_desc(int desc_no);


	/* Open file with file name function:
	* Parameter(s):
	*    symbolic_file_name: The name of file to open.
	* Return:
	*    index: An integer number, which is a index number of open file table.
	*    Return -1 or -2 if it cannot be open.
	*/
	// TODOs:
	// 1. Open directory file
	// 2. Search for a file with given name
	//    Return -1 if not found.
	// 3. Get descriptor number of the found file
	// 4. Looking for unoccupied entry in open file table.
	//    Return -2 if all entry are occupied.
	// 5. Initialize the entry (descriptor number, current position, etc.)
	// 6. Return entry number
	int open(string symbolic_file_name);


	/* File Read function:
	*    This reads a number of bytes from the the file indicated by index.
	*    Reading should start from the point pointed by current position of the file.
	*    Current position should be updated accordingly after read.
	* Parameter(s):
	*    index: File index which indicates the file to be read.
	*    mem_area: buffer to be returned
	*    count: number of byte(s) to read
	* Return:
	*    Actual number of bytes returned in mem_area[].
	*    -1 value for error case "File hasn't been open"
	*    -2 value for error case "End-of-file"
	TODOs:
	1. Read the open file table using index.
	1.1 Get the file descriptor number and the current position.
	1.2 Can't get proper file descriptor, return -1.
	2. Read the file descriptor
	2.1 Get file size and block array.
	3. Read 'count' byte(s) from file and store in mem_area[].
	3.1 If current position crosses block boundary, call read_block() to read the next block.
	3.2 If current position==file size, stop reading and return.
	3.3 If this is called when current position==file size, return -2.
	3.4 If count > mem_area size, only size of mem_area should be read.
	3.5 Returns actual number of bytes read from file.
	3.6 Update current position so that next read() can be done from the first byte haven't-been-read.
	*/
	int read(int index, char* mem_area, int count);


	/* File Write function:
	*    This writes 'count' number of 'value'(s) to the file indicated by index.
	*    Writing should start from the point pointed by current position of the file.
	*    Current position should be updated accordingly.
	* Parameter(s):
	*    index: File index which indicates the file to be read.
	*    value: a character to be written.
	*    count: Number of repetition.
	* Return:
	*    >0 for successful write
	*    -1 value for error case "File hasn't been open"
	*    -2 for error case "Maximum file size reached" (not implemented.)
	*/
	int write(int index, char value, int count);


	/* Setting new read/write position function:
	* Parameter(s):
	*    index: File index which indicates the file to be read.
	*    pos: New position in the file. If pos is bigger than file size, set pos to file size.
	* Return:
	*    0 for successful write
	*    -1 value for error case "File hasn't been open"
	*/
	int lseek(int index, int pos);


	/* Close file function:
	* Parameter(s):
	*    index: The index of open file table
	* Return:
	*    none
	*/
	void close(int index);


	/* Delete file function:
	*    Delete a file
	* Parameter(s):
	*    symbolic_file_name: a file name to be deleted.
	* Return:
	*    Return 0 with success
	*    Return -1 with error (ie. No such file).
	*/
	int deleteFile(string fileName);


	/* Directory listing function:
	*    List the name and size of files in the directory. (We have only one directory in this project.)
	*    Example of format:
	*       abc 66 bytes, xyz 22 bytes
	* Parameter(s):
	*    None
	* Return:
	*    None
	*/
	void directory();

	/*------------------------------------------------------------------
	Disk management functions.
	These functions are not really a part of file system.
	They are provided for convenience in this emulated file system.
	------------------------------------------------------------------
	Restores the saved disk image in a file to the array.
	*/
	void restore();

	// Saves the array to a file as a disk image.
	void save();

	// Disk dump, from block 'start' to 'start+size-1'.
	void diskdump(int start, int size);

	// Reads block from ldisk and copies it to pointer p
	void read_block(int i,  char *p);

	// Writes block from p and copies it to ldisk at index i
	void write_block(int i,  char *p);

	// Prints out the ldisk
	void print();

	// Return current position in OFTable
	int getCurrentPosition(int index);

	~FileSystem53();
};

//done
FileSystem53::FileSystem53()
{
	ldisk = new char*[l];
	for (int i = 0; i < l; i++)
	{
		ldisk[i] = new char[l];

		if (i == 0)
		{
			for (int j = 0; j < l; j++)
				ldisk[i][j] = '0';
		}
		else if (i < l)
		{
			for (int j = 0; j < l; j++)
				ldisk[i][j] = '\0';
		}

		OpenFileTable();
	}
}

//done
void FileSystem53::read_block(int i,  char *p)
{
	char *q = ldisk[i];
	for (int i = 0; i < l; i++)
	{
			p[i] = q[i];
	}
}

//done
void FileSystem53::write_block(int i,  char *p)
{
	for (int j = 0; j < l; j++)
	{
			ldisk[i][j] = p[j];
	}
}

//done
void FileSystem53::save()
{
	ofstream txtFile("savedFile.txt");
	for (int i = 0; i < l; i++)
	{
		for (int j = 0; j < l; j++)
		{
			if (ldisk[i][j] == '\n')
			{
				txtFile.put(200);
			}
			else if (ldisk[i][j] == '\r')
			{
				txtFile.put(201);
			}
			else
				txtFile.put(ldisk[i][j]);
		}
	}
	txtFile.close();
}

//done
void FileSystem53::restore()
{
	ifstream txtFile("savedFile.txt");
	string str;
	string file_contents;
	int counter = 0;
	if (txtFile.is_open())
	{
		while (getline(txtFile, str))
		{
			file_contents += str;
		}

		for (int i = 0; i < 64; i++)
		{
			for (int j = 0; j < 64; j++)
			{
				if (file_contents[counter] == (char)200)
				{
					ldisk[i][j] = '\n';
				}
				else if ((int)file_contents[counter] == (char)201)
					ldisk[i][j] = '\r';
				else
				{
					ldisk[i][j] = file_contents[counter];
				}
				counter++;
			}
		}
	}
	else
		cout << "\nUnable to open file.";
}

//done
FileSystem53::~FileSystem53()
{
	for (int i = 0; i < l; i++)
	{
		delete ldisk[i];
	}

	delete ldisk;
}

//done
void FileSystem53::print()
{
	char* fileDescriptor = new char[l];
	read_block(1, fileDescriptor);
	for (int i = 0; i < l; i++)
	{
		if (i == 0)
		{
			cout << "Bytemap: ";
		}
		else if (i < 7)
		{
			cout << "File Descriptors: ";
		}
		else
			cout << "Block " << i << ": ";

		for (int j = 0; j < l; j++)
		{
			cout << ldisk[i][j];
		}

		cout << endl;
	}

	cout << endl << "OFTABLE" << endl;
	for (int k = 0; k < 3; k++)
	{
			cout << "Contents of OFTable " << endl;
		for (int j = 0; j < 66; j++)
		{
			cout << OFTable[k][j];
			if (j == 64)
				cout << endl << " Current Position: " << (int)OFTable[k][j] << endl;
			if (j == 65)
				cout << endl << " File length: " << (int)fileDescriptor[OFTable[k][j]] << endl;
		}
		cout << endl;
	}

	delete fileDescriptor;
}

//done
int FileSystem53::create(string symbolic_file_name)
{
	 char* bytemap = new  char[l];
	 char* fileDescriptor = new  char[l];
	 char* directoryFile = new  char[l];

	char tempChar;
	int asciiNum = '\0';
	int directoryIndexFound;
	char fileDescriptorIndex;

	bool flag = false;
	bool found = false;

	read_block(0, bytemap);
	read_block(1, fileDescriptor);

	// check and find a free file descriptor
	int tempCount = 1;
	while (tempCount < l && !flag)
	{
		tempCount += 4;
		if (fileDescriptor[tempCount] == '\0')
		{
			fileDescriptorIndex = tempCount;
			flag = true;
		}
	}

	// out of space
	if (!flag)
	{
		return -1;
	}

	// check the directory's file descriptor to see if the file already exists
	for (int i = 1; i < 4; i++)
	{
		// the directory's file descriptor has an allocated block
		if (fileDescriptor[i] != '\0')
		{
			asciiNum = fileDescriptor[i];
			read_block(asciiNum, directoryFile);

			// loop through every 10-byte filenames within the directory to find the file
			int count = 0;
				for (int j = 0; j < 55; j = count * 11)
				{
					count++;
					// found a free spot in directory file
					if (directoryFile[j] == '\0' && !found)
					{
						directoryIndexFound = j;
						found = true;
					}
					else // search through to find dupes
					{
						int k = 0;
						bool temp = true;
						while (k < symbolic_file_name.length() && temp)
						{
							temp = (symbolic_file_name[k] == directoryFile[j]);
							k++;
							j++;
						}

						// file was found in the directory file
						if (temp)
						{
							return -2;
						}

						temp = true;
					}
				}
		}
		else if (!found) // the directory's file descriptor has an empty block
		{
			// search the bytemap to find an open, empty block
			for (int j = 7; j < l; j++)
			{
				// if found, change the bytemap to 1
				if (bytemap[j] == '0')
				{
					read_block(j, directoryFile);
					bytemap[j] = '1';
					found = true;

					// change the fileDescriptor for directory file
					asciiNum = j;
					tempChar = j;
					fileDescriptor[i] = tempChar;

					directoryIndexFound = 0;
					j = l;
				}
			}
		}
	}

	//add the file name + index to the directory
	int counter = 0;
	for (int n = directoryIndexFound; n < directoryIndexFound + symbolic_file_name.length(); n++)
	{
		directoryFile[n] = symbolic_file_name[counter];
		counter++;
	}

	//update fileDescriptor
	for (int i = 7; i < l; i++)
	{
		if (bytemap[i] == '0')
		{
			bytemap[i] = '1';

			fileDescriptor[tempCount] = i;
			i = l;
		}
	}

	directoryFile[directoryIndexFound + 10] = fileDescriptorIndex - 1;

	int directoryFileNum = fileDescriptor[0];
	directoryFileNum++;
	fileDescriptor[0] = directoryFileNum;

	write_block(1, fileDescriptor);
	write_block(asciiNum, directoryFile);
	write_block(0, bytemap);

	delete bytemap;
	delete fileDescriptor;
	delete directoryFile;

	return 0;
}

//done
int FileSystem53::deleteFile(string symbolic_file_name)
{
	char* bytemap = new char[l];
	char* fileDescriptors = new char[l];
	char* directoryFile = new char[l];

	bool found = true;
	char tempChar;
	int asciiNum;
	int indexOfDirectory;
	int indexOfFileDescriptor;
	int indexOfByteMap;

	// get bytemap
	read_block(0, bytemap);

	// get file descriptors.
	read_block(1, fileDescriptors);

	// get the first filedescriptor  ->  the directory file , and check its three datablocks for the filename
	for (int i = 1; i < 4; i++)
	{
		// find and get directory block
		indexOfDirectory = fileDescriptors[i];
		read_block(indexOfDirectory, directoryFile);

		int count = 0;
		for (int j = 0; j < 55; j = count * 11)
		{
			found = true;
			count++;
			int counter = 0;
			int start = j;
			int end;
			while (counter < symbolic_file_name.length() && found)
			{
				found = (directoryFile[j] == symbolic_file_name[counter]);

				counter++;
				j++;
			}

			// found
			if (found)
			{
				end = (count * 11) - 1;
				indexOfFileDescriptor = directoryFile[end];
				// delete directory file
				while (start <= end)
				{
					directoryFile[start] = '\0';

					start++;
				}
				// loop through file descriptor and delete blocks/bytemap
				int temp = 0;

				while (fileDescriptors[indexOfFileDescriptor] != '\0' && temp < 3)
				{
					indexOfByteMap = fileDescriptors[indexOfFileDescriptor + 1];
					// delete file descriptor
					fileDescriptors[indexOfFileDescriptor] = '\0';

					// delete bytemap index
					bytemap[indexOfByteMap] = '0';

					char* p = new char[l];
					for (int k = 0; k < l; k++)
					{
						p[k] = '\0';
					}

					// delete and update file block
					write_block(indexOfByteMap, p);

					delete p;

					indexOfFileDescriptor++;
					temp++;
				}

				// update bytemap
				write_block(0, bytemap);

				// update directory file descriptor's size
				int directoryFileNum = fileDescriptors[0];
				directoryFileNum--;
				fileDescriptors[0] = directoryFileNum;

				// update file descriptors
				write_block(1, fileDescriptors);

				// update directory file
				write_block(indexOfDirectory, directoryFile);

				delete bytemap;
				delete fileDescriptors;
				delete directoryFile;

				return 0;
			}
		}
	}

	delete bytemap;
	delete fileDescriptors;
	delete directoryFile;

	if (!found)
		return -1;
}

//done
void FileSystem53::OpenFileTable()
{

	oftAllocation = new int[3];
	//initiaize the arrays simulating the table
	// 64 is the current position, 65 is the file descriptor index
	for (int i = 0; i< 3; i++)
	for (int j = 0; j < (l + 2); j++)
	{
		OFTable[i][j] = 0;
	}

	// oft not in use = 0
	for (int j = 0; j < 3; j++)
		oftAllocation[j] = 0;
}

//done
int FileSystem53::open(string symbolic_file_name)
{
	int fileDescriptorNum;
	char* bytemap = new char[l];
	char* fileDescriptors = new char[l];
	char* directoryFile = new char[l];

	bool found = true;
	char tempChar;
	int asciiNum;
	int indexOfDirectory;
	int indexOfFileDescriptor;
	int indexOfByteMap;
	bool alrdyfound = false;

	// get bytemap
	read_block(0, bytemap);

	// get file descriptors.
	read_block(1, fileDescriptors);

	// get the first filedescriptor  ->  the directory file , and check its three datablocks for the filename
	for (int i = 1; i < 4; i++)
	{
		// find and get directory block
		indexOfDirectory = fileDescriptors[i];
		read_block(indexOfDirectory, directoryFile);

		int count = 0;
		for (int j = 0; j < 55; j = count * 11)
		{
			found = true;
			count++;
			int counter = 0;
			while (counter < symbolic_file_name.length() && found)
			{
				found = (directoryFile[j] == symbolic_file_name[counter]);
				counter++;
				j++;
			}

			// if filename is the same, go to the filedescriptor containing it
			if (found)
			{
				// first thing filedescriptor of file directory points to is the filelength
				// gets the index of the file descriptor for the file
				fileDescriptorNum = directoryFile[count*11 - 1];
				alrdyfound = true;

				break;
			}
		}
	}

	if (!alrdyfound)
	{
		delete bytemap;
		delete fileDescriptors;
		delete directoryFile;
		return -1;
	}

	int freeoft = find_oft();
	if (freeoft != -1)
	{
		// has free oft
		char a = fileDescriptorNum;
		OFTable[freeoft][l] = '\0';
		OFTable[freeoft][l + 1] = a;
		read_block(1, fileDescriptors);
		int asciiIndexForFirstBlock = fileDescriptors[fileDescriptorNum + 1];
		read_block(asciiIndexForFirstBlock, OFTable[freeoft]);
		oftAllocation[freeoft] = 1;
		
		delete bytemap;
		delete fileDescriptors;
		delete directoryFile;

		return freeoft;
	}
	else
	{
		delete bytemap;
		delete fileDescriptors;
		delete directoryFile;
		return -2;
	}
}

//done
int FileSystem53::find_oft()
{
	for (int i = 0; i < 3; i++)
	{
		if (oftAllocation[i] == 0)
			return i;
	}
	return -1;
}

//done
void FileSystem53::directory()
{
	char* directoryFile = new char[l];
	char* fileDescriptor = new char[l];

	read_block(1, fileDescriptor);

	int count = 1;
	int temp;
	int numOfFiles = fileDescriptor[0];
	
	while (count < 4)
	{
		if (fileDescriptor[count] != '\0')
		{
			temp = fileDescriptor[count];
			read_block(temp, directoryFile);

			int counter = 0;
			for (int i = 0; i < 55; i = counter * 11)
			{
				counter++;
				//something is there in the 10-byte block
				if (directoryFile[i] != '\0')
				{
					for (int j = i; j < counter * 10; j++)
					{
						if (directoryFile[j] != '\0')
							cout << directoryFile[j];
					}

					numOfFiles--;
					if (numOfFiles == 0)
						cout << " " << (int) fileDescriptor[(int) directoryFile[(counter * 11) - 1]] << " bytes";
					else
						cout << " " << (int) fileDescriptor[(int) directoryFile[(counter * 11) - 1]] << " bytes, ";
				}
			}
		}
		count++;
	}

	delete directoryFile;
	delete fileDescriptor;
}

//done
int FileSystem53::read(int index, char* mem_area, int count)
{
	int fileDescriptorIndex = OFTable[index][65];
	int currentPosition = OFTable[index][64];
	int blockIndex = currentPosition % 64;
	int blockNumber;
	int actualValue = 0;

	char* fileDescriptor = new char[l];

	if (count > 64)
		count = 64;

	read_block(1, fileDescriptor);

	int fileSize = fileDescriptor[fileDescriptorIndex];

	if (fileDescriptor[fileDescriptorIndex] == '\0')
	{
		delete fileDescriptor;

		return -1;
	}
	
	blockNumber = (currentPosition / l) + 1;
	char* file = new char[l];
	char* buffer = new char[l];
	
	for (int i = 0; i < count; i++)
	{
		blockNumber = (currentPosition / l) + 1;
		blockIndex = currentPosition % 64;

		// runs when it reaches the end of a file block
		//need new file block in buffer
		if(blockIndex == 0 && blockNumber > 1)
		{

			//clear buffer
			for (int k = 0; k < l; k++)
			{
				buffer[k] = OFTable[index][k];
				OFTable[index][k] = '\0';
				
			}
			
			//write buffer to ldisk
			write_block(fileDescriptor[fileDescriptorIndex + blockNumber - 1], buffer);
			//get new block from ldisk
			read_block(fileDescriptor[fileDescriptorIndex + blockNumber], file);

			//put new block into OFTable
			for (int k = 0; k < l; k++)
			{
				OFTable[index][k] = file[k];
			}
		}

		//write from buffer into mem_area
		if (OFTable[index][blockIndex] != '\0')
		{
			mem_area[i] = OFTable[index][blockIndex];
			actualValue++;
			currentPosition++;
		}
	}
	
	int countTemp = 1;
	int length = 0;
	while (fileDescriptor[OFTable[index][65] + countTemp] != '\0' && countTemp < 4)
	{
		read_block(fileDescriptor[OFTable[index][65] + countTemp], file);
		int counter = 0;
		while (file[counter] != '\0' && counter < 64)
		{
			length++;
			counter++;
		}

		countTemp++;
	}

	fileDescriptor[OFTable[index][65]] = length;
	write_block(1, fileDescriptor);

	OFTable[index][64] = currentPosition;

	if (currentPosition == fileSize)
	{
		delete fileDescriptor;
		delete file;
		delete buffer;

		return -2;
	}

	delete fileDescriptor;
	delete file;
	delete buffer;

	return actualValue;
}

//done
void FileSystem53::close(int index)
{
	char* fileDescriptors = new char[l];
	char* buffer = new char[l];
	char* fileBlock = new char[l];

	//clear the "fake buffer"
	for (int i = 0; i < 64; i++)
	{
		buffer[i] = '\0';
	}

	int fileLengthIndex = 0;
	int length = 0;

	read_block(1, fileDescriptors);

	//currently pointing to file length block in file descriptor
	int fileDescriptorIndex = OFTable[index][65];

	int blockNumber = OFTable[index][64] / l + 1;
	int fileBlockIndex = fileDescriptors[fileDescriptorIndex + blockNumber];
	fileLengthIndex = fileDescriptors[fileDescriptorIndex];

	int count = 0;
	while (count < 64 && OFTable[index][count] != '\0')
	{
		buffer[count] = OFTable[index][count];
		count++;
	}
	write_block(fileBlockIndex, buffer);


	count = 1;
	while (fileDescriptors[fileDescriptorIndex + count] != '\0' && count < 4)
	{
		read_block(fileDescriptors[fileDescriptorIndex + count], fileBlock);
		int counter = 0;
		while (fileBlock[counter] != '\0' && counter < 64)
		{
			length++;
			counter++;
		}

		count++;
	}

	fileDescriptors[fileDescriptorIndex] = length;
	write_block(1, fileDescriptors);

	for (int j = 0; j < (l + 2); j++)
		OFTable[index][j] = '\0';

	oftAllocation[index] = 0;

	delete fileBlock;
	delete fileDescriptors;
	delete buffer;
}

//done
int FileSystem53::lseek(int index, int pos)
{
	char* fileDescriptors = new char[l];
	char* fileBlock = new char[l];
	char* buffer = new char[l];
	int newBlockNumber = (pos / l) + 1;
	int currentBlockNumber = (OFTable[index][64] / l) + 1;

	read_block(1, fileDescriptors);

	if (oftAllocation[index] == 0)
	{
		delete fileBlock;
		delete fileDescriptors;
		delete buffer;
		return -1;
	}

	if (pos > OFTable[index][64])
		pos = OFTable[index][64];

	if (newBlockNumber != currentBlockNumber)
	{
		for (int i = 0; i < 64; i++)
		{
			buffer[i] = OFTable[index][i];
			OFTable[index][i] = '\0';
		}
		write_block(fileDescriptors[OFTable[index][65] + currentBlockNumber], buffer);
		read_block(fileDescriptors[OFTable[index][65] + newBlockNumber], buffer);
		for (int i = 0; i < 64; i++)
		{
			OFTable[index][i] = buffer[i];
		}
		

		OFTable[index][64] = pos;
	}
	else if (currentBlockNumber == newBlockNumber)
	{
		OFTable[index][64] = pos;
	}

	delete fileBlock;
	delete fileDescriptors;
	delete buffer;

	return 0;
}

//done
int FileSystem53::write(int index, char value, int count)
{
	if (oftAllocation[index] == 0)
		return -1;

	int currentPosition = OFTable[index][64];
	//right now this index is pointing to file length
	int fileDescriptorIndex = OFTable[index][65];
	int fileIndex;
	int blockNumber = (currentPosition / l) + 1;
	bool flag = false;

	char* fileDescriptor = new char[l];
	char* fileBlock = new char[l];
	char* bytemap = new char[l];
	char* buffer = new char[l];

	read_block(1, fileDescriptor);
	read_block(0, bytemap);

	fileIndex = fileDescriptor[fileDescriptorIndex + blockNumber];
	read_block(fileIndex, fileBlock);
	

	for (int i = 0; i < count; i++)
	{
		blockNumber = (currentPosition / l) + 1;

		// when block is full and new block needs to be allocated for bytemap and file descriptor
		if (fileDescriptor[fileDescriptorIndex + blockNumber] == '\0')
		{
			int counter = 6;
			while (counter < 64 && !flag)
			{
				counter++;
				if (bytemap[counter] == '0')
				{
					bytemap[counter] = '1';
					write_block(0, bytemap);
					flag = true;
				}
			}

			fileDescriptor[fileDescriptorIndex + blockNumber] = counter;
			write_block(1, fileDescriptor);

			//write OFT's buffer into buffer and clear OFT buffer
			for (int k = 0; k < l; k++)
			{
				buffer[k] = OFTable[index][k];
				OFTable[index][k] = '\0';
			}

			//write buffer into ldisk
			//this might error
			write_block(fileDescriptor[fileDescriptorIndex + blockNumber - 1], buffer);
		}

		//only if buffer is empty
		if (currentPosition < 64)
			OFTable[index][currentPosition] = value;
		else if (currentPosition < 128)
			OFTable[index][currentPosition - 64] = value;
		else
			OFTable[index][currentPosition - 128] = value;

		OFTable[index][64] = ++currentPosition;
	}

	int countTemp = 1;
	int length = 0;
	while (fileDescriptor[OFTable[index][65] + countTemp] != '\0' && countTemp < 4)
	{
		read_block(fileDescriptor[OFTable[index][65] + countTemp], fileBlock);
		int counter = 0;
		while (fileBlock[counter] != '\0' && counter < 64)
		{
			length++;
			counter++;
		}

		countTemp++;
	}

	fileDescriptor[OFTable[index][65]] = length;
	write_block(1, fileDescriptor);

	delete fileBlock;
	delete fileDescriptor;
	delete buffer;
	delete bytemap;

	return 0;
}

//done
int FileSystem53::getCurrentPosition(int index)
{
	return OFTable[index][64];
}


int main()
{
	FileSystem53 *fileSystem = new FileSystem53();
	string temp = "";
	ifstream input;

	int x, y;
	int returnedValue = 0;
	int init = 0;

	input.open("Text.txt");

	while (!input.eof())
	{
		getline(input, temp);

		string buffer;
		stringstream ss(temp);

		vector<string> tokens;

		while (ss >> buffer)
			tokens.push_back(buffer);

		if (tokens[0] == "op") {
			returnedValue = fileSystem->open(tokens[1]);
			if (returnedValue > -1)
				cout << "file " << tokens[1] << " opened, index = " << returnedValue + 1 << endl;
			else
				cout << "error" << endl;
		}
		else if (tokens[0] == "cl") {
			stringstream kk(tokens[1]);
			kk >> x;
			fileSystem->close(x-1);
			cout << "file with index " << x << " closed" << endl;
		}
		else if (tokens[0] == "rd") {
			stringstream kk(tokens[1]);
			kk >> x;
			stringstream jj(tokens[2]);
			jj >> y;

			char* p = new char[64];
			for (int i = 0; i < 64; i++)
			{
				p[i] = '\0';
			}
			returnedValue = fileSystem->read(x-1, p, y);

			if (returnedValue != -1 && returnedValue != -2) {
				cout << returnedValue << " bytes read: ";
				for (int r = 0; r < 64; r++)
					cout << p[r];
				cout << endl;

				delete p;
			}
			else
				cout << "error" << endl;
		}
		else if (tokens[0] == "wr") {
			stringstream kk(tokens[1]);
			kk >> x;
			stringstream jj(tokens[3]);
			jj >> y;

			returnedValue = fileSystem->write(x-1, tokens[2][0], y);
			if (returnedValue == 0)
				cout << y << " bytes written" << endl;
			else
				cout << "error" << endl;
		}
		else if (tokens[0] == "cr") {
			returnedValue = fileSystem->create(tokens[1]);
			if (returnedValue == 0)
				cout << "file " << tokens[1] << " created" << endl;
			else
				cout << "error" << endl;
		}
		else if (tokens[0] == "de") {
			returnedValue = fileSystem->deleteFile(tokens[1]);
			if (returnedValue == 0)
				cout << "file " << tokens[1] << " deleted" << endl;
			else
				cout << "error" << endl;
		}
		else if (tokens[0] == "sk") {
			stringstream kk(tokens[1]);
			kk >> x;
			stringstream jj(tokens[2]);
			jj >> y;
			returnedValue = fileSystem->lseek(x-1, y);
			if (returnedValue == 0)
				cout << "current position is " << fileSystem->getCurrentPosition(x-1) << endl;
			else
				cout << "error" << endl;
		}
		else if (tokens[0] == "dr") {
			fileSystem->directory();
			cout << endl;
		}
		else if (tokens[0] == "q") {
			break;
		}
		else if (tokens[0] == "sv") {
			fileSystem->save();
			cout << "disk saved" << endl;
		}
		else if (tokens[0] == "in") {
			if (init == 0) {
				cout << "disk initialized" << endl;
				init = 1;
			}
			else
			{
				fileSystem->restore();
				cout << "disk restored" << endl;
			}
		}
	}

	input.close();

	cout << endl;
	system("pause");
	return 0;
}