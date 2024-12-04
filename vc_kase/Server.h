#ifndef __SERVER_H__
#define __SERVER_H__
#include "util.h"
#include <iostream>
#include <vector>
#include "network/network.h"
using namespace std;
class Server {
public:
    Server(Params* param);
    void init();
    ~Server();

    void keygen();
    void send_pks_to_dow();

    void receive_index_from_dow();

    void adjust(Trapdoor& tr_in, vector<size_t>& S, int i, Trapdoor& tr_out);
    bool test(Trapdoor& tr_in, vector<size_t>& S, int i, Cipher& cipher);
    void search();

private:
    Params* sys_param;
    Server_N net_to_dow;
    Server_N net_to_user;
    element_t pks;
    element_t sks;
    Cipher* cipher;
};
#endif
