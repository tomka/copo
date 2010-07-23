#ifndef CGV_DEBUG_DIAGNOSTICS_H
#define CGV_DEBUG_DIAGNOSTICS_H

#include <strstream>
#include <string>

/// macro to avoid the need to specify file name and line number
#define TEST(test_result) test(test_result,__FILE__,__LINE__)

namespace cgv {
	namespace debug {
		/** the diagnostics class is used for testing. 
		    See for example the cgv/storage/view/triangular_view_test.cxx for a sample usage. */
		class diagnostics
		{
		public:
			/// different diagnostic modes
			enum mode_type { PER_CONDITION, PER_TEST, ONCE };
		protected:
			/// in which granularity to show diagnostics
			mode_type mode;
			/// whether to show file name and line number
			bool show_file_info;
			/// accumulate error message here
			std::strstream* error_message;
			/// total number of tests
			unsigned int number_tests;
			/// number of failed tests
			unsigned int number_failed_tests;
			/// total number of tested conditions
			unsigned int number_conditions;
			/// number of failed conditions
			unsigned int number_failes_conditions;
		private:
			/// whether to collect the next error message
			bool collect_error;
			/// the number of errors in the current test
			unsigned int current_number_failed_conditions;
			/// current test name
			std::string current_test_name;
			/// tag used to transfer the test result
			struct test_tag
			{
				test_tag(bool result, const char* file, int line) : test_result(result), file_name(file), line_number(line) {}
				bool test_result;
				const char* file_name;
				int line_number;
			};
			/// tag used to transfer the end marker
			struct end_tag { };
		public:
			///
			diagnostics(mode_type _mode = PER_TEST, bool _show_file_info = true) : 
			  mode(_mode), show_file_info(_show_file_info), number_tests(0), number_failed_tests(0), 
			  number_conditions(0), number_failed_conditions(0), 
			  current_test_name(0), current_number_failed_conditions(0), collect_error(false),
			  error_message(new std::strstream())
			{
			}
			/// start a new test
			void start_test(const std::string& test_name)
			{
				if (mode == PER_CONDITION)
					std::cerr << "start test " << test_name.c_str() << std::endl << "----------------------------------------------" << std::endl;
				current_test_name = test_name;
				current_number_failed_conditions = 0;
			}
			/// finish the current test and return the number of failed conditions
			unsigned int finish_test()
			{
				if (mode == PER_TEST) {
					if (current_number_failed_conditions > 0)
						std::cerr << "test " << current_test_name.c_str() << " failed " << current_number_failed_conditions << " times" << std::endl;
				}
				if (mode == PER_CONDITION) {
					std::cerr << "finished test " << test_name.c_str() << std::endl;
			}
			/// the test function generates a test tag as output, use the macro TEST(test_result) to automatically add file name and line number
			static test_tag test(bool test_result, const char* file_name = 0, int line_number = -1) 
			{ 
				return test_tag(test_result,file_name,line_number); 
			}
			/// the test function generates a test tag as output
			static end_tag end() 
			{
				return end_tag(); 
			}
			/// pass << operator to error_message stream by default
			template <typename ta_type>
			diagnostics& operator << (const ta_type& value)
			{
				if (collect_error)
					(*error_message) << value;
				return *this;
			}
			/// handle tests
			diagnostics& operator << (const test_tag& test)
			{
				collect_error = false;
				if (!test.test_result) {
					++number_errors;
					if (show_each_error) {
						if (show_file_info) {
							if (test.file_name)
								std::cerr << test.file_name;
							if (test.line_number != -1)
								std::cerr << "(" << test.line_number << ") : ";
						}
						std::cerr << "FAILURE: ";
						collect_error = true;
					}
				}
				return *this;
			}
			/// handle end
			diagnostics& operator << (const end_tag&)
			{
				if (collect_error) {
					std::cerr.write(error_message->str(), error_message->pcount());
					std::cerr << std::endl;
					delete error_message;
					error_message = new std::strstream;
				}
				return *this;
			}
			/// show diagnostics
			friend std::ostream& operator << (std::ostream& os, const diagnostics& diag)
			{
				if (diag.number_errors == 0)
					return os << "all tests successful" << std::endl;
				else
					return os << "found a total of " << diag.number_errors << " errors" << std::endl;
			}
		};
	}
}

#endif