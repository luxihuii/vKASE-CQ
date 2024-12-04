#include "User.h"
#include "Server.h"
#include "DataOwner.h"
#include "util.h"
#include <stdlib.h>
using namespace std;
int main(int argc, char **argv) {
    Params param;
    param.init(argv[1], atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    if(strcmp(argv[3], "0") == 0) {
    std::cout << GREEN << "server start " << WHITE<< std::endl;
        Server server(&param);
        server.keygen();
        server.receive_index_from_dow();
        while(1)
        {
            //控制查询
            cout << "search input 1, exit input 0: ";
            bool contro;
            cin >> contro;
            if(! contro) break;
            server.search();
        }
       
    } else if (strcmp(argv[3], "1") == 0) {
        std::cout << GREEN << "DataOwner start " << WHITE<< std::endl;
        DataOwner dow(&param);
        dow.keygen();
        dow.encrypt();
        while(1)
        {
            //控制查询
            cout << "search input 1, exit input 0: ";
            bool contro;
            cin >> contro;
            if(! contro) break;
            dow.extract();
        }
    } else {
        std::cout << GREEN << "User start " << WHITE<< std::endl;
        User user(&param);
        user.keygen();
        while(1)
        {
            //控制查询
            cout << "search input 1, exit input 0: ";
            bool contro;
            cin >> contro;
            if(! contro) break;
            user.gen_trapdoor();
            
            cout << "verify input 1, exit input 0: ";
            bool controv;
            cin >> controv;
            if(! controv) break;
            user.verify();
        }
    }
}
