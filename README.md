# vecdb

A vector database engine built from scratch in C++20, designed around a
contiguous-memory, offset-addressed storage layout (no embeddings inside
records, no per-vector heap allocations, SIMD-friendly scans).

## Layout

```
vectordb/
├── CMakeLists.txt          Top-level build config (C++20, warnings, sanitizers)
├── include/vecdb/          Public headers, one folder per module
│   ├── storage/            Memory mapper, embedding pool, record table, text/metadata stores
│   ├── index/              ISimilarityIndex seam, brute-force, later HNSW
│   ├── query/              Query engine: orchestrates read/write paths
│   ├── similarity/         SIMD distance kernels (AVX2/AVX512)
│   ├── threading/          Thread pool for parallel scans
│   ├── wal/                Write-ahead log + crash recovery
│   ├── config/             Configuration manager
│   └── logging/            Logger
├── src/                    Implementation (.cpp), mirrors include/vecdb/ structure
├── tests/                  GoogleTest unit tests, one file per module
├── benchmarks/             Google Benchmark suite (throughput, latency, recall@k)
├── api/
│   ├── rest/                Thin REST layer over the query engine
│   └── python_sdk/          pybind11 bindings
├── third_party/             Vendored dependencies (if any, beyond FetchContent)
├── cmake/                    Custom CMake modules/helpers
└── data/                     Runtime output: records.bin, embeddings.bin, texts.bin,
                               metadata.bin, wal.log (gitignored)
```

## Design rules this codebase follows

- **No embedded vectors in records.** Records store only `{id, embeddingOffset,
  textOffset, metadataOffset}`. Embeddings live in one contiguous, aligned pool.
- **Dependencies point downward only.** Query Engine → Index Engine → Storage
  Engine → Disk. Storage has zero knowledge of indexing or querying.
- **One virtual seam.** `ISimilarityIndex` is the only vtable boundary in the
  hot path; everything below it (SIMD kernels, memory access) is templates/CRTP.
- **Durability before visibility.** Writes hit the WAL before the data pools,
  and the record table commit is the last step, making it the single
  linearization point for read visibility.
- **Zero-copy reads.** Text/metadata are returned as `std::span<const std::byte>`
  views into mapped memory, never copied into `std::string`.

## Building

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build
```

Options:
- `-DVECDB_BUILD_TESTS=OFF` — skip unit tests (GoogleTest, fetched via CMake)
- `-DVECDB_BUILD_BENCHMARKS=OFF` — skip benchmarks (Google Benchmark, fetched via CMake)
- `-DVECDB_ENABLE_AVX512=ON` — compile AVX-512 kernels (runtime-dispatched, falls
  back to AVX2 if the CPU doesn't support it)

## Status

Project skeleton only. Modules are implemented in this order:

1. Memory Mapper *(next)*
2. Embedding Pool
3. Record Table
4. Text Store + Metadata Store
5. Binary serialization format
6. Similarity Engine (SIMD brute-force)
7. Thread Pool
8. Query Engine
9. WAL + crash recovery
10. Config Manager + Logger
11. HNSW index
12. Benchmark suite
13. REST API
14. Python SDK
