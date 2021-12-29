#pragma once

#include <exception>
#include <string>

class BadFormat final : public std::exception
{
public:
	explicit BadFormat(size_t line_number, size_t element_number);
	[[nodiscard]] char const* what() const override;
private:
	std::string msg_;
	size_t line_number_;
	size_t element_number_;
};

class BadLineFormat final :
	public std::exception
{
public:
	explicit BadLineFormat(size_t unread_elements);
	size_t GetElemFromEnd()const;
private:
	size_t unread_elements_;
};

class BadLineQuotes final :
	public std::exception
{
public:
	explicit BadLineQuotes(size_t reversed_element_number);
	size_t GetElemFromEnd()const;
private:
	size_t reversed_element_number_;
};

class BadQuotes final :
	public std::exception
{
public:
	explicit BadQuotes(size_t line_number, size_t element_number);
	[[nodiscard]] char const* what() const override;
private:
	std::string error_message_;
	size_t line_number_;
	size_t element_number_;
};

class ElementWithUnknownSequence final :
	public std::exception
{
public:
	explicit ElementWithUnknownSequence(std::string unknown_word);
	std::string GetWord()const;
private:
	std::string unknown_word_;
};

class RecordWithUnknownSequence final :
	public std::exception
{
public:
	explicit RecordWithUnknownSequence(size_t unread_elements, std::string unknown_word);
	size_t GetElemFromEnd()const;
	std::string GetWord()const;
private:
	size_t unread_elements_;
	std::string unknown_word_;
};

class UnknownRuleSequence final :
	public std::exception
{
public:
	explicit UnknownRuleSequence(size_t error_line, size_t error_element, std::string unknown_word);
	[[nodiscard]] char const* what() const override;

private:
	std::string error_message_;
	size_t error_line_;
	size_t error_element_;
	std::string unknown_word_;
};
