
#include <thread>
#include <vector>

#include <print/print.hpp>

int main()
{
    constexpr int thread_count = 42;
    std::vector<std::thread> printing_threads(thread_count);

    for(auto&& thread : printing_threads)
        thread = std::thread{ []{ print("OK"); } };

    for(auto&& thread : printing_threads)
        thread.join();
}
