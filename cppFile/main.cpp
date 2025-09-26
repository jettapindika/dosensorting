#include <iostream>
#include "header.hpp"

using namespace std;

int main()
{
    int choice = displayMenu();
    switch (choice)
    {
    case 1:
        send1MingguChecklist("../cppFile/datadosen.json");
        break;
    case 2:
        send1Hari("../cppFile/datadosen.json");
        break;
    case 3:
        inputAbsen();
        break;
    case 0:
        cout << "Exiting...\n";
        break;
    default:
        cerr << "Invalid choice.\n";
        break;
    }
    return 0;
}
