#include "pch.h"
#include "BitArray.h"
#include <iostream>

using namespace winrt;
using namespace Windows::Foundation;

int main()
{
    BitArray mybits = BitArray(9);

    std::cout << mybits << '\n';

    mybits.set(0, true);
    mybits.set(2, true);
    mybits.set(4, true);
    mybits.set(6, true);
    mybits.set(8, true);

    std::cout << mybits << '\n';

    mybits.set(0, false);
    mybits.set(4, false);
    mybits.set(8, false);

    std::cout << mybits << '\n';

    init_apartment();
    Uri uri(L"http://aka.ms/cppwinrt");
    printf("Hello, %ls!\n", uri.AbsoluteUri().c_str());
}
