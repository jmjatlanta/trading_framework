#include <string>

class CSVParser {
public:
	CSVParser(std::string line, std::string seperator) : line(line), seperator(seperator) {
		pos = seperator.length() * -1;
	}

	/****
	 * Go through the line, retrieving the next element
	 */
	std::string retrieveNextElement() {
		std::string retVal = "";

		if (line.empty())
		{
			atEOL = true;
			return retVal;
		}

		if (pos > 0 && line.length() <= (size_t)pos)
		{
			atEOL = true;
			return retVal;
		}

		size_t end = line.find(seperator, pos + seperator.length());

		if (end == std::string::npos) {
			// we're at the end of the line
			retVal = line.substr(pos + seperator.length());
			pos = line.length();
			atEOL = true;
		} else {
			// we're not at the end of the line, another seperator found
			retVal = line.substr(pos + seperator.length(), end - (pos + seperator.length()));
			pos = end;
		}

		return retVal;
	}

	/****
	 * Retrieve the next element
	 * @param pos the position (0 based)
	 * @returns the string at that position
	 */
	std::string elementAt(int pos) {
		this->pos = seperator.length() * -1;
		for(int i = 0; i < pos; i++)
			retrieveNextElement();
		return retrieveNextElement();
	}

	uint16_t numElements()
	{
		this->pos = seperator.length() * -1;
		uint16_t counter = 0;
		atEOL = false;
		while (!atEOL)
		{
			retrieveNextElement();
			++counter;
		}
		return counter;
	}

private:
	std::string line;
	std::string seperator;
	long pos; // the current comma position
	bool atEOL;
};
