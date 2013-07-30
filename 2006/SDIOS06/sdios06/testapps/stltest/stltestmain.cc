#include <config.h>

#include <stdio.h>

#include "vector"
#include "map"
#include "my_vector"
#include "my_string"
#include "streambuf"
#include "iostream"

#define TESTSTL

class E
{
public:
	E(int v)
	{
		printf("Construct (%p) %d\n", this, v);
		i = v;
	}

	E(const E& other)
	{
		i = other.i;
		printf("CopyConstruct (%p) %d\n", this, i);
	}

	~E()
	{
		printf("Destruct (%p) %d\n", this, i);
	}

	int getval() const
	{
		return i;
	}

private:
	int i;
}; 

std::ostream& operator<< (std::ostream& out, const E& e)
{
	return out << e.getval();
}

typedef E TT;
typedef std::vector<TT> vec;
typedef std::vector<TT> tvec;
typedef std::string str;

void compare(vec& v1, tvec& v2)
{
	vec::iterator i1 = v1.begin();
	tvec::iterator i2 = v2.begin();

	if(v1.size() != v2.size())
		printf("Different vector lengths\n");

	while(i1 != v1.end()) {
		if(i1->getval() != i2->getval()) {
			printf("Difference in vector...\n");
		}
		E e = *i1;
		printf("E: %d\n", e.getval());
		i1++;
		i2++;
	}
}

void print(vec& v)
{	
	for(vec::iterator i = v.begin(); i != v.end(); ++i) {
		std::cout << *i << " ";
	}
	std::cout << "\n";
	std::cout.flush();
}

int main()
{
	vec v1;
	tvec v2;

	std::cout << "Blup\n";
	std::cout.flush();

	compare(v1, v2);

	v1.push_back(2);
	v1.push_back(6);
	v1.push_back(8);
	v1.push_back(254);
	v1.push_back(13);
	v1.push_back(26);
	v2.push_back(2);
	v2.push_back(6);
	v2.push_back(8);
	v2.push_back(254);
	v2.push_back(13);
	v2.push_back(26);

	print(v1);
	
    v1.erase(v1.begin());
	v1.pop_back();
	print(v1);

	v1.erase(v1.begin() + 1);
	v1.erase(v1.end() - 1);
	print(v1);

	compare(v1, v2);

	str s("Hello World");
	printf("Str: '%s'\n", s.c_str());
	
	if(s == "Hello World") {
		printf("equals1ok\n");
	} else if(s == "Hallo World") {
		printf("COMPAREBUG1\n");
	} else if(s == "h") {
		printf("COMPAREBUG2\n");
	}

	str s2("H");
	if(s2 == s)
		printf("COMPAREBUG3\n");
	s2 = s;
	if(s2 == s)
		printf("equals2ok\n");

	str path;
	path += "/blup";
	path += "/";
	path += "var/assign.txt";
	printf("Path: '%s'\n", path.c_str());
	std::cout << "cout << Path: " << path << std::endl;

	std::map<std::string, std::string> somemap;

	somemap["Hallo"] = "Welt";
   	somemap["more"] = "cool";

	printf("Map[Hallo]: %s\n", somemap["Hallo"].c_str());
	printf("Map[iii]: %s\n", somemap["iii"].c_str());
	printf("Map[more]: %s\n", somemap["more"].c_str());

	std::map<std::string, std::string>::iterator i = somemap.find("more");
	if(i == somemap.end()) {
		printf("more not found");
	} else {
		printf("More is %s\n", i->second.c_str());
	}

	return 0;
}
