#include "SOE.h"

#include <stdlib.h> // for strtol()
#include <errno.h>  // for errno

#include <chrono>
#include <string.h>
#include <iostream>

static bool get_input_number(char *argv[], int choice, unsigned long &out)
{
    if (strlen(argv[choice]) == 0)
    {
        std::cout << "Error. Length of the string is 0." << std::endl;
        return false;
    }

    char *p;
    errno = 0;
    long arg = strtol(argv[choice], &p, 10);

    if (errno != 0)
    {
        std::cout << "Error. Errno: " << errno << "." << std::endl;
        return false;
    }
    else if (*p != '\0')
    {
        std::cout << "Error. An invalid character was found before the end of the string," << std::endl;
        return false;
    }
    else if (arg <= 1)
    {
        std::cout << "Incorrect number. Example: a; a > 1." << std::endl;
        return false;
    }

    out = arg;

    return true;
}

static void show_result(unsigned char *primes, unsigned long &first_border, unsigned long &second_border)
{
    bool flag = false;
    std::cout << "Prime numbers: " << std::endl;

    for (unsigned long i = first_border; i <= second_border; i++)
    {
        if (!GET_BIT(primes, i))
        {
            flag = true;
            std::cout << i << " ";
        }
    }
    if (!flag)
    {
        std::cout << "weren't found.";
    }

    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    unsigned char *primes = nullptr;
    unsigned char *primesThreads = nullptr;
    unsigned long first_border, second_border = 0;

    if (argc == 1)
    {
        std::cout << "Don't see any parameters." << std::endl;
        return 1;
    }
    else if (argc == 2)
    {
        first_border = 2;

        if (!get_input_number(argv, 1, second_border))
        {
            std::cout << "Error. Parameter is invalid." << std::endl;
            return 1;
        }
    }
    else if (argc == 3)
    {
        if (!get_input_number(argv, 1, first_border))
        {
            std::cout << "Error. First parameter is invalid." << std::endl;
            return 1;
        }
        else if (!get_input_number(argv, 2, second_border))
        {
            std::cout << "Error. Second parameter is invalid." << std::endl;
            return 1;
        }
        else if ((first_border >= second_border))
        {
            std::cout << "Incorrect borders. Example: a b; a, b > 1; a < b." << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "Too many parameters." << std::endl;
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    primes = perfom_soe(second_border);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    start = std::chrono::high_resolution_clock::now();
    primesThreads = perfom_soe_threads(second_border);
    stop = std::chrono::high_resolution_clock::now();
    auto durationThreads = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Algorithm - Sieve of Eratosthenes." << std::endl;
    show_result(primes, first_border, second_border);
    std::cout << "Time: " << double((duration.count())) / 1000000 << std::endl;
    std::cout << "Done. " << std::endl;

    std::cout << "Algorithm - Sieve of Eratosthenes. Threads." << std::endl;
    show_result(primesThreads, first_border, second_border);
    std::cout << "Time: " << double((durationThreads.count())) / 1000000 << std::endl;
    std::cout << "Done. " << std::endl;

    delete[] primes;
    delete[] primesThreads;

    return 0;
}
