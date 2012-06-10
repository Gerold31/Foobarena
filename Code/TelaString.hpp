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

#ifndef _TELASTRING_H_
#define _TELASTRING_H_

#include <deque>
#include <cstring>
using namespace std;


class TelaString /// @todo TelaString: Use references instead of copying
{
public:
	TelaString();

	TelaString(int input);
	TelaString(long input);
	TelaString(size_t input);
	TelaString(double input);
	TelaString(char *input);
	TelaString(const char *input);
	TelaString(const char input);
	TelaString(string input);
#ifdef OGRE
	TelaString(Ogre::String input);
#endif
//	TelaString(TelaString &input);

	TelaString &operator=(int input);
	TelaString &operator=(long input);
	TelaString &operator=(size_t input);
	TelaString &operator=(double input);
	TelaString &operator=(char *input);
	TelaString &operator=(const char *input);
	TelaString &operator=(const char input);
	TelaString &operator=(string input);
//	TelaString operator=(TelaString input);
#ifdef OGRE
	TelaString operator=(Ogre::String input);
#endif

	void operator+=(int input);
	void operator+=(long input);
	void operator+=(size_t input);
	void operator+=(double input);
	void operator+=(char *input);
	void operator+=(const char *input);
	void operator+=(const char input);
	void operator+=(string input);
	void operator+=(TelaString input);
#ifdef OGRE
	void operator+=(Ogre::String input);
#endif

	void operator<<(int input);
	void operator<<(size_t input);
	void operator<<(double input);
	void operator<<(char *input);
	void operator<<(const char *input);
	void operator<<(const char input);
	void operator<<(string input);
	void operator<<(TelaString input);
#ifdef OGRE
	void operator<<(Ogre::String input);
#endif

	void operator>>(int &input);
	void operator>>(size_t &input);
	void operator>>(double &input);
	void operator>>(char* &input);
	void operator>>(string &input);
	void operator>>(TelaString &input);
#ifdef OGRE
	void operator>>(Ogre::String input);
#endif

	bool operator==(int input);
	bool operator==(size_t input);
	bool operator==(double input);
	bool operator==(char *input);
	bool operator==(const char *input);
	bool operator==(const char input);
	bool operator==(string input);
	bool operator==(TelaString input);
#ifdef OGRE
	bool operator==(Ogre::String input);
#endif

	bool operator!=(int input);
	bool operator!=(size_t input);
	bool operator!=(double input);
	bool operator!=(char *input);
	bool operator!=(const char *input);
	bool operator!=(const char input);
	bool operator!=(string input);
	bool operator!=(TelaString input);
#ifdef OGRE
	bool operator!=(Ogre::String input);
#endif

	TelaString operator+(int input);
	TelaString operator+(long input);
	TelaString operator+(size_t input);
	TelaString operator+(double input);
	TelaString operator+(char *input);
	TelaString operator+(const char *input);
	TelaString operator+(const char input);
	TelaString operator+(string input);
	TelaString operator+(TelaString input);
#ifdef OGRE
	TelaString operator+=(Ogre::String input);
#endif

	bool operator^=(char *input);
	bool operator^=(const char *input);
	bool operator^=(const char input);
	bool operator^=(string input);
	bool operator^=(TelaString input);
#ifdef OGRE
	bool operator^=(Ogre::String input);
#endif

	operator char *();
	operator const char *();
#ifdef OGRE
	operator Ogre::String();
#endif

	int         toInt();
	double      toDouble();
	char       *toChar();
	string      toString();
	const char *c_str();
#ifdef OGRE
	Ogre::String toOgreString();
#endif


	char		at(unsigned int position);

	void clear();
	int size();

	/**
	 * @brief find a string in this string
	 * @param str string to search for
	 * @param startPos position where to start searching, default is 0
	 * @return returns the position of the string, if not found or invalid input (no string, startPos too big...) it will return -1
	 */
	int find(TelaString str, unsigned int startPos = 0);

	/**
	 * @brief check how many times this string contains a string
	 * @param str string to search for
	 * @param startPos position where to start searching, default is 0
	 * @return returns the number of matches, if not found or invalid input (no string, startPos too big...) it will return -1
	 */
	int contains(TelaString str, unsigned int startPos = 0);

	/**
	 * @brief replace a certain char with an other in the entire string
	 * @param c character to remove
	 * @param d character to insert
	 * @return returns this string
	 */
	TelaString replace(char c, char d);

	/**
	 * @brief removes a certain char in the entire string
	 * @param c character to remove
	 * @return returns this string
	 */
	TelaString remove(char c);

	TelaString substr(unsigned int startPos, unsigned int n = -1);

	TelaString section(char sectionIndicator, int startSection, int numberOfSections = 1);

	unsigned int findFirstOf(char c, unsigned int startPos = 0);
	unsigned int findFirstNotOf(char c, unsigned int startPos = 0);
	unsigned int findLastNotOf(char c);

private:
	deque<char> content;
};

#endif
