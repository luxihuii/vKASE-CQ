LDFLAGS = -L /usr/local/lib
LNK_OPT = -lpbc -lgmp
CPPFLAGS = -I /usr/local/include/pbc -I ./kase_cq -I ./

obj_main = main.o Server.o DataOwner.o util.o User.o network.o
main: $(obj_main)
	g++ -g $(obj_main) $(LDFLAGS) $(LNK_OPT) -o main
%.o:kase_cq/%.cpp
	g++ -g $(CPPFLAGS) -c $< -o $@
%.o:network/%.cpp
	g++ -g $(CPPFLAGS) -c $< -o $@


clean:
	rm -f $(obj_main) main 
