# Freepublicapis
api for freepublicapis.com api for getting free api
# main
```cpp
#include "Freepublicapis.h"
#include <iostream>

int main() {
   Freepublicapis api;
    auto random_api = api.random_api().then([](json::value result) {
        std::cout << result<< std::endl;
    });
    random_api.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```
