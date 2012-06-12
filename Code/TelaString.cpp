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

#include "TelaString.hpp"

typedef unsigned int uint;

/////////////////
// constructor //
/////////////////

TelaString::TelaString()
{
	content.clear();
}

TelaString::TelaString(int input)
{
	operator=((long)input);
}

TelaString::TelaString(long input)
{
	operator=(input);
}

TelaString::TelaString(size_t input)
{
	operator=((long)input);
}

TelaString::TelaString(double input)
{
	operator=(input);
}

TelaString::TelaString(char *input)
{
	operator=(input);
}

TelaString::TelaString(const char *input)
{
	operator=(input);
}

TelaString::TelaString(const char input)
{
	operator=(input);
}

TelaString::TelaString(string input)
{
	operator=(input);
}


///////////
// utils //
///////////

void TelaString::clear()
{
	content.clear();
}

int TelaString::size()
{
	return content.size();
}


////////////////
// operator = //
////////////////

TelaString &TelaString::operator=(int input)
{
	return operator=((long)input);
}

TelaString &TelaString::operator=(long input)
{
	content.clear();
	bool negative = false;
	if(input == 0)
	{
		content.push_back(48);
	}
	else
	{
		if(input < 0)
		{
			input *= -1;
			negative = true;
		}
		while(input > 0)
		{
			content.push_front(input % 10 + 48);
			input /= 10;
		}
		if(negative)
			content.push_front(45);
	}
	return *this;
}

TelaString &TelaString::operator=(size_t input)
{
	return operator=((long)input);
}

TelaString &TelaString::operator=(double input)
{
	content.clear();
	bool negative = false;
	bool all0 = true;
    long n = (long)(input * 100000);
	if(n == 0)
	{
		content.push_back(48);
	}
	else
	{
		int i = 0;
		if(n < 0)
		{
			n *= -1;
			negative = true;
		}
		while(n > 0)
		{
			if(i == 5 && !all0)
				content.push_front('.');
			if(i < 5 && all0 && n % 10 == 0)
			{
			}
			else if(i < 5 && n % 10 != 0)
			{
				all0 = false;
				content.push_front(n % 10 + 48);
			}
			else
			{
				content.push_front(n % 10 + 48);
			}
			n /= 10;
			i++;
		}
		while(i <= 5)
		{
			if(i == 5)
				content.push_front('.');
			content.push_front('0');
			i++;
		}
		if(negative)
			content.push_front(45);
	}
	return *this;
}

TelaString &TelaString::operator=(char *input)
{
	content.clear();
	operator+=(input);
	return *this;
}

TelaString &TelaString::operator=(const char *input)
{
	content.clear();
	operator+=(input);
	return *this;
}

TelaString &TelaString::operator=(const char input)
{
	content.clear();
	operator+=(input);
	return *this;
}

TelaString &TelaString::operator=(string input)
{
	return operator=(input.c_str());
}

/*
TelaString &TelaString::operator=(TelaString input)
{
	return operator=(input.c_str());
//	return *this;
}*/


/////////////////
// operator += //
/////////////////


void TelaString::operator+=(int input)
{
	TelaString tmp(input);
	operator+=(tmp);
}

void TelaString::operator+=(long input)
{
	TelaString tmp(input);
	operator+=(tmp);
}

void TelaString::operator+=(size_t input)
{
	operator+=((int)input);
}

void TelaString::operator+=(double input)
{
	TelaString tmp(input);
	operator+=(tmp);
}

void TelaString::operator+=(char *input)
{
    for(uint i = 0; i < strlen(input); i++)
	{
		content.push_back(input[i]);
	}
}

void TelaString::operator+=(const char *input)
{
	for(uint i = 0; i < strlen(input); i++)
	{
		content.push_back(input[i]);
	}
}

void TelaString::operator+=(const char input)
{
	content.push_back(input);
}

void TelaString::operator+=(string input)
{
	for(uint i = 0; i < input.size(); i++)
	{
		content.push_back(input.at(i));
	}
}

void TelaString::operator+=(TelaString input)
{
    for(uint i = 0; i < (uint)input.size(); i++)
	{
		content.push_back(input.at(i));
	}
}


/////////////////
// operator == //
/////////////////

bool TelaString::operator==(int input)
{
	return (toInt() == input);
}

bool TelaString::operator==(size_t input)
{
	return operator==((int)input);
}

bool TelaString::operator==(double input)
{
	return (toDouble() == input);
}

bool TelaString::operator==(char *input)
{
	return operator==((const char *) input);
}

bool TelaString::operator==(const char *input)
{
	if(this->size() != strlen(input))
	{
		return false;
	}
    for(uint i = 0; i < (uint)this->size(); i++)
	{
		if(this->at(i) != input[i])
		{
			return false;
		}
	}
	return true;
}

bool TelaString::operator==(const char input)
{
	if(content.size() != 1)
		return false;
	if(this->at(0) == input)
		return true;
	return false;
}

bool TelaString::operator==(string input)
{
    return (*this == TelaString(input));
}

bool TelaString::operator==(TelaString input)
{
	if(this->size() != input.size())
	{
		return false;
	}
    for(uint i = 0; i < (uint)this->size(); i++)
	{
		if(this->at(i) != input.at(i))
		{
			return false;
		}
	}
	return true;
}


/////////////////
// operator != //
/////////////////

bool TelaString::operator!=(int input)
{
	return !(this->operator==(input));
}

bool TelaString::operator!=(size_t input)
{
	return !(this->operator==(input));
}

bool TelaString::operator!=(double input)
{
	return !(this->operator==(input));
}

bool TelaString::operator!=(char *input)
{
	return !(this->operator==(input));
}

bool TelaString::operator!=(const char *input)
{
	return !(this->operator==(input));
}

bool TelaString::operator!=(const char input)
{
	return !(this->operator==(input));
}

bool TelaString::operator!=(string input)
{
	return !(this->operator==(input));
}

bool TelaString::operator!=(TelaString input)
{
	return !(this->operator==(input));
}

#ifdef OGRE
bool operator!=(Ogre::String input)
{
	return !(this->operator==(input));
}

#endif


////////////////
// operator + //
////////////////

TelaString TelaString::operator+(int input)
{
	TelaString str = *this;
	str += input;
	return str;
}

TelaString TelaString::operator+(long input)
{
	TelaString str = *this;
	str += input;
	return str;
}

TelaString TelaString::operator+(size_t input)
{
	return operator+((int)input);
}

TelaString TelaString::operator+(double input)
{
	TelaString str = *this;
	str += input;
	return str;
}

TelaString TelaString::operator+(char *input)
{
	TelaString str = *this;
	str += input;
	return str;
}

TelaString TelaString::operator+(const char *input)
{
	TelaString str = *this;
	str += input;
	return str;
}

TelaString TelaString::operator+(const char input)
{
	TelaString str = *this;
	str += input;
	return str;
}

TelaString TelaString::operator+(string input)
{
	TelaString str = *this;
	str += input;
	return str;
}

TelaString TelaString::operator+(TelaString input)
{
	TelaString str = *this;
	str += input;
	return str;
}


/////////////////
// operator << //
/////////////////

void TelaString::operator<<(int input)
{
	operator+=(input);
}

void TelaString::operator<<(size_t input)
{
	operator+=((int)input);
}

void TelaString::operator<<(double input)
{
	operator+=(input);
}

void TelaString::operator<<(char *input)
{
	operator+=(input);
}

void TelaString::operator<<(const char *input)
{
	operator+=(input);
}

void TelaString::operator<<(const char input)
{
	operator+=(input);
}

void TelaString::operator<<(string input)
{
	operator+=(input);
}

void TelaString::operator<<(TelaString input)
{
	operator+=(input);
}


/////////////////
// operator >> //
/////////////////

void TelaString::operator>>(int &input)
{
	input = toInt();
}


void TelaString::operator>>(size_t &input)
{
	input = toInt();
}

void TelaString::operator>>(double &input)
{
	input = toDouble();
}

void TelaString::operator>>(char* &input)
{
	input = toChar();
}

void TelaString::operator>>(string &input)
{
	input = toString();
}

void TelaString::operator>>(TelaString &input)
{
	input = *this;
}


/////////////////
// operator ^= //
/////////////////

bool TelaString::operator^=(char *input)
{
	TelaString str = input;
	return operator^=(str);
}

bool TelaString::operator^=(const char *input)
{
	TelaString str = input;
	return operator^=(str);
}

bool TelaString::operator^=(const char input)
{
	return operator^=((TelaString)input);
}

bool TelaString::operator^=(string input)
{
	return operator^=((TelaString)input);
}

bool TelaString::operator^=(TelaString input)
{
//	return operator^=(input.toString());
	if(input.size() != content.size())
		return false;

    for(uint i = 0; i < (uint)input.size(); i++)
	{
		bool tmp = false;

		if(input.at(i) == this->at(i))
			tmp = true;

		if(input.at(i) >= 0x41 && input.at(i) <= 0x5A)
			if(input.at(i) == this->at(i) - 32)
				tmp = true;

		if(input.at(i) >= 0x61 && input.at(i) <= 0x7A)
			if(input.at(i) - 32 == this->at(i))
				tmp = true;

		if(!tmp)
			return false;
	}
	return true;
}

///////////////////
// cast operator //
///////////////////

TelaString::operator char *()
{
	return this->toChar();
}

TelaString::operator const char *()
{
	return this->c_str();
}

////////////////////////
// the "to"-commands //
////////////////////////

int TelaString::toInt()
{
    return atoi(toString().c_str());
}

double TelaString::toDouble()
{
    return atof(toString().c_str());
}

char *TelaString::toChar()
{
	char *output = new char[content.size()+1];
	int k = 0;
	for(deque<char>::iterator i = content.begin(); i != content.end(); ++i)
	{
		output[k] = (*i);
		k++;
	}
	output[k] = '\0';
	return output;
}

char TelaString::at(unsigned int position)
{
	if(position <= content.size() && position >= 0)
	{
		try
		{
			return content.at(position);
		}
		catch(...){}
	}
	return 0;
}

string TelaString::toString()
{
	return string(toChar());
}

const char *TelaString::c_str()
{
	return toChar();
}


TelaString TelaString::replace(char c, char d)
{
	for(deque<char>::iterator i = content.begin(); i != content.end(); ++i)
	{
		if((*i) == c)
		{
			(*i) = d;
		}
	}
	return *this; /// @bug useless to return replace, you already have the string, right? Just means copying string: useless and inefficient
}

TelaString TelaString::remove(char c)
{
	for(deque<char>::iterator i = content.begin(); i != content.end(); ++i)
	{
		if((*i) == c)
		{
			content.erase(i);
		}
	}
	return *this; /// @bug useless to return replace, you already have the string, right? Just means copying string: useless and inefficient
}

int TelaString::find(TelaString str, unsigned int startPos)
{
    if((uint)this->size() < startPos)
	{
		return -1;
	}

	if(!str.size())
	{
		return -1;
	}

    if((uint)str.size() > ((uint)this->size() - startPos))
	{
		return -1;
	}

    for(unsigned int position = startPos; position < (uint)this->size(); position++)
	{
		if(this->at(position) == str.at(0))
		{
			bool isEqual = true;
			unsigned int j = position + 1;
            for(unsigned int i = 1; i < (uint)str.size() && j < (uint)this->size(); i++, j++)
			{
				if(this->at(j) != str.at(i))
				{
					isEqual = false;
					break;
				}
			}
			if(isEqual)
			{
				return position;
			}
		}
	}

	return -1;
}

int TelaString::contains(TelaString str, unsigned int startPos)
{
    if((uint)this->size() < startPos)
	{
		return 0;
	}

	if(!str.size())
	{
		return 0;
	}

    if((uint)str.size() > ((uint)this->size() - startPos))
	{
		return 0;
	}

	unsigned int finds = 0, i = startPos;
    while(i < (uint)this->size())
	{
		if(substr(i, str.size()) == str)
		{
			finds++;
			i += str.size();
		}
		else
			i++;
	}

	return finds;
}

TelaString TelaString::substr(unsigned int startPos, unsigned int n)
{
    if(startPos > (uint)this->size()) return "";
    if(n == (uint) - 1) return substr(startPos, this->size() - startPos);
    if(startPos + n > (uint)this->size()) return substr(startPos, this->size() - startPos);

	TelaString str;
	for(uint i = 0; i < n; i++)
	{
		str += this->at(startPos + i);
	}
	return str;
}

TelaString TelaString::section(char sectionIndicator, int startSection, int numberOfSections)
{
	if((numberOfSections == 0) || (numberOfSections < -1))
		return "";

	int startPos = 0;
    for(uint i = 0; i < (uint)startSection; i++)
	{
		startPos = this->find(sectionIndicator, startPos);
		// check if invalid, else return empty string
		if(startPos == -1)
			return "";
		// increase to first char after sectionIndicator, increase here, to be able to check, if startPos is -1 before
		startPos++;
	}

	if(numberOfSections == -1)
		return this->substr(startPos);

	int endPos = startPos;
	bool sectionFound = true;
    for(uint i = 0; i < (uint)numberOfSections; i++)
	{
		endPos = this->find(sectionIndicator, endPos + 1);
		if(endPos == -1)
		{
			sectionFound = false;
			break;
		}
	}
	endPos--;

	return this->substr(startPos, sectionFound ? endPos - startPos : -1);
}

unsigned int TelaString::findFirstOf(char c, unsigned int startPos)
{
    if((uint)this->size() < startPos)
	{
		return -1;
	}

    for(unsigned int i = startPos; i < (uint)this->size(); i++)
	{
		if(this->at(i) == c) return i;
	}

	return -1;
}

unsigned int TelaString::findFirstNotOf(char c, unsigned int startPos)
{
    if((uint)this->size() < startPos)
	{
		return -1;
	}

    for(unsigned int i = startPos; i < (uint)this->size(); i++)
	{
		if(this->at(i) != c) return i;
	}

	return -1;
}

unsigned int TelaString::findLastNotOf(char c)
{
	for(unsigned int i = this->size() - 1; i >= 0; i--)
	{
		if(this->at(i) != c) return i;
	}

	return -1;
}

#ifdef OGRE

TelaString::TelaString(Ogre::String input)
{
	operator=(input.c_str());
}

TelaString &TelaString::operator=(Ogre::String input)
{
	return operator=(input.c_str());
}

void TelaString::operator+=(Ogre::String input)
{
	operator+=(input.c_str());
}

void TelaString::operator<<(Ogre::String input)
{
	operator<<(input.c_str());
}

void TelaString::operator>>(Ogre::String input)
{
	operator>>(input.c_str());
}

bool TelaString::operator==(Ogre::String input)
{
	return operator==(input.c_str());
}

TelaString TelaString::operator+(Ogre::String input)
{
	return operator=+(input.c_str());
}

bool TelaString::operator^=(Ogre::String input)
{
	return operator^=(input.c_str());
}

TelaString::operator Ogre::String()
{
	return this->c_str();
}

Ogre::String TelaString::toOgreString()
{
	return this->c_str();
}


#endif
