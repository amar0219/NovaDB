#include <iostream>

#include "search/TopKHeap.h"

int main()
{
    novadb::TopKHeap heap(3);

    heap.push({1, 0.81f});
    heap.push({2, 0.93f});
    heap.push({3, 0.77f});
    heap.push({4, 0.99f});
    heap.push({5, 0.89f});

    std::vector<novadb::SearchResult> results =
        heap.getResults();

    for(std::size_t i = 0; i < results.size(); i++)
    {
        std::cout
            << results[i].id
            << " "
            << results[i].score
            << std::endl;
    }

    return 0;
}