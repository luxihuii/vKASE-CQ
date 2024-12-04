#ifndef __UTIL_H__
#define __UTIL_H__
#include <pbc.h>
#include <functional>
#include <vector>
#include <string>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define WHITE   "\033[37m"      /* White */

#define ELEMENT_SIZE 512

class Params{
public:    
    Params() {};
    void init(char* file_name, int n_, int m_, int);
    ~Params();
    size_t to_string(char* buf);
    size_t string_to_param(char* buf); 
public:
    pairing_t pairing;
    element_t g;
    element_t* gn;
    std::function<void(element_t, char*, int)> Hash;
    int n;
    int m;
};

class file
{
public:
	int id;
	std::vector<std::string> keyword;
public:
	file(int input, std::vector<std::string> input2):id(input),keyword(input2){};
};

class Cipher {
public:
    Cipher();
    ~Cipher();
    void init(pairing_t& pairing, int m);
    size_t to_string(char* buf);
    size_t string_to_cipher(char* buf);
public:
	element_t c1;
	element_t c2;
	element_t c3;
	element_t c4;
	element_t c5;
	element_t sig;
	element_t d;
	element_t cf;
	element_t* Cw;
    int m;
};

class Sign {
public:
    Sign();
    ~Sign();
    void init(pairing_t& pairing, int s);
    size_t to_string(char* buf);
    size_t string_to_sign(char* buf);
public:
        int s;
        element_t* d;
	element_t* cd;
	element_t aggsig;
};

class Trapdoor {
public:
    Trapdoor();
    Trapdoor(Trapdoor&);
    ~Trapdoor();
    void init(pairing_t& pairing);
    size_t to_string(char* buf);
    size_t string_to_trapdoor(char* buf, size_t);
    element_t t1;
    element_t t2;
    element_t t3;
    element_t t4;
    std::vector<size_t> t5;
};

class Aggregate {
public:
    Aggregate();
    ~Aggregate();
    void init(pairing_t& pairing);
    size_t to_string(char* buf);
    size_t string_to_aggregate(char* buf);
    element_t a1;
    element_t a2;
};
#endif
