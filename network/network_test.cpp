#include "network.h"
#include <iostream>
int main(int argc, char* argv[]) {
int port = DEFPORT;
char hostname[] = "127.0.0.1";

if(strcmp(argv[1], "0") == 0) {
  std::cout << "0" << std::endl;
  Server_N ser;
  ser.init(port);
  // 1 <--> 2
  ser.server_accept();
  char S_pack[20] = "hello world\n";
  ser.send(S_pack, 20);
} else {
    Client_N client;
    client.init(hostname, port);
    char S_pack[20] = "hello world\n";
    client.receive(S_pack, 20);
    std::cout << S_pack;
}
  return 0;
}