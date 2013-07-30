#include <string>
#include <cctype>

// functionals for std::transform with correct signature
static inline char string_toupper_functional(char c)
{
    return std::toupper(c);
} 

static inline char string_tolower_functional(char c)
{
    return std::tolower(c);
} 

static inline void string_upper_inplace(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), string_toupper_functional);
}

static inline void string_lower_inplace(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), string_tolower_functional);
}

static inline std::string string_upper(const std::string &str)
{
    std::string strcopy(str.size(), 0);
    std::transform(str.begin(), str.end(), strcopy.begin(), string_toupper_functional);
    return strcopy;
}

static inline std::string string_lower(const std::string &str)
{
    std::string strcopy(str.size(), 0);
    std::transform(str.begin(), str.end(), strcopy.begin(), string_tolower_functional);
    return strcopy;
}

// <longversion/>

#include <assert.h>

// Test the functions above
int main()
{
    // string-copy functions
    assert( string_upper(" aBc ") == " ABC " );
    assert( string_lower(" AbCdEfG ") == " abcdefg " );

    // in-place functions
    std::string str1 = "  aBc  ";
    std::string str2 = "AbCdEfGh ";

    string_upper_inplace(str1);
    string_lower_inplace(str2);

    assert( str1 == "  ABC  " );
    assert( str2 == "abcdefgh " );

    return 0;
}
