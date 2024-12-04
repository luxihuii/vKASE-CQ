#include "User.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;

User::User(Params* param) {
	sys_param = param;
	net_to_dow.init("127.0.0.1", DEFPORT);
	net_to_server.init("127.0.0.1", DEFPORT + 2);
	init();
};

void User::init() {
	size_t buffer_size = sys_param->string_to_param(NULL);
	char buf[buffer_size];
    net_to_server.receive(buf, buffer_size);
    sys_param->string_to_param(buf);
}

User::~User() {
	element_clear(do_pk);
};

void User::keygen() {
	element_init_G1(do_pk, sys_param->pairing);
	receive_pk_from_dow();
}

void User::receive_pk_from_dow() {
	char buf[ELEMENT_SIZE];
	net_to_dow.receive(buf, ELEMENT_SIZE);
	element_set_str(do_pk, buf, 0);
}

void User::gen_trapdoor() {
size_t wanted_size;
net_to_dow.receive(reinterpret_cast<char*>(&wanted_size), sizeof(wanted_size));
size_t wanted[wanted_size];
net_to_dow.receive(reinterpret_cast<char*>(&wanted), wanted_size * sizeof(wanted_size));

Aggregate agg;
agg.init(sys_param->pairing);
size_t buffer_size;
net_to_dow.receive(reinterpret_cast<char*>(&buffer_size), sizeof(size_t));
char buf[buffer_size];
net_to_dow.receive(buf, buffer_size);
agg.string_to_aggregate(buf);

Trapdoor trapdoor;
trapdoor.init(sys_param->pairing);
//input keywords
	vector<string> Q;
	while(1)
	{
		cin.clear();
		cin.ignore();
		string a;
		int a1;
		bool contol;
		cout << "please input you wanted keyword field: ";
		cin >> a1;
		cin.clear();
		cin.ignore();
		cout <<  "please input Corresponding keyword: ";
		cin >> a;
		cin.clear();
		cin.ignore();
		Q.push_back(a);
		trapdoor.t5.push_back(--a1);
		cout << "if continue query, input 1, else input 0: ";
		cin >> contol;
		if(contol == 0) break;
	}

auto start_trap = chrono::system_clock::now();		
	int l = Q.size();		//l是查询的关键词数量
	element_t a, b;	
	element_init_Zr(a, sys_param->pairing);
	element_random(a);
	element_init_Zr(b, sys_param->pairing);
	element_random(b);
	
	element_t p1, hashsum, p2;
	element_init_G1(p1, sys_param->pairing);
	element_init_Zr(hashsum, sys_param->pairing);
	for(int i =0; i<l ;i++)
	{
		element_t hash;
		element_init_Zr(hash, sys_param->pairing);
		int len = Q[i].length();
		char* keyword = new char[len + 1];
		strcpy(keyword, Q[i].c_str());
		element_from_hash(hash, keyword, len);
		delete[] keyword;
		if(i == 0) element_set(hashsum, hash);
		else element_add(hashsum, hashsum, hash);
		element_clear(hash);
	}
	element_mul(hashsum, hashsum, a);
	element_pow_zn(p1, do_pk, hashsum);
	element_init_G1(p2, sys_param->pairing);
	element_pow_zn(p2, sys_param->g, b);
	element_mul(p1, p1, p2);
	element_mul(trapdoor.t1, agg.a1, p1);
	
	element_pow_zn(trapdoor.t2, sys_param->g, a);
	element_pow_zn(trapdoor.t3, agg.a2, a);
	element_pow_zn(trapdoor.t4, agg.a2, b);
	
	element_clear(p1);
	element_clear(hashsum);
	element_clear(p2);
	element_clear(a);
	element_clear(b);
auto end_trap= chrono::system_clock::now();
chrono::duration<double> time_trap = end_trap - start_trap;
cout << "trap generate time " << GREEN << time_trap.count() << WHITE << " s"<<endl;
cout << "------------------------------------------" <<endl;

 	net_to_server.send(reinterpret_cast<char*>(&wanted_size), sizeof(size_t));
	net_to_server.send(reinterpret_cast<char*>(&wanted), wanted_size * sizeof(size_t));
	size_t trapdoor_size = trapdoor.to_string(NULL);
	net_to_server.send(reinterpret_cast<char*>(&trapdoor_size), sizeof(size_t));
	char buf_trap[trapdoor_size];
	trapdoor.to_string(buf_trap);
	net_to_server.send(buf_trap, trapdoor_size);
}

void User::verify() {
      auto start_verify= chrono::system_clock::now();
      int size;
      net_to_server.receive(reinterpret_cast<char*>(&size), sizeof(int));
      if(size != 0){
       Sign sign;
       sign.init(sys_param->pairing, size);
       size_t buffer_size = sign.string_to_sign(NULL);
       char buf[buffer_size];
       net_to_server.receive(buf, buffer_size);
       sign.string_to_sign(buf);
       element_t e1, e2;
       element_init_GT(e1, sys_param->pairing);
       element_init_GT(e2, sys_param->pairing);
       element_pairing(e1, sys_param->g, sign.aggsig);
       element_t hash, temp;
       element_init_G1(hash, sys_param->pairing);
       element_init_G1(temp, sys_param->pairing);
       for(int i = 0; i < size; ++i){
             int slth = 2 * ELEMENT_SIZE;
	     unsigned char* ibuf = (unsigned char*)malloc(slth);
	     element_to_bytes(ibuf, sign.d[i]);
	     element_to_bytes(ibuf + ELEMENT_SIZE, sign.cd[i]);
             element_from_hash(hash, ibuf, slth);
             delete[] ibuf;
             if(i == 0) element_set(temp, hash);
             else element_mul(temp, temp, hash);
       }
       element_pairing(e2, do_pk, temp);
       if(!element_cmp(e1, e2)){
	     cout << RED << "verify is success." << GREEN << endl  << WHITE;
	}
	element_clear(temp);
	element_clear(hash);
	element_clear(e1);
	element_clear(e2);
      }else{
          cout << "result is null." << GREEN <<  endl  << WHITE;
      }
      auto end_verify = chrono::system_clock::now();
chrono::duration<double> time_verify = end_verify - start_verify;
cout << "verify generate time " << GREEN << time_verify.count() << WHITE << " s"<<endl;
cout << "------------------------------------------" <<endl;
}
