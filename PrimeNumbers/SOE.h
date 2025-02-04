#pragma once

// Получить i-бит в массиве, считая с 0 бита 0 элемента
#define GET_BIT(is_prime, i) ((is_prime[(i) / 8]) & (1 << ((i) - ((i) / 8) * 8)))

// Переключить i-бит на единицу в массиве, считая с 0 бита 0 элемента
#define SET_BIT_ONE(is_prime, i) ((is_prime[(i) / 8]) |= (1 << ((i) - ((i) / 8) * 8)))

// Выполнить алгоритм Решето Эратосфена
unsigned char *perfom_soe(unsigned long &limit);

// Выполнить алгоритм Решето Эратосфена с использованием потоков
unsigned char *perfom_soe_threads(unsigned long &limit);