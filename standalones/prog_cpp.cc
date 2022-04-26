#include <iostream>
#include <hybris.h>

int main()
{   
    std::cout << "Hello from cpp" << std::endl;
    hybris::registry_t *reg = hybris::registry_create(10, 20, 20, 100);
    return 0;
}
