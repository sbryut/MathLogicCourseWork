#pragma comment(lib, "StaticBuddy.lib")
#include "bdd.h"
#include <fstream>
#include <vector>
#include <iostream>

constexpr int N = 9;
constexpr int M = 4;
constexpr int LOG_N = 4;
constexpr int N_VAR = N * M * LOG_N;

std::ofstream out;
std::vector<char> var(N_VAR, 0);

enum DifficultyLevel {
    EASY,
    HARD
};

void fun(char* varset, int size);
void createBDDProperties(bdd p1[N][N], bdd p2[N][N], bdd p3[N][N], bdd p4[N][N]);
void applyConstraints(bdd& task, bdd p1[N][N], bdd p2[N][N], bdd p3[N][N], bdd p4[N][N], DifficultyLevel level, int type);

int main()
{
    DifficultyLevel level = EASY;
    char userInput;
    std::cout << "Select difficulty level (E - easy, H - difficult): " << std::endl;
    std::cin >> userInput;

    if (userInput == 'H') {
        level = HARD;
        std::cout << "Solution for DIFFICULT difficulty level." << std::endl;
    }
    else if (userInput == 'E') {
        level = EASY;
        std::cout << "Solution for EASY difficulty level." << std::endl;
    }
    else {
        std::cout << "Incorrect input\n";
        return 1;
    }

    bdd_init(1000000, 100000);
    bdd_setvarnum(N_VAR);

    bdd p1[N][N], p2[N][N], p3[N][N], p4[N][N];
    createBDDProperties(p1, p2, p3, p4);

    bdd task = bddtrue;
    for (int i = 1; i <= 6; ++i) {
        applyConstraints(task, p1, p2, p3, p4, level, i);
    }

    out.open("out.txt", std::ios::out);
    if (!out.is_open())
    {
        std::cerr << "Cannot open output file!" << std::endl;
        return 1;
    }
    if (task == bddfalse)
    {
        std::cout << "No solutions possible due to constraints\n";
    }
    else
    {
        unsigned satcount = (unsigned)bdd_satcount(task);
        out << satcount << " solutions:\n" << std::endl;
        std::cout << satcount << " solutions\n" << std::endl;
        if (satcount) {
            bdd_allsat(task, fun);
        }
    }
    out.close();
    bdd_done();
    return 0;
}

void createBDDProperties(bdd p1[N][N], bdd p2[N][N], bdd p3[N][N], bdd p4[N][N])
{
    unsigned I = 0;
    for (unsigned i = 0; i < N; i++)
    {
        for (unsigned j = 0; j < N; j++)
        {
            for (unsigned prop = 0; prop < 4; prop++)
            {
                bdd& currentProperty = (prop == 0) ? p1[i][j] : (prop == 1) ? p2[i][j] : (prop == 2) ? p3[i][j] : p4[i][j];
                currentProperty = bddtrue;
                for (unsigned k = 0; k < LOG_N; k++)
                {
                    currentProperty &= ((j >> k) & 1) ? bdd_ithvar(I + prop * LOG_N + k) : bdd_nithvar(I + prop * LOG_N + k);
                }
            }
        }
        I += LOG_N * M;
    }
}

int getLeftNeighbour(unsigned i, DifficultyLevel level)
{
    if (i % 3 == 2)
        return -1;

    if (level == HARD)
    {
        if (i == 0 || i == 1)
            return i + 7;
        return i - 2;
    }
    else
    {
        if (i == 0 || i == 1)
            return -1;
        return i - 2;
    }
}

int getRightNeighbour(unsigned i)
{
    if (i % 3 == 2)
        return -1;
    return i + 1;
}

void applyConstraints(bdd& task, bdd p1[N][N], bdd p2[N][N], bdd p3[N][N], bdd p4[N][N], DifficultyLevel level, int type)
{
    switch (type)
    {
    case 1:
        task &= p1[8][8];
        task &= p2[3][5];
        task &= p2[6][2];
        task &= p3[5][2];

        //task &= p1[4][4];
        task &= p2[8][3];
        task &= p2[2][6];
        task &= p3[2][3];
        task &= p1[3][3];
        task &= p1[1][1];
        break;

    case 2:
        for (unsigned i = 0; i < N; i++)
        {
            task &= !(p1[i][2] ^ p4[i][2]);
            task &= !(p3[i][5] ^ p1[i][1]);
            task &= !(p2[i][5] ^ p3[i][7]);
            task &= !(p3[i][5] ^ p4[i][3]);
            task &= !(p2[i][2] ^ p4[i][4]);
            task &= !(p1[i][0] ^ p4[i][6]);

            task &= !(p1[i][3] ^ p4[i][7]);
            task &= !(p4[i][5] ^ p3[i][1]);

            if (level == HARD)
            {
                task &= !(p3[i][3] ^ p2[i][6]);
                task &= !(p2[i][6] ^ p4[i][8]);
            }
        }
        break;
    case 3:
        for (unsigned i = 0; i < N; i++)
        {
            if (i == 0)
                task &= !p1[i][0];

            int leftNeighbour = getLeftNeighbour(i, level);

            if (leftNeighbour == -1)
                continue;
            else
            {
                task &= !(p2[leftNeighbour][8] ^ p3[i][8]);
                task &= !(p2[leftNeighbour][7] ^ p3[i][0]);
                task &= !(p1[leftNeighbour][3] ^ p3[i][2]);

                //task &= !(p4[leftNeighbour][2] ^ p4[i][5]);
                //task &= !(p3[leftNeighbour][4] ^ p2[i][7]);
            }
        }

        for (unsigned i = 0; i < N; i++)
        {
            if (i == N - 1)
                task &= !p4[i][2];

            int rightNeighbour = getRightNeighbour(i);

            if (rightNeighbour == -1)
                continue;
            else
            {
                task &= !(p1[rightNeighbour][5] ^ p4[i][0]);
                task &= !(p1[rightNeighbour][3] ^ p2[i][6]);
                task &= !(p3[rightNeighbour][1] ^ p3[i][8]);

                //task &= !(p4[rightNeighbour][3] ^ p1[i][5]);
                //task &= !(p2[rightNeighbour][4] ^ p2[i][2]);
            }
        }
        break;
    case 4:
        for (unsigned i = 0; i < N; i++)
        {
            int leftNeighbour = getLeftNeighbour(i, level);
            int rightNeighbour = getRightNeighbour(i);

            if (leftNeighbour != -1 && rightNeighbour != -1)
            {
                task &= !(p1[leftNeighbour][0] ^ p2[i][4]) | !(p1[rightNeighbour][0] ^ p2[i][4]);
                task &= !(p2[leftNeighbour][1] ^ p3[i][7]) | !(p2[rightNeighbour][1] ^ p3[i][7]);
                task &= !(p4[leftNeighbour][8] ^ p1[i][1]) | !(p4[rightNeighbour][8] ^ p1[i][1]);
                task &= !(p1[leftNeighbour][7] ^ p4[i][3]) | !(p1[rightNeighbour][7] ^ p4[i][3]);
                task &= !(p3[leftNeighbour][4] ^ p1[i][7]) | !(p3[rightNeighbour][4] ^ p1[i][7]);

                if (level == HARD)
                {
                    task &= !(p2[leftNeighbour][0] ^ p3[i][1]) | !(p2[rightNeighbour][0] ^ p3[i][1]);
                    task &= !(p3[leftNeighbour][6] ^ p2[i][5]) | !(p3[rightNeighbour][6] ^ p2[i][5]); //доп огр, убрать для получения единств решения
                    task &= !(p1[leftNeighbour][1] ^ p4[i][1]) | !(p1[rightNeighbour][1] ^ p4[i][1]); //аналогично 
                    task &= !(p2[leftNeighbour][0] ^ p1[i][3]) | !(p2[rightNeighbour][0] ^ p1[i][3]); //аналогично
                }
            }
            if (level == HARD)
                continue;
            else
            {
                if (leftNeighbour != -1 && rightNeighbour == -1)
                {
                    task &= !(p1[leftNeighbour][0] ^ p2[i][4]);
                    task &= !(p2[leftNeighbour][1] ^ p3[i][7]);
                    task &= !(p4[leftNeighbour][8] ^ p1[i][1]);
                    task &= !(p1[leftNeighbour][7] ^ p4[i][3]);
                    task &= !(p3[leftNeighbour][4] ^ p1[i][7]);
                }
                else if (rightNeighbour != -1 && leftNeighbour == -1)
                {
                    task &= !(p1[rightNeighbour][0] ^ p2[i][4]);
                    task &= !(p2[rightNeighbour][1] ^ p3[i][7]);
                    task &= !(p4[rightNeighbour][8] ^ p1[i][1]);
                    task &= !(p1[rightNeighbour][7] ^ p4[i][3]);
                    task &= !(p3[rightNeighbour][4] ^ p1[i][7]);
                }
            }
        }
        break;
    case 5:
        for (unsigned j = 0; j < N; j++)
        {
            for (unsigned i = 0; i < N; i++) // перебор всех пар объектов
            {
                for (unsigned k = i + 1; k < N; k++)
                {
                    task &= p1[i][j] >> !p1[k][j]; // >> правосторонняя импликация
                    task &= p2[i][j] >> !p2[k][j];
                    task &= p3[i][j] >> !p3[k][j];
                    task &= p4[i][j] >> !p4[k][j];
                }
            }
        }
        break;
    case 6:
        for (unsigned i = 0; i < N; i++)
        {
            bdd temp1 = bddfalse;
            bdd temp2 = bddfalse;
            bdd temp3 = bddfalse;
            bdd temp4 = bddfalse;

            for (unsigned j = 0; j < N; j++)
            {
                temp1 |= p1[i][j];
                temp2 |= p2[i][j];
                temp3 |= p3[i][j];
                temp4 |= p4[i][j];
            }
            task &= temp1 & temp2 & temp3 & temp4;
        }
        break;
    }
}

void print(void)
{
    for (unsigned i = 0; i < N; i++)
    {
        out << i << ": ";
        std::cout << i << ": ";
        for (unsigned j = 0; j < M; j++)
        {
            int J = i * M * LOG_N + j * LOG_N;
            int num = 0;
            for (unsigned k = 0; k < LOG_N; k++)
                num += (unsigned)(var[J + k] << k);
            out << num << ' ';
            std::cout << num << ' ';
        }
        out << std::endl;
        std::cout << std::endl;
    }
    out << std::endl;
    std::cout << std::endl;
}

void build(char* varset, unsigned n, unsigned I)
{
    if (I == n - 1) { // достигли последней переменной
        if (varset[I] >= 0) {
            var[I] = varset[I];
            print();
            return;
        }
        var[I] = 0; // для переменной нет установленного значения, она может быть равна как 0, так и 1
        print();    // поэтому проверяем оба варианта
        var[I] = 1;
        print();
        return;
    }
    if (varset[I] >= 0) {
        var[I] = varset[I];
        build(varset, n, I + 1);
        return;
    }
    var[I] = 0;
    build(varset, n, I + 1);
    var[I] = 1;
    build(varset, n, I + 1);
}

void fun(char* varset, int size)
{
    build(varset, size, 0);
}