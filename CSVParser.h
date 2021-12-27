#pragma once
#include <string>
#include <iostream>

constexpr char LINE_DELIMITER = '\n';
constexpr char ELEM_DELIMITER = ',';

template <typename... Args>
class CSVParser{
public:
	class Iterator {
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::tuple<Args...>;

		explicit Iterator(const std::string& file_name, size_t line_number);
		
		bool operator==(const Iterator& other)const;
		bool operator!=(const Iterator& other)const;
		Iterator& operator++();
		
		std::tuple<Args...>& operator*();
		std::tuple<Args...>* operator->();
	
	private:
		std::tuple<Args...> current_record;
		std::ifstream file_stream;
		std::string file_name;
		
		size_t current_line_number;
		bool read_record;
	};
	
	Iterator begin();
	Iterator end();

	explicit CSVParser(const std::string& file_name, size_t skip_lines = 0);

private:
	const std::string& file_name;
	size_t skip_lines;
};
