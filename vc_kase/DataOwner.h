#include "util.h"
#include "network/network.h"
class DataOwner{
public:
    DataOwner(Params* param);
    void init();
    ~DataOwner();

    void keygen();
    void receive_pks_from_server();
    void send_pk_to_user();

    void encrypt();
    void extract();
    
private:
    Params* sys_param;
    Server_N net_to_user;
    Client_N net_to_server;
    element_t pk;
    element_t sk1;
    element_t sk2;
    element_t ser_pk;
};
