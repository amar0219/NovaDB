# NovaDB - AI Vector Database in C++

> A lightweight vector database built from scratch in C++ for storing, persisting, and searching high-dimensional AI embeddings.

![C++](https://img.shields.io/badge/C%2B%2B-14-blue)
![Status](https://img.shields.io/badge/Status-In%20Development-orange)
![License](https://img.shields.io/badge/License-MIT-green)

---

## Overview

NovaDB is an educational yet production-inspired vector database implemented entirely in C++. The project demonstrates how modern vector databases used in AI systems store, persist, and retrieve embeddings efficiently.

Unlike traditional databases that search by exact values, NovaDB performs **similarity search** over dense vector embeddings using mathematical distance metrics such as cosine similarity.

The project is built completely from scratch without using external database libraries.

---

## Features

- Offset-based vector storage architecture
- Contiguous embedding storage for cache efficiency
- Binary serialization and persistence
- CRUD operations
- Cosine Similarity
- Dot Product
- Euclidean Distance
- Optimized Top-K retrieval using Min Heap
- Custom storage engine
- Zero-copy embedding access
- Modular database architecture

---

## Project Architecture

```
                    +----------------+
                    |    VectorDB    |
                    +--------+-------+
                             |
          +------------------+------------------+
          |                                     |
          v                                     v
 +-------------------+              +-------------------+
 |  Embedding Pool   |              |    Record Table   |
 +-------------------+              +-------------------+
          |                                     |
          +------------------+------------------+
                             |
                             v
                   +-------------------+
                   |  Storage Engine   |
                   +--------+----------+
                            |
              +-------------+-------------+
              |                           |
              v                           v
      BinarySerializer             FileManager
              |                           |
              +-------------+-------------+
                            |
                            v
                   Binary Database Files
```

---

## Folder Structure

```
NovaDB/
│
├── include/
│   ├── core/
│   ├── storage/
│   └── search/
│
├── src/
│   ├── core/
│   ├── storage/
│   └── search/
│
├── tests/
│
├── data/
│
├── README.md
│
└── LICENSE
```

---

## Core Components

### VectorDB

Main interface of the database.

Responsible for

- Insert
- Update
- Delete
- Retrieve
- Search

---

### EmbeddingPool

Stores all embedding vectors inside one contiguous memory block.

Instead of

```
vector<vector<float>>
```

NovaDB stores

```
vector<float>
```

This significantly improves cache locality and reduces heap fragmentation.

---

### Record Table

Stores metadata for every vector.

```cpp
struct Record
{
    uint64_t id;
    uint64_t embeddingOffset;
    uint32_t dimension;
    uint64_t metadataOffset;
    uint8_t active;
};
```

The actual embedding remains inside the Embedding Pool.

---

### Storage Engine

Responsible for

- Saving database
- Loading database
- Binary persistence

---

### Binary Serializer

Converts C++ objects into binary format and reconstructs them during loading.

---

### Similarity Engine

Supports

- Cosine Similarity
- Dot Product
- Euclidean Distance

---

### Top-K Heap

Maintains only the K highest-scoring vectors during search.

Complexity

```
O(N log K)
```

instead of

```
O(N log N)
```

---

## Search Pipeline

```
Query Embedding

        │

        ▼

Similarity Engine

        │

        ▼

Top-K Heap

        │

        ▼

Most Similar Results
```

---

## Complexity Analysis

| Operation | Complexity |
|-----------|------------|
| Insert | O(d) |
| Update | O(d) |
| Delete | O(1) |
| Lookup | O(1) Average |
| Cosine Similarity | O(d) |
| Dot Product | O(d) |
| Euclidean Distance | O(d) |
| Top-K Search | O(N log K) |

Where

- **N** = Number of vectors
- **d** = Embedding dimension
- **K** = Requested results

---

## Build

Compile using GCC

```bash
g++ -std=c++14 -Iinclude src/**/*.cpp -o novadb
```

Example

```bash
g++ -std=c++14 -Iinclude tests/test_similarity.cpp src/search/Similarity.cpp -o test_similarity
```

---

## Future Work

- Approximate Nearest Neighbor (ANN)
- HNSW Index
- Product Quantization
- SIMD Optimization
- Metadata Filtering
- REST API
- Multithreading
- Benchmark Suite
- Python SDK

---

## Learning Objectives

This project was built to understand the internal architecture of modern vector databases such as

- FAISS
- Milvus
- ChromaDB
- Pinecone
- Qdrant
- Weaviate

while implementing the core components from scratch in C++.

---

## Author

**Amar Barade**

B.Tech CSE, VNIT Nagpur

GitHub: https://github.com/your-github

LinkedIn: https://linkedin.com/in/your-linkedin

---

## License

MIT License