#include <iostream>
#include <tuple>

#include "CSVParser.h"

int main() {
	Parser<int, double, std::string> parser("in.csv", 0);

	try {
		parser.CheckFile();

		for (auto& record : parser) {
			std::cout << record << std::endl;
		}
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	return 0;
}
