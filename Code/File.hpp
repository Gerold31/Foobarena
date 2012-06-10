/*******************************************************************
This file is part of TelaMitto.

TelaMitto is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TelaMitto distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TelaMitto.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#ifndef __filehandling_h_
#define __filehandling_h_

#include "TelaString.hpp"
#include <fstream>
using namespace std;

class File
{
public:
	enum OpenMode
	{
		ReadOnly = fstream::in,
		WriteOnly = fstream::out,
        ReadWrite = ReadOnly | WriteOnly
	};

	enum ErrorCodes
	{
		ERROR_NO_ERROR = 0,
		ERROR_FILE_OPEN_FAILED,
		ERROR_FILE_NOT_OPEN,
		ERROR_FILE_MODE_WRITE_ONLY,
		ERROR_FILE_MODE_READ_ONLY,
		ERROR_FILE_MODE_NOT_READ_WRITE,
		ERROR_FILE_EOF,
		ERROR_TAG_NOT_FOUND,
		ERROR_IS_NOT_A_INTEGER,
        ERROR_IS_NOT_A_DOUBLE
	};

	File(TelaString filename);
	~File();
	bool open(OpenMode openMode = ReadWrite, bool binary = false);
	void close();
	TelaString getLine(int lineNumber);
	TelaString getNextLine();
	TelaString getStr(TelaString tag, bool removeSpaces = true);
	TelaString getStr(TelaString tag, TelaString start, bool removeSpaces = true);
	int getInt(TelaString tag);
	int getInt(TelaString tag, TelaString start);
	double getDouble(TelaString tag);
	double getDouble(TelaString tag, TelaString start);

	void add(TelaString contents);
	void add(int contents);
	void add(double contents);

	void addLine(TelaString contents);
	void addLine(int contents);
	void addLine(double contents);
	void addLine(TelaString contents, int value);
	void addLine(TelaString contents, double value);

	int getFileLength();
	void replace(TelaString oldContents, TelaString newContents);
	void erase(TelaString contents);

	int getLastError();
	void resetLastError();


private:
	TelaString mFilename;
	fstream mFile;
	bool mIsOpen;
	unsigned int mLineNumber;
	OpenMode mOpenMode;
	int mLastError;

	/*********Errorcodes********
	0 - None
	1 - Could not open file
	2 - File is not open
	3 - Can not Read from file, file is WriteOnly
	4 - Can not Write in file, file is ReadOnly
	5 - File is not ReadWrite
	6 - EOF reached
	7 - Tag not found
	8 - Start not found
	***************************/
};

#endif
