#include "Exceptions.h"
#include <string>

BadFormat::BadFormat(const size_t line_number, const size_t element_number) :
	line_number_(line_number),
	element_number_(element_number)
{
	msg_ += "Wrong csv format, ";
	msg_ += "line: ";
	msg_ += std::to_string(line_number_);
	msg_ += ", element: ";
	msg_ += std::to_string(element_number_);
	msg_ += ".";
}
char const* BadFormat::what() const
{
	return msg_.c_str();
}

BadLineFormat::BadLineFormat(const size_t unread_elements) :
	unread_elements_(unread_elements)
{}

size_t BadLineFormat::GetElemFromEnd() const
{
	return unread_elements_;
}

BadLineQuotes::BadLineQuotes(const size_t reversed_element_number) :
	reversed_element_number_(reversed_element_number)
{}

size_t BadLineQuotes::GetElemFromEnd() const
{
	return reversed_element_number_;
}

BadQuotes::BadQuotes(const size_t line_number, const size_t element_number) :
	error_message_("Error, one of screening symbols does not have pair.\n"),
	line_number_(line_number),
	element_number_(element_number)
{
	error_message_ += "Line: ";
	error_message_ += std::to_string(line_number_);
	error_message_ += " Element: ";
	error_message_ += std::to_string(element_number_);
}

char const* BadQuotes::what() const
{
	return error_message_.c_str();
}

ElementWithUnknownSequence::ElementWithUnknownSequence(std::string unknown_word) :
	unknown_word_(std::move(unknown_word))
{}

std::string ElementWithUnknownSequence::GetWord()const
{
	return unknown_word_;
}

RecordWithUnknownSequence::RecordWithUnknownSequence(const size_t unread_elements,
	std::string unknown_word) :
	unread_elements_(unread_elements),
	unknown_word_(std::move(unknown_word))
{}

size_t RecordWithUnknownSequence::GetElemFromEnd() const
{
	return unread_elements_;
}

std::string RecordWithUnknownSequence::GetWord() const
{
	return unknown_word_;
}

UnknownRuleSequence::UnknownRuleSequence(const size_t error_line,
	const size_t error_element,
	std::string unknown_word) :
	error_line_(error_line),
	error_element_(error_element),
	unknown_word_(std::move(unknown_word))
{
	error_message_ = "Error, unknown rule sequence: " + unknown_word_ + "\n";
	error_message_ += "Line: " + std::to_string(error_line_) + " Element: " + std::to_string(error_element_) + "\n";
}

char const* UnknownRuleSequence::what() const
{
	return error_message_.c_str();
}