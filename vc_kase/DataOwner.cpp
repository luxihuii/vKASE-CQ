#include "DataOwner.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;
DataOwner::DataOwner(Params* param) {
    sys_param = param;
    net_to_user.init(DEFPORT);
    net_to_user.server_accept();
    net_to_server.init("127.0.0.1", DEFPORT + 1);
    init();
};

void DataOwner::init() {
	size_t buffer_size = sys_param->string_to_param(NULL);
	char buf[buffer_size];
    net_to_server.receive(buf, buffer_size);
    sys_param->string_to_param(buf);
}

void DataOwner::keygen() {
  element_init_Zr(sk1, sys_param->pairing);
  element_init_Zr(sk2, sys_param->pairing);
  element_init_G1(pk, sys_param->pairing);
  // pk_s
  element_init_G1(ser_pk, sys_param->pairing);
  
  element_random(sk1);
  element_random(sk2);
  // KeyGen_o
  element_pow_zn(pk, sys_param->g, sk1);
  
  receive_pks_from_server();
  send_pk_to_user();
}

void DataOwner::send_pk_to_user() {
    char buf[ELEMENT_SIZE];
    element_snprint(buf, ELEMENT_SIZE, pk);
    net_to_user.send(buf, ELEMENT_SIZE);
}

void DataOwner::receive_pks_from_server() {
    char buf[ELEMENT_SIZE];
    net_to_server.receive(buf, ELEMENT_SIZE);
    element_set_str(ser_pk, buf, 0);
};

DataOwner::~DataOwner() {
    element_clear(sk1);
    element_clear(sk2);
    element_clear(pk);
    element_clear(ser_pk);
}

void DataOwner::encrypt() {
    Cipher* cipher = new Cipher[sys_param->n + 1];
	for (size_t i = 0; i < sys_param->n + 1; ++i) {
		cipher[i].init(sys_param->pairing, sys_param->m);
	}
    string filename = "kase_cq/character/5W.txt";
    fstream f(filename);
    vector<file> data;
    if(!f)
    {
        cout <<"character txt not exit" << endl;
        return;
    }
    vector<string> input;
    string line;
    while (getline(f, line))
    {
        line.pop_back();
        input.push_back(line);
    }
    f.close();
    for(int i = 0; i<sys_param->n; i++)
    {
        vector<string> v1(input.begin()+(i*sys_param->m), input.begin() + ((i+1)*sys_param->m));	
        file f1(i+1, v1);
        data.push_back(f1);
    }

    char save[10];
    sprintf(save, "%d%d", sys_param->n, sys_param->m);
    ofstream f1(save, ios::trunc | ios::out);
    for(vector<file>::iterator it=data.begin(); it!= data.end(); it++)
    {
        f1 << it->id <<endl;
        for(vector<string>::iterator its = (it->keyword).begin(); its != (it->keyword).end(); its++)
        {
            f1 << *its << endl;
        }
    }
    f1.close();

auto start_enc = chrono::system_clock::now();	
//encryption
  for(int i = 1; i< sys_param->n + 1; i++)
  {
    element_t r, t;
    element_init_Zr(r, sys_param->pairing);
    element_random(r);
    element_init_Zr(t, sys_param->pairing);
    element_random(t);
    element_t yr, yt;
    element_init_Zr(yr, sys_param->pairing);
    element_mul(yr, sk2, r);
    element_init_Zr(yt, sys_param->pairing);
    element_mul(yt, sk2, t);

	element_pow_zn(cipher[i].c1, sys_param->g, r);
	element_pow_zn(cipher[i].c2, sys_param->g, yr);
	element_pow_zn(cipher[i].c3, sys_param->g, yt);
	element_mul(cipher[i].c4, pk, sys_param->gn[i]);
	element_pow_zn(cipher[i].c4, cipher[i].c4, yr);
	element_pairing(cipher[i].c5, sys_param->gn[1], sys_param->gn[sys_param->n]);
	element_pow_zn(cipher[i].c5, cipher[i].c5, yr);
        //CW_i
        string fp = "";
	for(int j=0; j<sys_param->m; j++ )
	{
		string temp = (data[i-1].keyword)[j];
		fp += temp;
		int len = temp.length();
		char* keyword = new char[len + 1];
		strcpy(keyword, temp.c_str());
		element_t hash;
		element_t mul1;

		element_init_Zr(hash, sys_param->pairing);
		sys_param->Hash(hash, keyword, len);
		element_init_Zr(mul1, sys_param->pairing);
		element_mul(mul1, hash, sk1);
		element_mul(mul1, mul1, r);
		element_add(mul1, mul1, t);
		element_pow_zn((cipher[i].Cw)[j], ser_pk, mul1);
		element_clear(hash);
		element_clear(mul1);
		delete[] keyword;
	}
	element_clear(yr);
	element_clear(yt);
	element_clear(r);
	element_clear(t);
	
	element_t id;
	element_init_G1(id, sys_param->pairing);
	element_random(id);
	element_set(cipher[i].d, id);
	element_clear(id);
	
	int fl = fp.length();
	char* fc = new char[fl + 1];
	strcpy(fc, fp.c_str());
	// cf
	sys_param->Hash(cipher[i].cf, fc, fl);
	// sig
	int slth = 2 * ELEMENT_SIZE;
	unsigned char* ibuf = (unsigned char*)malloc(slth);
	element_to_bytes(ibuf, cipher[i].d);
	element_to_bytes(ibuf + ELEMENT_SIZE, cipher[i].cf);
	element_from_hash(cipher[i].sig, ibuf, slth);
	free(ibuf);
	element_pow_zn(cipher[i].sig, cipher[i].sig, sk1);
	delete[] fc;
  }

auto end_enc= chrono::system_clock::now();
chrono::duration<double> time_enc = end_enc - start_enc;
cout << "enc generate time " << GREEN << time_enc.count() << WHITE << " s"<<endl;
cout << "------------------------------------------" <<endl;

    size_t buffer_size = (sys_param->n + 1) *  cipher[0].to_string(NULL);
    char buf[buffer_size];
    size_t offset = 0;
    for (size_t i = 0; i < sys_param->n + 1; ++i) {
        offset = cipher[i].to_string(buf + i * offset);
    }
    net_to_server.send(buf, buffer_size);
}

void DataOwner::extract() {
    size_t wanted_size;
    cout << "please input you share file number: ";
    cin.clear();
    cin.ignore();
    cin >> wanted_size;
    size_t wanted[wanted_size];
    for(size_t i = 0; i<wanted_size; i++)
    {
        wanted[i] = i + 1;
    }
auto start_agg = chrono::system_clock::now();
    Aggregate aggregate;
    aggregate.init(sys_param->pairing);
    for(auto it : wanted)
    {
        element_t temp;
        size_t temp2 = (sys_param->n + 1) - it;
        element_init_G1(temp, sys_param->pairing);
        element_pow_zn(temp, sys_param->gn[temp2], sk1);
        if(it == wanted[0]) element_set(aggregate.a1, temp);
        else element_mul(aggregate.a1, aggregate.a1, temp);
        element_clear(temp);
    }
    element_pow_zn(aggregate.a2, sys_param->g, sk2);
auto end_agg= chrono::system_clock::now();
chrono::duration<double> time_agg = end_agg - start_agg;
cout << "agg generate time " << GREEN << time_agg.count() << WHITE << " s"<<endl;
cout << "------------------------------------------" <<endl;

    net_to_user.send(reinterpret_cast<char*>(&wanted_size), sizeof(size_t));
    net_to_user.send(reinterpret_cast<char*>(&wanted), wanted_size * sizeof(size_t));

    size_t aggregate_size = aggregate.to_string(NULL);
    net_to_user.send(reinterpret_cast<char*>(&aggregate_size), sizeof(size_t));
    char buf_aggr[aggregate_size];
    aggregate.to_string(buf_aggr);
    net_to_user.send(buf_aggr, aggregate_size);
}
