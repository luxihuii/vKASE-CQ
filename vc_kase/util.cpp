#include "util.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;
void Params::init(char* file_name, int n_, int m_, int number) {
    file_name = (char*)"kase_cq/a.param";
auto start_set = chrono::system_clock::now();
    fstream fp(file_name);
    fp.seekg(0, std::ios::end);    // go to the end  
    int count = fp.tellg();           // report location (this is the length)  
    fp.seekg(0, std::ios::beg);    // go back to the beginning  
    char* param = new char[count];    // allocate memory for a buffer of appropriate dimension  
    fp.read(param, count);      // read the whole file into the buffer  
    fp.close();
    pairing_init_set_buf(this->pairing, param, count);
    delete[] param;
    n = n_;
    m = m_;
    Hash = element_from_hash;
if (number == 0) {
    element_t a;
    element_init_G1(this->g, pairing);
    element_init_Zr(a, pairing);
    element_random(this->g);
    element_random(a);
    gn = new element_t[(n*2)+1];
    element_init_G1(gn[0], pairing);
    for(int i = 1; i<((2*n) +1); i++)
    {
        if(i != n+1){
        mpz_t temp;
        element_t ai;
        mpz_init(temp);
        mpz_init_set_ui(temp, i);
        element_init_Zr(ai,pairing);	
        element_pow_mpz(ai, a, temp);
        element_init_G1(gn[i], pairing);
        element_pow_zn(gn[i], this->g, ai);
        mpz_clear(temp);
        element_clear(ai);
        }
        else{
        element_init_G1(gn[i], pairing);
        }
    }
    element_clear(a);
} else {
    element_init_G1(this->g, pairing);
    gn = new element_t[(n*2)+1];
    element_init_G1(gn[0], pairing);
    for(int i = 1; i<((2*n) +1); i++)
    {
        if(i != n+1){
        element_init_G1(gn[i], pairing);
        }
        else{
        element_init_G1(gn[i], pairing);
        }
    }
}
auto end_set= chrono::system_clock::now();
chrono::duration<double> time_set = end_set - start_set;
cout << "setup generte time " << GREEN << time_set.count() << WHITE << " s"<<endl;
cout << "------------------------------------------" <<endl;
}

size_t Params::to_string(char* buf) {
    if (buf == NULL) return ((2*n) +1) * ELEMENT_SIZE;
    element_snprint(buf, ELEMENT_SIZE, g);
    for(int i = 1; i<((2*n) +1); i++) {
        element_snprint(buf + i * ELEMENT_SIZE, ELEMENT_SIZE, gn[i]);
    }
    return ((2*n) +1) * ELEMENT_SIZE;
}

size_t Params::string_to_param(char* buf) {
    if (buf == NULL) return ((2*n) +1) * ELEMENT_SIZE;
    element_set_str(g, buf, 0);
    for(int i = 1; i<((2*n) +1); i++) {
        element_set_str(gn[i], buf + i * ELEMENT_SIZE, 0);
    }
    return ((2*n) +1) * ELEMENT_SIZE;
}

Params::~Params() {
    for(int i=0; i<2*n+1; i++)
    {
        element_clear(gn[i]);
    }
    delete[] gn;
    element_clear(g);
    pairing_clear(pairing);
}

Cipher::Cipher() {}

void Cipher::init(pairing_t& pairing, int m)
{
	this->m = m;
    element_init_G1(c1, pairing);
    element_init_G1(c2, pairing);
    element_init_G1(c3, pairing);
    element_init_G1(c4, pairing);
    element_init_GT(c5, pairing);
    element_init_G1(sig, pairing);
    element_init_G1(d, pairing);
    element_init_G1(cf, pairing);
	Cw = new element_t[m];
    for (int i = 0; i < m; ++i) {
        element_init_G1(Cw[i], pairing);
    }
}

// 8 + m ELEMENT_SIZE
size_t Cipher::to_string(char* buf) {
    if (buf == NULL) return ((8 + m) * ELEMENT_SIZE);
    element_snprint(buf, ELEMENT_SIZE, c1);
    element_snprint(buf + ELEMENT_SIZE, ELEMENT_SIZE, c2);
    element_snprint(buf + ELEMENT_SIZE * 2, ELEMENT_SIZE, c3);
    element_snprint(buf + ELEMENT_SIZE * 3, ELEMENT_SIZE, c4);
    element_snprint(buf + ELEMENT_SIZE * 4, ELEMENT_SIZE, c5);
    element_snprint(buf + ELEMENT_SIZE * 5, ELEMENT_SIZE, sig);
    element_snprint(buf + ELEMENT_SIZE * 6, ELEMENT_SIZE, d);
    element_snprint(buf + ELEMENT_SIZE * 7, ELEMENT_SIZE, cf);
    for (int i = 0; i < m; ++i) {
        element_snprint(buf + ELEMENT_SIZE * (8 + i), ELEMENT_SIZE, Cw[i]);
    }
    return ((8 + m) * ELEMENT_SIZE);
}

size_t Cipher::string_to_cipher(char* buf) {
    if (buf == NULL) return ((8 + m) * ELEMENT_SIZE);
    element_set_str(c1, buf, 0);
    element_set_str(c2, buf + ELEMENT_SIZE, 0);
    element_set_str(c3, buf + ELEMENT_SIZE * 2, 0);
    element_set_str(c4, buf + ELEMENT_SIZE * 3, 0);
    element_set_str(c5, buf + ELEMENT_SIZE * 4, 0);
    element_set_str(sig, buf + ELEMENT_SIZE * 5, 0);
    element_set_str(d, buf + ELEMENT_SIZE * 6, 0);
    element_set_str(cf, buf + ELEMENT_SIZE * 7, 0);
    for (int i = 0; i < m; ++i) {
        element_set_str(Cw[i], buf + ELEMENT_SIZE * (8 + i), 0);
    }
    return ((8 + m) * ELEMENT_SIZE);
}


Cipher::~Cipher()
{
	if(m != 0)
	{
        element_clear(c1);
        element_clear(c2);
        element_clear(c3);
        element_clear(c4);
        element_clear(c5);
        element_clear(sig);
        element_clear(d);
        element_clear(cf);
        for(int i=0; i<m; i++)
        {
            element_clear(Cw[i]);
        }
        delete[] Cw;
	}
}

Sign::Sign() {}

void Sign::init(pairing_t& pairing, int s)
{
    this->s = s;
    d = new element_t[s];
    cd = new element_t[s];
    for (int i = 0; i < s; ++i) {
        element_init_G1(d[i], pairing);
        element_init_G1(cd[i], pairing);
    }
    element_init_G1(aggsig, pairing);
}

size_t Sign::to_string(char* buf) {
    if (buf == NULL) return ((2 * s + 1) * ELEMENT_SIZE);
    for (int i = 0; i < s; ++i) {
        element_snprint(buf + ELEMENT_SIZE * (2 * i), ELEMENT_SIZE, d[i]);
        element_snprint(buf + ELEMENT_SIZE * (1 + 2 * i), ELEMENT_SIZE, cd[i]);
    }
    element_snprint(buf + (2 * s) * ELEMENT_SIZE, ELEMENT_SIZE, aggsig);
    return ((2 * s + 1) * ELEMENT_SIZE);
}

size_t Sign::string_to_sign(char* buf) {
    if (buf == NULL) return ((2 * s + 1) * ELEMENT_SIZE);
    for (int i = 0; i < s; ++i) {
        element_set_str(d[i], buf + ELEMENT_SIZE * (2 * i), 0);
        element_set_str(cd[i], buf + ELEMENT_SIZE * (1 + 2 * i), 0);
    }
    element_set_str(aggsig, buf + (2 * s) * ELEMENT_SIZE, 0);
    return ((2 * s + 1) * ELEMENT_SIZE);
}

Sign::~Sign()
{
	if(s != 0)
	{
        for(int i=0; i<s; i++)
        {
            element_clear(d[i]);
            element_clear(cd[i]);
        }
        delete[] d;
        delete[] cd;
        element_clear(aggsig);
	}
}

Trapdoor::Trapdoor() {}

void Trapdoor::init(pairing_t& pairing)
{
    element_init_G1(t1, pairing);
    element_init_G1(t2, pairing);
    element_init_G1(t3, pairing);
    element_init_G1(t4, pairing);
}

Trapdoor::Trapdoor(Trapdoor& tr) : t5(tr.t5) {
    element_init_same_as(this->t1, tr.t1);
    element_set(this->t1, tr.t1);
    element_init_same_as(this->t2, tr.t2);
    element_set(this->t2, tr.t2);
    element_init_same_as(this->t3, tr.t3);
    element_set(this->t3, tr.t3);
    element_init_same_as(this->t4, tr.t4);
    element_set(this->t4, tr.t4);
}

Trapdoor::~Trapdoor()
{
    element_clear(t1);
    element_clear(t2);
    element_clear(t3);
    element_clear(t4);
}

// 4 * ELEMENT_SIZE + t5.size() * sizeof(size_t)
size_t Trapdoor::to_string(char* buf) {
    if (buf == NULL) return (4 * ELEMENT_SIZE + t5.size() * sizeof(size_t));
    element_snprint(buf, ELEMENT_SIZE, t1);
    element_snprint(buf + ELEMENT_SIZE, ELEMENT_SIZE, t2);
    element_snprint(buf + ELEMENT_SIZE * 2, ELEMENT_SIZE, t3);
    element_snprint(buf + ELEMENT_SIZE * 3, ELEMENT_SIZE, t4);
    size_t tmp[t5.size()];
    for (int i = 0; i < t5.size(); ++i) {
        tmp[i] = t5[i];
    }
    memcpy(buf + 4 * ELEMENT_SIZE, tmp, t5.size() * sizeof(size_t));
    return (4 * ELEMENT_SIZE + t5.size() * sizeof(size_t));
}

size_t Trapdoor::string_to_trapdoor(char* buf, size_t len) {
    if (buf == NULL) return (4 * ELEMENT_SIZE + t5.size() * sizeof(size_t));
    size_t t5_size = (len - 4 * ELEMENT_SIZE) / sizeof(size_t);
    std::cout << t5_size << std::endl;
    element_set_str(t1, buf, 0);
    element_set_str(t2, buf + ELEMENT_SIZE, 0);
    element_set_str(t3, buf + ELEMENT_SIZE * 2, 0);
    element_set_str(t4, buf + ELEMENT_SIZE * 3, 0);
    size_t tmp[t5_size];
    memcpy(tmp, buf + 4 * ELEMENT_SIZE, t5_size * sizeof(size_t));
    t5.resize(t5_size);
    for (int i = 0; i < t5_size; ++i) {
       t5[i] = tmp[i];
    }
    return len;
}

Aggregate::Aggregate() {}

void Aggregate::init(pairing_t& pairing)
{
    element_init_G1(a1, pairing);
    element_init_G1(a2, pairing);
}

Aggregate::~Aggregate()
{
    element_clear(a1);
    element_clear(a2);
}

// 2 * ELEMENT_SIZE
size_t Aggregate::to_string(char* buf) {
    if (buf == NULL) return (2 * ELEMENT_SIZE);
    element_snprint(buf, ELEMENT_SIZE, a1);
    element_snprint(buf + ELEMENT_SIZE, ELEMENT_SIZE, a2);
    return (2 * ELEMENT_SIZE);
}

size_t Aggregate::string_to_aggregate(char* buf) {
    if (buf == NULL) return (2 * ELEMENT_SIZE);
    element_set_str(a1, buf, 0);
    element_set_str(a2, buf + ELEMENT_SIZE, 0);
    return (2 * ELEMENT_SIZE);
}
