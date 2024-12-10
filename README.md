# VC-KASE

## Introduction
Verifiable Conjunctive Field Keyword Searchable Encryption with Aggregate Keys for Data Sharing

## Build

### Compile
make main
### Clean
make clean
### Environmental requirement
install the pbc library
## Use
Open three terminals, which represent the server, dataowner, user.
* server: ./main 100 10 0
* dataowner: ./main 100 10 1
* user: ./main 100 10 2

argv[0] denotes test program. argv[1] denotes number of files, argv[2] denotes the number of keyword fields, argv[3] denotes parties, where 0 represents server, 1 represents dataowner, and 2 represents user.

* After running, the result is as follows：

```html
server:
server start
setup generate time 0.226664 s
search input 1, exit input 0:
------------------------------------------
dataowner:
setup generate time 0.00226338 s
DataOwner start
enc generate time 2.98576 s
------------------------------------------
user:
setup generate time 0.00320752 s
User start
search input 1, exit input 0:
```

* At this time, choose to input 1 to search, and 0 to exit the program.

* Input the number of documents shared by the data owner to the user.

```html
dataowner: please input you share file number:  10
```
* Input the keyword field for query.

```html
user: please input you wanted keyword field:  2
```
* Input the search keyword (you can choose a keyword from the plaintext file 10010 (it can be the second field or not))

```html
user: please input Corresponding keyword: jchyyxletc
```
* Continue querying input 1, otherwise input 0. Continuing to query the target document needs to satisfy the conjunctive query.

```html
user: if continue query, input 1, else input 0:  0
```

```html
user: trap generate time 0.00713352 s
```

```html
server: file 2  match	-----> (search result)
search generate time 0.060368 s
```

* At this time, choose to input 1 to verify, and 0 to exit the program.

```html
user: verify input 1, exit input 0:  1
verify is success.
verify generate time 0.00584956 s
```


