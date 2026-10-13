#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <random>
#include <functional>

#include "core/EmbeddingPool.h"
#include "core/EmbeddingView.h"
#include "search/SearchResult.h"

namespace novadb{

    class HNSWIndex
    {
        public:
            using VectorGetter = std::function<EmbeddingView(uint64_t)>;

            struct Config{
                uint32_t M=16; // Number of bi-directional links created for every new element during construction
                uint32_t efConstruction=200; // Size of the dynamic list for the nearest neighbors (used during the construction)
                uint32_t efSearch=50; // Size of the dynamic list for the nearest neighbors (used during the search)
                uint32_t seed=42; // Random seed for reproducibility
            };

            explicit HNSWIndex(const Config& config = Config{});

            void add(uint64_t id,EmbeddingView embedding,const VectorGetter& getVector);

            std::vector<SearchResult> search(const EmbeddingView& query, uint32_t k, const VectorGetter& getVector) const;

            void clear();

            bool empty() const;

            std::size_t size() const;

        private:
            struct Node{
                uint64_t id=0;
                int level=0;

                std::vector<std::vector<uint64_t>> neighbors; // neighbors[level] contains the ids of the neighbors at that level
            };

            Config config_;

            std::unordered_map<uint64_t, Node> nodes_; // Map from id to Node

            uint64_t entryPoint_=0; // The id of the entry point node
            int maxLevel_=-1; // The maximum level of the graph

            std::mt19937 rng_; // Random number generator for level assignment

            int randomLevel();

            static float distance(
                const EmbeddingView& a,
                const EmbeddingView& b
            );

            std::vector<uint64_t> searchLayer(
                const EmbeddingView& query,
                const std::vector<uint64_t>& entries,
                uint32_t ef,
                int layer,
                const VectorGetter& getVector) const;

            void connect(
                uint64_t source,
                uint64_t target,
                int layer,
                const VectorGetter& getVector
            );
             
    };
}