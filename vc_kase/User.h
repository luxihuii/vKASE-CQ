#include "util.h"
#include "network/network.h"
#include <vector>
class User{
public:
public:
    User(Params* param);
    void init();
    ~User();

    void keygen();
    void receive_pk_from_dow();
    void gen_trapdoor();
    void verify();

private:
    Params* sys_param;
    Client_N net_to_dow;
    Client_N net_to_server;
    element_t do_pk;
};
