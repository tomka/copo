#pragma once

#include <string>


namespace cgv {
	namespace utils {
/** allows to split a text into tokens separated by special characters.

	First Sample:
\code	
tokenizer t("a,b,c,de,,ff");
while (!t.at_end()) {
	std::cout << "'" << *t << "'" << std::endl;
	++t;
}
\endcode
	output is 
\verbatim
'a'
'b'
'c'
'de'
'ff'
\endverbatim
	Second Sample:
\code
tokenizer t("a,b,c,de,,ff", ",", false);
while (!t.at_end()) {
	std::cout << "'" << *t << "'" << std::endl;
	++t;
}
\endcode
	output is 
\verbatim
'a'
'b'
'c'
'de'
''
'ff'
\endverbatim
*/
class tokenizer
{
	const std::string text;
	const std::string separators;
	std::size_t start_pos, end_pos;
	bool skip_successive_seps;
	bool is_separator(std::size_t pos) const
	{
		return separators.find_first_of(text[pos]) != std::string::npos;
	}
	void find_end_pos()
	{
		if (at_end()) {
			end_pos = start_pos;
			return;
		}
		end_pos = start_pos;
		while (end_pos < text.size()) {
			if (is_separator(end_pos))
				break;
			++end_pos;
		}
	}
public:
	/// construct 
	tokenizer(const std::string& _text, 
			  const std::string& _separators = ",;",
			  bool _skip_successive_seps = true)
		: text(_text), separators(_separators), skip_successive_seps(_skip_successive_seps)
	{
		restart();
	}
	/// go back to beginning
	void restart()  
	{
		start_pos = 0; 
		find_end_pos();
	}
	/// check if end is reached
	bool at_end()   
	{
		return start_pos >= text.size(); 
	}
	/// return current token
	std::string operator * () 
	{
		if (at_end())
			return std::string();
		return text.substr(start_pos, end_pos-start_pos);
	}
	/// goto next token
	tokenizer& operator ++()
	{
		if (at_end())
			return *this;
		start_pos = end_pos;
		if (start_pos < text.size())
			++start_pos;
		if (skip_successive_seps) {
			while (start_pos < text.size() && is_separator(start_pos))
				++start_pos;
		}
		find_end_pos();
		return *this;
	}
};

	}
}