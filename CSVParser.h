#pragma once
#include "Exceptions.h"

#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>

constexpr char LINE_DELIMITER    = '\n';
constexpr char ELEMENT_DELIMITER = ',';
constexpr char SCREEN_GUARD      = '"';

const std::unordered_map<std::string, char> RULE_SEQUENCE_MEANING = {
		{"comma", ELEMENT_DELIMITER},
		{"endl", LINE_DELIMITER},
		{"quote", SCREEN_GUARD}
};

inline void RemoveRuleSequences(std::string& line) {
	int last_changed_screen_guard_index = -1;
	
	while (line.find(SCREEN_GUARD, last_changed_screen_guard_index + 1) != std::string::npos) {
		size_t first_quote_occurrence = line.find(SCREEN_GUARD,
			last_changed_screen_guard_index + 1);
		size_t second_quote_occurrence = line.find(SCREEN_GUARD,
			first_quote_occurrence + 1);
		
		std::string word_inside_guards = line.substr(first_quote_occurrence + 1,
			second_quote_occurrence - first_quote_occurrence - 1);
		
		if (second_quote_occurrence == std::string::npos) {
			throw std::runtime_error("Wrong rule sequence usage");
		}
		try {
			line.replace(first_quote_occurrence,
				second_quote_occurrence - first_quote_occurrence + 1,
				std::string{ RULE_SEQUENCE_MEANING.at(word_inside_guards) });
			last_changed_screen_guard_index = first_quote_occurrence;
		}
		catch (std::out_of_range&) {
			throw ElementWithUnknownSequence(word_inside_guards);
		}
	}
}

inline size_t GetLinesAmount(const std::string& file_name) {
	std::ifstream file_stream(file_name);
	std::string line;

	size_t line_counter = 0;
	
	while (std::getline(file_stream, line, LINE_DELIMITER)) {
		++line_counter;
	}
	
	return line_counter;
}

template<typename T, typename... Args>
auto& operator<<(std::basic_ostream<char>& os, const std::tuple<T, Args...>& tuple) {
	auto remove_first = [](T first, Args... args) { return std::make_tuple(args...); };

	return os << std::get<0>(tuple) << "," << std::apply(remove_first, tuple);
}

template<typename T>
auto& operator<<(std::basic_ostream<char>& os, const std::tuple<T>& tuple) {
	return os << std::get<0>(tuple);
}

template<typename T, typename... Args>
auto& operator>>(std::basic_istream<char>& input_stream, std::tuple<T, Args...>& tuple) {
	T first_element;
	std::string string_element;
	
	std::getline(input_stream, string_element, ELEMENT_DELIMITER);
	try {
		RemoveRuleSequences(string_element);
	}
	catch (ElementWithUnknownSequence& e) {
		throw RecordWithUnknownSequence(sizeof...(Args), e.GetWord());
	}
	catch (std::runtime_error&) {
		throw BadLineQuotes(sizeof...(Args));
	}
	
	std::stringstream char_stream(string_element);
	
	char_stream >> first_element;
	if (input_stream.fail()
		|| char_stream.fail() 
		|| !char_stream.eof()) {
		throw BadLineFormat(sizeof...(Args));
	}
	std::tuple<Args...> tail;
	
	input_stream >> tail;
	
	tuple = std::tuple_cat(std::make_tuple(first_element), tail);
	
	return input_stream;
}

template<typename T>
auto& operator>>(std::basic_istream<char>& input_stream, std::tuple<T>& tuple) {
	T element;
	std::string string_element;
	
	std::getline(input_stream, string_element, LINE_DELIMITER);
	if (string_element.find(ELEMENT_DELIMITER) != std::string::npos) {
		throw BadLineFormat(0);
	}
	try {
		RemoveRuleSequences(string_element);
	}
	catch (ElementWithUnknownSequence& e) {
		throw RecordWithUnknownSequence(0, e.GetWord());
	}
	catch (std::runtime_error&) {
		throw BadLineQuotes(0);
	}
	
	std::stringstream char_stream(string_element);
	
	char_stream >> element;
	
	if (input_stream.fail()
		|| char_stream.fail() 
		|| !char_stream.eof()) {
		throw BadLineFormat(0);
	}

	tuple = std::make_tuple(element);
	
	return input_stream;
}

template <typename... Args>
class Parser {
public:
	class Iterator {
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::tuple<Args...>;

		explicit Iterator(const std::string& file_name, size_t line_number);
		
		bool operator==(const Iterator& other)const;
		bool operator!=(const Iterator& other)const;
		Iterator& operator++();
		void ReadRecord();

		std::tuple<Args...>& operator*();
		std::tuple<Args...>* operator->();
	private:
		std::tuple<Args...> current_record;

		std::ifstream file_stream;
		std::string file_name;

		size_t current_line_number;

		bool read_record;
	};

	explicit Parser(std::string file_name, size_t skip_rows = 0);
	void CheckFile() const;

	Iterator begin();
	Iterator end();

private:
	std::string csv_file_name;
	size_t skip_records;
};

template <typename ... Args>
Parser<Args...>::Iterator::Iterator(const std::string& file_name, const size_t line_number)
	: file_stream(file_name),
	  file_name(file_name),
	  current_line_number(line_number),
	  read_record(false) {
	for (int i = 0; i < current_line_number - 1; ++i) {
		std::string line;
		std::getline(file_stream, line, LINE_DELIMITER);
	}
}

template <typename ... Args>
bool Parser<Args...>::Iterator::operator==(const Iterator& other) const {
	return file_name == other.file_name && current_line_number == other.current_line_number;
}

template <typename ... Args>
bool Parser<Args...>::Iterator::operator!=(const Iterator& other) const {
	return !(*this == other);
}

template <typename ... Args>
typename Parser<Args...>::Iterator& Parser<Args...>::Iterator::operator++() {
	ReadRecord();
	
	current_line_number++;
	read_record = false;
	
	return *this;
}

template <typename ... Args>
void Parser<Args...>::Iterator::ReadRecord() {
	if (!read_record) {
		std::string string_record;
		
		std::getline(file_stream, string_record, LINE_DELIMITER);
		
		std::stringstream string_stream(string_record);
		try {
			string_stream >> current_record;
		}
		catch (BadLineFormat& e) {
			throw BadFormat(current_line_number, sizeof...(Args) - e.GetElemFromEnd());
		}
		catch (BadLineQuotes& e) {
			throw BadQuotes(current_line_number, sizeof...(Args) - e.GetElemFromEnd());
		}
		catch (RecordWithUnknownSequence& e) {
			throw UnknownRuleSequence(current_line_number, sizeof...(Args) - e.GetElemFromEnd(), e.GetWord());
		}

		read_record = true;
	}
}

template <typename ... Args>
std::tuple<Args...>& Parser<Args...>::Iterator::operator*() {
	ReadRecord();

	return current_record;
}

template <typename ... Args>
std::tuple<Args...>* Parser<Args...>::Iterator::operator->() {
	ReadRecord();

	return &current_record;
}

template <typename ... Args>
Parser<Args...>::Parser(std::string file_name, const size_t skip_rows) 
	: csv_file_name(std::move(file_name)),
	  skip_records(skip_rows) {}

template <typename ... Args>
void Parser<Args...>::CheckFile() const {
	Parser test_parser(csv_file_name, skip_records);
	
	std::tuple<Args...> buffer;
	for (auto& record : test_parser) {
		buffer = record;
	}
}

template <typename ... Args>
typename Parser<Args...>::Iterator Parser<Args...>::begin() {
	return Iterator(csv_file_name, skip_records + 1);
}

template <typename ... Args>
typename Parser<Args...>::Iterator Parser<Args...>::end() {
	return Iterator(csv_file_name, GetLinesAmount(csv_file_name) + 1);
}
