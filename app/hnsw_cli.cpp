#include <algorithm>
#include <chrono>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../include/core/VectorDB.h"
#include "../include/search/HNSWIndex.h"

using namespace std;
using namespace novadb;

int main(int argc, char* argv[])
{
    try {
        VectorDB db;
        db.load();

        if (argc < 2) {
            cout << "{\"error\":\"Missing command\"}" << endl;
            return 1;
        }

        const string command = argv[1];

        if (command == "insert") {
            if (argc < 3) {
                cout << "{\"error\":\"Invalid arguments\"}" << endl;
                return 1;
            }

            vector<float> embedding;
            for (int i = 2; i < argc; ++i) {
                embedding.push_back(stof(argv[i]));
            }

            const uint64_t id = db.insert(embedding);
            if (id == 0) {
                cout << "{\"error\":\"Insertion failed\"}" << endl;
                return 1;
            }

            if (!db.save()) {
                cout << "{\"error\":\"Database save failed\"}" << endl;
                return 1;
            }

            cout << "{\"status\":\"success\",\"id\":" << id << "}" << endl;
            return 0;
        }

        if (command == "search") {
            if (argc < 4) {
                cout << "{\"error\":\"Invalid arguments\"}" << endl;
                return 1;
            }

            const int requestedK = stoi(argv[2]);
            if (requestedK <= 0) {
                cout << "{\"error\":\"k must be positive\"}" << endl;
                return 1;
            }

            vector<float> query;
            for (int i = 3; i < argc; ++i) {
                query.push_back(stof(argv[i]));
            }

            if (query.empty()) {
                cout << "{\"error\":\"Query cannot be empty\"}" << endl;
                return 1;
            }

            using Clock = chrono::steady_clock;

            HNSWIndex::Config config;
            config.M = 16;
            config.efConstruction = 200;
            config.efSearch = 50;
            config.seed = 42;

            const auto buildStart = Clock::now();

            HNSWIndex index(config);
            const auto getVector = [&db](uint64_t id) {
                return db.getEmbedding(id);
            };

            vector<uint64_t> ids;
            ids.reserve(db.getRecords().size());

            for (const auto& entry : db.getRecords()) {
                if (entry.second.active) {
                    ids.push_back(entry.first);
                }
            }

            sort(ids.begin(), ids.end());

            for (const uint64_t id : ids) {
                const EmbeddingView embedding = db.getEmbedding(id);
                if (embedding.data != nullptr && embedding.size > 0) {
                    index.add(id, embedding, getVector);
                }
            }

            const auto buildEnd = Clock::now();

            EmbeddingView queryView(
                query.data(),
                static_cast<uint32_t>(query.size())
            );

            const auto searchStart = Clock::now();
            const auto results = index.search(
                queryView,
                static_cast<uint32_t>(requestedK),
                getVector
            );
            const auto searchEnd = Clock::now();

            const double buildTimeUs =
                chrono::duration<double, micro>(
                    buildEnd - buildStart
                ).count();

            const double searchTimeUs =
                chrono::duration<double, micro>(
                    searchEnd - searchStart
                ).count();

            cout << fixed << setprecision(3);
            cout << "Index build time: " << buildTimeUs << " microseconds\n";
            cout << "HNSW search time: " << searchTimeUs << " microseconds\n";
            cout << "Indexed vectors: " << index.size() << '\n';

            cout << setprecision(6);
            cout << "{\n  \"results\": [\n";

            for (size_t i = 0; i < results.size(); ++i) {
                cout << "    {\"id\": " << results[i].id
                     << ", \"score\": " << results[i].score << "}";

                if (i + 1 < results.size()) {
                    cout << ",";
                }
                cout << "\n";
            }

            cout << "  ]\n}\n";
            return 0;
        }

        if (command == "get") {
            if (argc != 3) {
                cout << "{\"error\":\"Invalid arguments\"}" << endl;
                return 1;
            }

            const uint64_t id = stoull(argv[2]);
            Record record;

            if (!db.get(id, record) || !record.active) {
                cout << "{\"status\":\"not_found\"}" << endl;
                return 0;
            }

            const EmbeddingView embedding = db.getEmbedding(id);
            cout << fixed << setprecision(6);
            cout << "{\"id\":" << id << ",\"embedding\":[";
            for (uint32_t i = 0; i < embedding.size; ++i) {
                if (i > 0) {
                    cout << ",";
                }
                cout << embedding.data[i];
            }
            cout << "]}" << endl;
            return 0;
        }

        if (command == "delete") {
            if (argc != 3) {
                cout << "{\"error\":\"Invalid arguments\"}" << endl;
                return 1;
            }

            const uint64_t id = stoull(argv[2]);
            if (!db.erase(id)) {
                cout << "{\"status\":\"not_found\"}" << endl;
                return 0;
            }

            if (!db.save()) {
                cout << "{\"error\":\"Database save failed\"}" << endl;
                return 1;
            }

            cout << "{\"status\":\"success\"}" << endl;
            return 0;
        }

        if (command == "save") {
            if (!db.save()) {
                cout << "{\"error\":\"Database save failed\"}" << endl;
                return 1;
            }
            cout << "{\"status\":\"success\"}" << endl;
            return 0;
        }

        if (command == "load") {
            if (!db.load()) {
                cout << "{\"error\":\"Database load failed\"}" << endl;
                return 1;
            }
            cout << "{\"status\":\"success\"}" << endl;
            return 0;
        }

        cout << "{\"error\":\"Unknown command\"}" << endl;
        return 1;
    }
    catch (const exception& e) {
        cerr << "NovaDB HNSW error: " << e.what() << '\n';
        cout << "{\"error\":\"Command failed\"}" << endl;
        return 1;
    }
}
