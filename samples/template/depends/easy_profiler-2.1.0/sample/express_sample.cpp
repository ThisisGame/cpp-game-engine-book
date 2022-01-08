#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <condition_variable>
#include <cstdlib>
#include <math.h>

#include <easy/profiler.h>
#include <easy/reader.h>

int OBJECTS = 500;

void modellingThread(){
    EASY_THREAD("Modelling");

    static const int N = OBJECTS;

    volatile double *pos[N];
    for (int i = 0; i < N; ++i)
    {
        pos[i] = new volatile double[3];
    }

    {
        EASY_BLOCK("Collisions");
        volatile int i, j;
        volatile double dist;
        for (i = 0; i < N; ++i)
        {
            for (j = i + 1; j < N; ++j)
            {
                EASY_BLOCK("Check");
                volatile double v[3];
                v[0] = pos[i][0] - pos[j][0];
                v[1] = pos[i][1] - pos[j][1];
                v[2] = pos[i][2] - pos[j][2];
                dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
                if (dist < 10000)
                {
                    dist *= dist;
                }
            }
        }
    }

    for (int i = 0; i < N; ++i)
    {
        delete [] pos[i];
    }
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    if (argc > 1 && argv[1]){
        OBJECTS = std::atoi(argv[1]);
    }

    std::cout << "Objects count: " << OBJECTS << std::endl;

    auto start = std::chrono::system_clock::now();


    EASY_PROFILER_ENABLE;
    EASY_MAIN_THREAD;


    modellingThread();

    auto end = std::chrono::system_clock::now();
    auto elapsed =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Elapsed time: " << elapsed.count() << " usec" << std::endl;

    auto blocks_count = profiler::dumpBlocksToFile("test.prof");

    std::cout << "Blocks count: " << blocks_count << std::endl;

    return 0;
}
