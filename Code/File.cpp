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

#include "File.hpp"

#include <string>
using namespace std;

File::File(TelaString filename)
{
	mFilename = filename;
	mIsOpen = false;

}

File::~File()
{
}

bool File::open(OpenMode openMode, bool binary)
{
	mOpenMode = openMode;

	mFile.open(mFilename.c_str(), (std::ios_base::openmode)openMode | fstream::app);

	if(mFile.is_open())
	{
		mIsOpen = true;

	}
	else
	{
		mIsOpen = false;
		mLastError = ERROR_FILE_OPEN_FAILED;
	}

	return mIsOpen;
}

void File::close()
{
	mFile.close();
	mIsOpen = false;
}

TelaString File::getLine(int lineNumber)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return "";
	}
	if(mOpenMode == WriteOnly)
	{
		mLastError = ERROR_FILE_MODE_WRITE_ONLY;
		return "";
	}
//	int cursor = mFile.tellg();
	mFile.seekg(fstream::beg);
	for(int i = 0; i < lineNumber && !mFile.eof(); i++)
	{
		getNextLine();
	}
	if(mFile.eof())
	{
		mLastError = ERROR_FILE_EOF;
		return "";
	}

	return getNextLine();
}

TelaString File::getNextLine()
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return "";
	}
	if(mOpenMode == WriteOnly)
	{
		mLastError = ERROR_FILE_MODE_WRITE_ONLY;
		return "";
	}
	if(mFile.eof())
	{
		mLastError = ERROR_FILE_EOF;
		return "";
	}
	char c[256];
	mFile.getline(c, 256);


	return TelaString(c);
}

TelaString File::getStr(TelaString tag, bool removeSpaces)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return "";
	}
	if(mOpenMode == WriteOnly)
	{
		mLastError = ERROR_FILE_MODE_WRITE_ONLY;
		return "";
	}

	mFile.clear();
	mFile.seekg(0);
	string str;
	TelaString str2;
    while(getline(mFile, str))
	{
		int i = str.find(tag);
		if(i == -1) continue;
		str2 = str.substr(0, i);
		//check if there is nothing then ' ' before the tag
		if(str2.size() == 0 || str2.contains(' ') == str2.size())
		{
			i = str.find('=', tag.size() + i);
			str2 = str.substr(i + 1, str.size() - i);
			if(removeSpaces)
			{
				i = str2.findFirstNotOf(' ');
				str2 = str2.substr(i, str.size() - i);
				i = str2.findLastNotOf(' ');
				str2 = str2.substr(0, i + 1);
			}

			mFile.clear();
			return str2;
		}
	}
    mLastError = ERROR_TAG_NOT_FOUND_;
	mFile.clear();
	return "";
}

TelaString File::getStr(TelaString tag, TelaString start, bool removeSpaces)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return "";
	}
	if(mOpenMode == WriteOnly)
	{
		mLastError = ERROR_FILE_MODE_WRITE_ONLY;
		return "";
	}

	mFile.clear();
	mFile.seekg(0);
	string str;
	TelaString str2;
    while(getline(mFile, str))
	{
		if(start == str) break;
	}
	if(mFile.eof())
	{
		mLastError = ERROR_FILE_EOF;
		return "";
	}
	while(!mFile.eof())
	{
		str = getNextLine();
		int i = str.find(tag);
		if(i == -1) continue;
		str2 = str.substr(0, i);
		//check if there is nothing then ' ' before the tag
		if(str2.contains(' ') == str2.size())
		{
			i = str.find('=', tag.size() + i);
			str2 = str.substr(i + 1, str.size() - i);
			if(removeSpaces)
			{
				i = str2.findFirstNotOf(' ');
				str2 = str2.substr(i);
				i = str2.findLastNotOf(' ');
				str2 = str2.substr(0, i + 1);
			}

			mFile.clear();
			return str2;
		}
	}
    mLastError = ERROR_TAG_NOT_FOUND_;
	mFile.clear();
	return "";
}

int File::getInt(TelaString tag)
{
	int ret = 0;
	int prevError = mLastError;
	mLastError = ERROR_NO_ERROR;
	try
	{
		ret = getStr(tag, true).toInt();
	}
	catch(...)
	{
		if(mLastError == ERROR_NO_ERROR)
		{
			mLastError = ERROR_IS_NOT_A_INTEGER;
		}
	}

	if(mLastError == ERROR_NO_ERROR)
	{
		mLastError = prevError;
	}

	return ret;
}

int File::getInt(TelaString tag, TelaString start)
{
	int ret = 0;
	int prevError = mLastError;
	mLastError = ERROR_NO_ERROR;
	try
	{
		ret = getStr(tag, start, true).toInt();
	}
	catch(...)
	{
		if(mLastError == ERROR_NO_ERROR)
		{
			mLastError = ERROR_IS_NOT_A_INTEGER;
		}
	}

	if(mLastError == ERROR_NO_ERROR)
	{
		mLastError = prevError;
	}

	return ret;
}

double File::getDouble(TelaString tag)
{
	double ret = 0;
	int prevError = mLastError;
	mLastError = ERROR_NO_ERROR;
	try
	{
		ret = getStr(tag, true).toDouble();
	}
	catch(...)
	{
		if(mLastError == ERROR_NO_ERROR)
		{
			mLastError = ERROR_IS_NOT_A_DOUBLE;
		}
	}

	if(mLastError == ERROR_NO_ERROR)
	{
		mLastError = prevError;
	}

	return ret;
}

double File::getDouble(TelaString tag, TelaString start)
{
	double ret = 0;
	int prevError = mLastError;
	mLastError = ERROR_NO_ERROR;
	try
	{
		ret = getStr(tag, start, true).toDouble();
	}
	catch(...)
	{
		if(mLastError == ERROR_NO_ERROR)
		{
			mLastError = ERROR_IS_NOT_A_DOUBLE;
		}
	}

	if(mLastError == ERROR_NO_ERROR)
	{
		mLastError = prevError;
	}

	return ret;
}


void File::add(TelaString contents)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return;
	}
	if(mOpenMode == ReadOnly)
	{
		mLastError = ERROR_FILE_MODE_READ_ONLY;
		return;
	}

	mFile.clear();
	mFile << contents.c_str();
}

void File::add(int contents)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return;
	}
	if(mOpenMode == ReadOnly)
	{
		mLastError = ERROR_FILE_MODE_READ_ONLY;
		return;
	}

	mFile << contents;
}

void File::add(double contents)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return;
	}
	if(mOpenMode == ReadOnly)
	{
		mLastError = ERROR_FILE_MODE_READ_ONLY;
		return;
	}

	mFile.clear();
	mFile << contents;
}


void File::addLine(TelaString contents)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return;
	}
	if(mOpenMode == ReadOnly)
	{
		mLastError = ERROR_FILE_MODE_READ_ONLY;
		return;
	}

	mFile.clear();
	mFile << contents.c_str() << endl;
}

void File::addLine(int contents)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return;
	}
	if(mOpenMode == ReadOnly)
	{
		mLastError = ERROR_FILE_MODE_READ_ONLY;
		return;
	}

	mFile.clear();
	mFile << contents << endl;
}

void File::addLine(double contents)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return;
	}
	if(mOpenMode == ReadOnly)
	{
		mLastError = ERROR_FILE_MODE_READ_ONLY;
		return;
	}

	mFile.clear();
	mFile << contents << endl;
}

void File::addLine(TelaString contents, int value)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return;
	}
	if(mOpenMode == ReadOnly)
	{
		mLastError = ERROR_FILE_MODE_READ_ONLY;
		return;
	}

	mFile.clear();
	mFile << contents.c_str() << "=" << value << endl;
}

void File::addLine(TelaString contents, double value)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return;
	}
	if(mOpenMode == ReadOnly)
	{
		mLastError = ERROR_FILE_MODE_READ_ONLY;
		return;
	}

	mFile.clear();
	mFile << contents.c_str() << "=" << value << endl;
}


int File::getFileLength()
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return -1;
	}
	if(mOpenMode == WriteOnly)
	{
		mLastError = ERROR_FILE_MODE_WRITE_ONLY;
		return -1;
	}

	mFile.clear();

    unsigned int length;
    unsigned int cursor = (unsigned int)mFile.tellg();
	mFile.seekg(0, ios::end);
    length = (unsigned int)mFile.tellg();
	mFile.seekg(cursor);

	return length;
}

void File::replace(TelaString oldContents, TelaString newContents)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return;
	}
	if(mOpenMode != ReadWrite)
	{
		mLastError = ERROR_FILE_MODE_NOT_READ_WRITE;
		return;
	}

	fstream newFile;
	newFile.open(mFilename + ".new", (std::ios_base::openmode)ReadWrite | fstream::app);

	mFile.seekg(0, ios::beg);

	TelaString buf;
	TelaString contents = "";
	while(!mFile.eof())
	{
		buf = getNextLine();
		int spot = buf.find(oldContents.c_str());
		if(spot >= 0)
		{
			TelaString tmpstring = buf.substr(0, spot);
			tmpstring += newContents;
			tmpstring += buf.substr(spot + oldContents.size(), buf.size());
			buf = tmpstring;
		}
		newFile << (const char *)buf << endl;
	}

	newFile.close();
	mFile.close();
	remove(mFilename);
	rename((mFilename + ".new").c_str(), mFilename);
	mFile.open(mFilename.c_str(), (std::ios_base::openmode)mOpenMode | fstream::app);


}

void File::erase(TelaString oldContents)
{
	if(mIsOpen == false)
	{
		mLastError = ERROR_FILE_NOT_OPEN;
		return;
	}
	if(mOpenMode != ReadWrite)
	{
		mLastError = ERROR_FILE_MODE_NOT_READ_WRITE;
		return;
	}



	fstream newFile;
	newFile.open(mFilename + ".new", (std::ios_base::openmode)ReadWrite | fstream::app);


	mFile.seekg(0, ios::beg);

	TelaString buf;
	TelaString contents = "";
	while(!mFile.eof())
	{
		buf = getNextLine();
		int spot = buf.find(oldContents.c_str());
		if(spot >= 0)
		{
			TelaString tmpstring = buf.substr(0, spot);
			tmpstring += (string)buf.substr(spot + oldContents.size(), buf.size());
			buf = tmpstring;
		}
		newFile << (const char *)buf << endl;
	}

	newFile.close();
	mFile.close();
	remove(mFilename);
	rename((mFilename + ".new").c_str(), mFilename);
	mFile.open(mFilename.c_str(), (std::ios_base::openmode)mOpenMode | fstream::app);


}

int File::getLastError()
{
	return mLastError;
}

void File::resetLastError()
{
	mLastError = ERROR_NO_ERROR;
}
