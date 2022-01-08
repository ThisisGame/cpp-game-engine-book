//#define FULL_DISABLE_PROFILER
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <condition_variable>
#include <cstdlib>
#include <math.h>

#include <easy/profiler.h>
#include <easy/arbitrary_value.h>
#include <easy/reader.h>

std::condition_variable cv;
std::mutex cv_m;
int g_i = 0;

int OBJECTS = 500;
int MODELLING_STEPS = 1500;
int RENDER_STEPS = 1500;
int RESOURCE_LOADING_COUNT = 50;

//#define SAMPLE_NETWORK_TEST

void localSleep(uint64_t magic=200000)
{
    //PROFILER_BEGIN_FUNCTION_BLOCK_GROUPED(profiler::colors::Blue);
    volatile int i = 0;
    for (; i < magic; ++i);
}

void loadingResources(){
    EASY_FUNCTION(profiler::colors::DarkCyan);
    localSleep();
//    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void prepareMath(){
    EASY_FUNCTION(profiler::colors::Green);
    uint64_t sum = 0;
    int* intarray = new int[OBJECTS];
    for (int i = 0; i < OBJECTS; ++i)
    {
        intarray[i] = i * i;
        sum += i * i;
    }
    delete[] intarray;
    //std::this_thread::sleep_for(std::chrono::milliseconds(3));

    EASY_VALUE("sum", sum, profiler::colors::Blue);
}

void calcIntersect(){
    EASY_FUNCTION(profiler::colors::Gold);
    //int* intarray = new int[OBJECTS * OBJECTS];
    int* intarray = new int[OBJECTS];
    for (int i = 0; i < OBJECTS; ++i)
    {
        for (int j = i; j < OBJECTS; ++j)
            //intarray[i * OBJECTS + j] = i * j - i / 2 + (OBJECTS - j) * 5;
            intarray[j] = i * j - i / 2 + (OBJECTS - j) * 5;
    }
    delete[] intarray;
    //std::this_thread::sleep_for(std::chrono::milliseconds(4));
}

double multModel(double i)
{
    EASY_FUNCTION(profiler::colors::PaleGold);
    return i * sin(i) * cos(i);
}

void calcPhys(){
    EASY_FUNCTION(profiler::colors::Amber);
    double* intarray = new double[OBJECTS];
    for (int i = 0; i < OBJECTS; ++i)
        intarray[i] = multModel(double(i)) + double(i / 3) - double((OBJECTS - i) / 2);
    calcIntersect();
    delete[] intarray;
}

double calcSubbrain(int i)
{
    EASY_FUNCTION(profiler::colors::Navy);
    auto val = i * i * i - i / 10 + (OBJECTS - i) * 7 ;
    EASY_VALUE("subbrainResult", val, profiler::colors::DarkRed);
    return val;
}

void calcBrain(){
    EASY_FUNCTION(profiler::colors::LightBlue);
    double* intarray = new double[OBJECTS];
    for (int i = 0; i < OBJECTS; ++i)
        intarray[i] = calcSubbrain(i) + double(i * 180 / 3);
    delete[] intarray;
    //std::this_thread::sleep_for(std::chrono::milliseconds(3));
}

void calculateBehavior(){
    EASY_FUNCTION(profiler::colors::Blue);
    calcPhys();
    calcBrain();
}

void modellingStep(){
    EASY_FUNCTION();
    prepareMath();
    calculateBehavior();
}

void prepareRender(){
    EASY_FUNCTION(profiler::colors::Brick);
    localSleep();
    //std::this_thread::sleep_for(std::chrono::milliseconds(8));

}

int multPhys(int i)
{
    EASY_FUNCTION(profiler::colors::Red700, profiler::ON);
    return i * i * i * i / 100;
}

int calcPhysicForObject(int i)
{
    EASY_FUNCTION(profiler::colors::Red);
    return  multPhys(i) + i / 3 - (OBJECTS - i) * 15;
}

void calculatePhysics(){
    EASY_FUNCTION(profiler::colors::Red);
    unsigned int* intarray = new unsigned int[OBJECTS];
    for (int i = 0; i < OBJECTS; ++i)
        intarray[i] = calcPhysicForObject(i);
    delete[] intarray;
    //std::this_thread::sleep_for(std::chrono::milliseconds(8));
}

void quadratic_loop(uint64_t n)
{
    EASY_FUNCTION(profiler::colors::Blue);
    EASY_VALUE("N", n);
    volatile int i = 0;
    for (; i < n; ++i)
        localSleep(n);
}

void frame(uint64_t n){
    EASY_FUNCTION(profiler::colors::Magenta);
    prepareRender();
    calculatePhysics();
    quadratic_loop(n);
}

void loadingResourcesThread(){
    //std::unique_lock<std::mutex> lk(cv_m);
    //cv.wait(lk, []{return g_i == 1; });
    EASY_THREAD("Resource loading");
#ifdef SAMPLE_NETWORK_TEST
    while (true) {
#else
    for(int i = 0; i < RESOURCE_LOADING_COUNT; i++){
#endif
        loadingResources();
        EASY_EVENT("Resources Loading!", profiler::colors::Cyan);
        localSleep(1200000);
        //std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void modellingThread(){
    //std::unique_lock<std::mutex> lk(cv_m);
    //cv.wait(lk, []{return g_i == 1; });
    EASY_THREAD("Modelling");
    uint64_t step = 0;
#ifdef SAMPLE_NETWORK_TEST
    while (true) {
#else
    for (int i = 0; i < MODELLING_STEPS; i++){
#endif
        EASY_END_BLOCK;
        EASY_NONSCOPED_BLOCK("Frame", true, 15., profiler::ON, -5.f, profiler::colors::Red);
        modellingStep();

        localSleep(1200000);

        ++step;
        double vals[] = {(double)step, sin((double)step), cos((double)step)};
        EASY_VALUE("step", vals, profiler::colors::Gold, EASY_VIN(step));
        if (step > 10000000)
            step = 0;

        EASY_TEXT("Test String", "Some short text. Hey!", profiler::colors::Red);
        //std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    EASY_END_BLOCK;
}

void renderThread(){
    //std::unique_lock<std::mutex> lk(cv_m);
    //cv.wait(lk, []{return g_i == 1; });
    EASY_THREAD("Render");
    uint64_t n = 20;
#ifdef SAMPLE_NETWORK_TEST
    while (true) {
#else
    for (int i = 0; i < RENDER_STEPS; i++){
#endif
        frame(n);
        localSleep(1200000);
        n += 20;
        if (n >= 700)
            n = 20;
        //std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    if (argc > 1 && argv[1]){
        OBJECTS = std::atoi(argv[1]);
    }
    if (argc > 2 && argv[2]){
        MODELLING_STEPS = std::atoi(argv[2]);
    }
    if (argc > 3 && argv[3]){
        RENDER_STEPS = std::atoi(argv[3]);
    }
    if (argc > 4 && argv[4]){
        RESOURCE_LOADING_COUNT = std::atoi(argv[4]);
    }

    std::cout << "Objects count: " << OBJECTS << std::endl;
    std::cout << "Render steps: " << MODELLING_STEPS << std::endl;
    std::cout << "Modelling steps: " << RENDER_STEPS << std::endl;
    std::cout << "Resource loading count: " << RESOURCE_LOADING_COUNT << std::endl;

    auto start = std::chrono::system_clock::now();

#ifndef SAMPLE_NETWORK_TEST
    EASY_PROFILER_ENABLE;
#endif

    EASY_MAIN_THREAD;
    profiler::startListen();

#ifdef EASY_CONSTEXPR_AVAILABLE
    constexpr int grrr[] {2, -3, 4};
    auto pppp = &grrr;
    EASY_ARRAY("threads count", grrr, 3, false, true, "blabla", profiler::colors::Blue/*, EASY_VIN("threads count")*/, profiler::OFF);
#endif

    int* intPtr = new int(2);
    EASY_VALUE("count", *intPtr);

    std::vector<std::thread> threads;
    //for (int i=0; i < 3; i++)
    {
        threads.emplace_back(loadingResourcesThread);
        threads.emplace_back(renderThread);
        threads.emplace_back(modellingThread);
    }

    cv_m.lock();
    g_i = 1;
    cv_m.unlock();
    cv.notify_all();

#ifndef SAMPLE_NETWORK_TEST
    std::atomic_bool stop;
    stop = false;
    auto frame_time_printer_thread = std::thread([&stop]()
    {
        while (!stop.load(std::memory_order_acquire))
        {
            std::cout << "Frame time: max " << profiler::main_thread::frameTimeLocalMax() << " us // avg " << profiler::main_thread::frameTimeLocalAvg() << " us\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
#endif

    modellingThread();

#ifndef SAMPLE_NETWORK_TEST
    stop.store(true, std::memory_order_release);
    frame_time_printer_thread.join();
#endif

    for(auto& t : threads)
        t.join();

    auto end = std::chrono::system_clock::now();
    auto elapsed =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Elapsed time: " << elapsed.count() << " usec" << std::endl;

    auto blocks_count = profiler::dumpBlocksToFile("test.prof");

    std::cout << "Blocks count: " << blocks_count << std::endl;

    return 0;
}
