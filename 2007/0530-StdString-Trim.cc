#include <string>

static inline void string_trim_left_inplace(std::string &str)
{
    str.erase(0, str.find_first_not_of(' '));
}

static inline void string_trim_right_inplace(std::string &str)
{
    str.erase(str.find_last_not_of(' ') + 1, std::string::npos);
}

static inline std::string string_trim_left(const std::string &str)
{
    std::string::size_type pos = str.find_first_not_of(' ');
    if (pos == std::string::npos) return std::string();

    return str.substr(pos, std::string::npos);
}

static inline std::string string_trim_right(const std::string &str)
{
    std::string::size_type pos = str.find_last_not_of(' ');
    if (pos == std::string::npos) return std::string();

    return str.substr(0, pos + 1);
}

static inline std::string string_trim(const std::string& str)
{
    std::string::size_type pos1 = str.find_first_not_of(' ');
    if (pos1 == std::string::npos) return std::string();

    std::string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 == std::string::npos) return std::string();

    return str.substr(pos1 == std::string::npos ? 0 : pos1, 
		      pos2 == std::string::npos ? (str.length() - 1) : (pos2 - pos1 + 1));
}

static inline void string_trim_inplace(std::string& str)
{
    std::string::size_type pos = str.find_last_not_of(' ');
    if(pos != std::string::npos) {
	str.erase(pos + 1);
	pos = str.find_first_not_of(' ');
	if(pos != std::string::npos) str.erase(0, pos);
    }
    else
	str.erase(str.begin(), str.end());
}

// <longversion/>

#include <assert.h>

// Test the functions above
int main()
{
    // string-copy functions
    assert( string_trim_left("  abc  ") == "abc  " );
    assert( string_trim_left("abc  ") == "abc  " );
    assert( string_trim_left("  ") == "" );

    assert( string_trim_right("  abc  ") == "  abc" );
    assert( string_trim_right("  abc") == "  abc" );
    assert( string_trim_right("  ") == "" );

    assert( string_trim("  abc  ") == "abc" );
    assert( string_trim("abc  ") == "abc" );
    assert( string_trim("  abc") == "abc" );
    assert( string_trim("  ") == "" );

    // in-place functions
    std::string str1 = "  abc  ";
    std::string str2 = "abc  ";
    std::string str3 = "  ";

    string_trim_left_inplace(str1);
    string_trim_left_inplace(str2);
    string_trim_left_inplace(str3);

    assert( str1 == "abc  " );
    assert( str2 == "abc  " );
    assert( str3 == "" );

    str1 = "  abc  ";
    str2 = "  abc";
    str3 = "  ";

    string_trim_right_inplace(str1);
    string_trim_right_inplace(str2);
    string_trim_right_inplace(str3);

    assert( str1 == "  abc" );
    assert( str2 == "  abc" );
    assert( str3 == "" );

    str1 = "  abc  ";
    str2 = "  abc";
    str3 = "abc  ";
    std::string str4 = "  ";

    string_trim_inplace(str1);
    string_trim_inplace(str2);
    string_trim_inplace(str3);
    string_trim_inplace(str4);

    assert( str1 == "abc" );
    assert( str2 == "abc" );
    assert( str3 == "abc" );
    assert( str4 == "" );

    return 0;
}
