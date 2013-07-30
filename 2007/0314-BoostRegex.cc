#include <iostream>
#include <stdlib.h>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

int main()
{
    // This regex is compiled at start-up and matches YYYY-MM-DD dates. If it
    // contains a syntax error, the program aborts at start-up with an
    // exception.
    static const boost::regex
        date_regex("(199[0-9]|200[0-9])-([1-9]|0[1-9]|1[012])-([1-9]|[0-2][1-9]|3[01])");

    // First example: char* c-style input strings use boost::cmatch results.
    {
        const char *input_cstr = "2007-03-14";
        boost::cmatch char_matches;

        if (boost::regex_match(input_cstr, char_matches, date_regex))
        {
            // Convert the parsed number using boost's lexical_cast library
            int year = boost::lexical_cast<int>( char_matches[1] );
            // Or use the old way: get the std::string object, then it's char*
            int month = atoi( char_matches[2].str().c_str() );

            std::cout << "First example:"
                      << " year " << year
                      << " month " << month
                      << " day " << char_matches[3] << "\n";
        } 
        else
        {
            std::cout << "First example should have matched the regex.\n";
        }
    }

    // Second example: STL strings use boost::smatch results.
    {
        std::string input_stlstr = "2007-03-34";
        boost::smatch str_matches;

        if (boost::regex_match(input_stlstr, str_matches, date_regex))
        {
            std::cout << "Second example shouldn't have matched the regex.\n";
        }
        else
        {
            std::cout << "Second example didn't match the regex. This was intended.\n";
        }
    }

    // Third example: Temporary regex object and no capture results needed.
    {
        if (boost::regex_match("2007", boost::regex("(199[0-9]|200[0-9])")))
        {
            std::cout << "Third example matched the temporary regex object.\n";
        }
        else
        {
            std::cout << "Third example should have matched the regex.\n";
        }
    }

    // Fourth example: regex_match matches the whole string while regex_search
    // matches substrings just like perl.
    {
        std::string input = "Today is 2007-03-14, how are you?";

        if (boost::regex_match(input, date_regex))
        {
            std::cout << "Fourth example (regex_match) shouldn't match.\n";
        }
        else
        {
            std::cout << "As expected, the fourth example (regex_match) didn't match.\n";
        }

        if (boost::regex_search(input, date_regex))
        {
            std::cout << "While the fourth example using regex_search did matched.\n";
        }
        else
        {
            std::cout << "Fourth example using regex_search should have matched the regex.\n";
        }
    }
}
