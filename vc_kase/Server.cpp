#include "Server.h"
#include <iostream>
#include <chrono>

Server::Server(Params* param) {
	sys_param = param;
	cipher = new Cipher[sys_param->n + 1];
	for (size_t i = 0; i < sys_param->n + 1; ++i) {
		cipher[i].init(sys_param->pairing, sys_param->m);
	}
	net_to_dow.init(DEFPORT + 1);
	net_to_dow.server_accept();
	net_to_user.init(DEFPORT + 2);
	net_to_user.server_accept();
	init();
};

void Server::init() {
	size_t buffer_size = sys_param->to_string(NULL);
	char buf[buffer_size];
	sys_param->to_string(buf);
	net_to_dow.send(buf, buffer_size);
	net_to_user.send(buf, buffer_size);
}

void Server::keygen() {
    element_init_Zr(sks, sys_param->pairing);
    element_random(sks);
    element_init_G1(pks, sys_param->pairing);
    element_pow_zn(pks, sys_param->g, sks);
	this->send_pks_to_dow();
}

void Server::send_pks_to_dow() {
	char buf[ELEMENT_SIZE];
	element_snprint(buf, ELEMENT_SIZE, pks);
	net_to_dow.send(buf, ELEMENT_SIZE);
}

void Server::receive_index_from_dow() {
	size_t buffer_size = (sys_param->n + 1) * cipher[0].string_to_cipher(NULL);;
	char buf[buffer_size];
	size_t offset = 0;
	net_to_dow.receive(buf, buffer_size);
	for (size_t i = 0; i < sys_param->n + 1; ++i) {
		offset = cipher[i].string_to_cipher(buf + i * offset);
	}
}

Server::~Server() {
    element_clear(sks);
    element_clear(pks);
    delete[] cipher;
}

void Server::adjust(Trapdoor& tr_in, vector<size_t>& wanted,
                    int i, Trapdoor& tr_out) {
	element_t temp;
    int w = wanted.size();
	element_init_G1(temp, sys_param->pairing);
	element_set1(temp);
	for(int j=0; j<w; j++)
	{
		if(wanted[j] == wanted[i]) continue;
		int ij = sys_param->n + 1 - wanted[j] + wanted[i];
		element_mul(temp, temp, sys_param->gn[ij]);
	}
	element_mul(tr_out.t1, tr_in.t1, temp);
	element_clear(temp);
}

bool Server::test(Trapdoor& tr_in, vector<size_t>& wanted, int i, Cipher& C) {
    bool res = false;
    element_t veri1;
    element_t veri2;
    int w = wanted.size();
    int l = tr_in.t5.size();
	element_t e1, e2, e3, e4, e5, temp, temp1;
	element_init_GT(e1, sys_param->pairing);
	element_pairing(e1, tr_in.t1, C.c2);
	element_init_G1(temp, sys_param->pairing);
	element_set(temp, sys_param->gn[(sys_param->n + 1)  - wanted[0]]);
	for(int j = 1; j < w; j++)
	{
		int ij = sys_param->n + 1 - wanted[j];
		element_mul(temp, temp, sys_param->gn[ij]);
	}
	element_init_GT(e2, sys_param->pairing);
	element_pairing(e2, temp, C.c4);
	element_init_GT(e3, sys_param->pairing);
	element_pairing(e3, tr_in.t4, C.c1);
	element_mul(e2, e2, e3);
	element_init_GT(veri1, sys_param->pairing);
	element_div(veri1, e1, e2);
	element_init_G1(temp1, sys_param->pairing);
	element_set(temp1, C.Cw[tr_in.t5[0]]);
	for(int j = 1; j < l; j++)
	{
		element_mul(temp1, temp1, C.Cw[tr_in.t5[j]]);
	}
	element_t sks1;
	element_init_Zr(sks1, sys_param->pairing);
	element_invert(sks1, sks);
	element_pow_zn(temp1, temp1, sks1);
	element_init_GT(e4, sys_param->pairing);
	element_pairing(e4, temp1, tr_in.t3);
	element_clear(sks1);
	
	element_t el;
	element_init_Zr(el, sys_param->pairing);
	element_set_si(el, l);
	element_init_GT(e5, sys_param->pairing);
	element_pairing(e5, C.c3, tr_in.t2);
	element_pow_zn(e5, e5, el);
	element_mul(e5, C.c5, e5);
	element_init_GT(veri2, sys_param->pairing);
	element_div(veri2, e4, e5);
	
	element_clear(el);
	element_clear(temp);
	element_clear(temp1);
	element_clear(e1);	
	element_clear(e2);
	element_clear(e3);
	element_clear(e4);
	element_clear(e5);
	if(!element_cmp(veri1, veri2)){
		cout << RED << "file " << wanted[i] << "  match" << endl << WHITE;
		res = true;
       }
		
	element_clear(veri1);
	element_clear(veri2);
	return res;
}

void Server::search() {
size_t w;
net_to_user.receive(reinterpret_cast<char*>(&w), sizeof(size_t));
size_t wanted[w];
vector<size_t> S(w);
net_to_user.receive(reinterpret_cast<char*>(&wanted), w * sizeof(size_t));
for (int i = 0; i < w; ++i) S[i] = wanted[i];

Trapdoor tr;
tr.init(sys_param->pairing);
size_t buffer_size;
net_to_user.receive(reinterpret_cast<char*>(&buffer_size), sizeof(size_t));
char buf[buffer_size];
net_to_user.receive(buf, buffer_size);
tr.string_to_trapdoor(buf, buffer_size);

vector<int> ai;
auto start_search = chrono::system_clock::now();
    for(int i = 0; i < w; i++)
    {
        Trapdoor tr_adjust(tr);
        adjust(tr, S, i, tr_adjust);
        bool res = test(tr_adjust, S, i, cipher[S[i]]);
        if(res){
             ai.push_back(S[i]);
        }
    }
auto end_search = chrono::system_clock::now();
chrono::duration<double> time_search = end_search - start_search;
cout << "search generate time " << GREEN << time_search.count() << WHITE << " s" <<endl;
cout << "------------------------------------------" <<endl;

int size;
if(!ai.empty()){
	size = ai.size();
	net_to_user.send(reinterpret_cast<char*>(&size), sizeof(int));
	
	Sign sign;
	sign.init(sys_param->pairing, size);
	
	for(int i = 0; i < size; ++i){
	    element_set(sign.d[i], cipher[ai[i]].d);
	    element_set(sign.cd[i], cipher[ai[i]].cf);
	    if(i == 0) element_set(sign.aggsig, cipher[ai[i]].sig);
	    else element_mul(sign.aggsig, sign.aggsig, cipher[ai[i]].sig);
	}
	
	size_t buffer_size = sign.to_string(NULL);
	char buf[buffer_size];
	sign.to_string(buf);
	net_to_user.send(buf, buffer_size);
}else{
	size = 0;
	net_to_user.send(reinterpret_cast<char*>(&size), sizeof(int));
}
ai.clear();
ai.shrink_to_fit();
}
