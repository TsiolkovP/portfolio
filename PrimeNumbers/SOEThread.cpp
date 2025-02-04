#include "SOE.h"

#include <cmath>

#include <thread>
#include <functional>
#include <vector>
#include <cstring>

static long find_count(unsigned char *is_prime, unsigned long &limit)
{
    long counter = 0;

    for (unsigned long i = 2; i <= limit; i++)
    {
        if (!GET_BIT(is_prime, i))
        {
            counter += 1;
        }
    }

    return counter;
}

static unsigned long *make_only_primes_array(unsigned char *is_prime, unsigned long &count_of_primes)
{
    unsigned long *primes;
    primes = (unsigned long *)calloc(count_of_primes, sizeof(unsigned long));
    int counter = 0;

    for (unsigned long i = 2; counter < count_of_primes; i++)
    {
        if (!GET_BIT(is_prime, i))
        {
            primes[counter] = i;
            counter += 1;
        }
    }

    return primes;
}

unsigned char *perfom_soe_threads(unsigned long &limit)
{

    unsigned long end = static_cast<int>(std::sqrt(limit));
    unsigned char *prime_bit_arr;
    unsigned char *prime_bit_arr_final;
    unsigned long *prime_arr;
    unsigned long size_primes = 0;

    prime_bit_arr = perfom_soe(end);
    prime_bit_arr_final = (unsigned char *)calloc(limit / 8 + 1, sizeof(char));
    memcpy(prime_bit_arr_final, prime_bit_arr, end / 8 + 1);

    size_primes = find_count(prime_bit_arr, end);
    prime_arr = make_only_primes_array(prime_bit_arr, size_primes);
    delete[] prime_bit_arr;

    unsigned long NumbersPerThread = static_cast<long>(std::ceil(limit / static_cast<long>(std::thread::hardware_concurrency())));
    NumbersPerThread = std::max(NumbersPerThread, static_cast<unsigned long>(1));
    std::vector<std::thread> threads;

    for (unsigned long first = end + 1; first < limit + 1; first += NumbersPerThread)
    {
        unsigned long last = std::min(first + NumbersPerThread, limit + 1);
        threads.emplace_back(
            [&prime_arr, &prime_bit_arr_final, &size_primes](unsigned long begin, unsigned long end)
            {
                for (int i = 0; i < size_primes; i++)
                {
                    unsigned long first = 0;
                    if (begin % prime_arr[i] == 0)
                    {
                        first = begin;
                    }
                    else if (begin / prime_arr[i] == 0)
                    {
                        first = prime_arr[i] * 2;
                    }
                    else
                    {
                        first = (begin / prime_arr[i] + 1) * prime_arr[i];
                    }
                    for (unsigned long n = first; n < end; n += prime_arr[i])
                    {
                        SET_BIT_ONE(prime_bit_arr_final, n);
                    }
                }
            },
            first, last);
    }
    for (auto &thr : threads)
    {
        thr.join();
    }

    delete[] prime_arr;

    return prime_bit_arr_final;
}