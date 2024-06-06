#include <iostream>
#include "client.h"

int main()
{
    std::cout << "hya\n";
    Client client = Client();
    client.start_dialog();

    return 0;
}