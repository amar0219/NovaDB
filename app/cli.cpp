#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

#include "../include/core/VectorDB.h"

using namespace std;
using namespace novadb;

void printUsage()
{
    cout << "NovaDB CLI\n\n";

    cout << "Usage:\n";
    cout << "  novadb_cli insert <id> <v1> <v2> ...\n";
    cout << "  novadb_cli search <k> <q1> <q2> ...\n";
    cout << "  novadb_cli get <id>\n";
    cout << "  novadb_cli delete <id>\n";
    cout << "  novadb_cli save\n";
    cout << "  novadb_cli load\n";
}

int main(int argc, char* argv[])
{
    VectorDB db;

    // Load existing database automatically
    db.load();

    if(argc < 2)
    {
        printUsage();
        return 0;
    }

    string command = argv[1];

    //-------------------------------------------------------
    // INSERT
    //-------------------------------------------------------

    if(command == "insert")
    {
        if(argc < 4)
        {
            cout << "{\"error\":\"Invalid arguments\"}" << endl;
            return 1;
        }

        vector<float> embedding;

        for(int i = 2; i < argc; i++)
        {
            embedding.push_back(std::stof(argv[i]));
        }

        uint64_t id = db.insert(embedding);

        db.save();

        cout << "{";
        cout << "\"status\":\"success\",";
        cout << "\"id\":" << id;
        cout << "}" << endl;

        return 0;
    }

    //-------------------------------------------------------
    // SEARCH
    //-------------------------------------------------------

    if(command == "search")
    {
        if(argc < 4)
        {
            cout << "{\"error\":\"Invalid arguments\"}" << endl;
            return 1;
        }

        int k = std::stoi(argv[2]);

        vector<float> query;

        for(int i = 3; i < argc; i++)
        {
            query.push_back(std::stof(argv[i]));
        }

        EmbeddingView view(
            query.data(),
            static_cast<uint32_t>(query.size())
        );

        vector<SearchResult> results =
            db.search(view, k);

        cout << "{\n";
        cout << "  \"results\": [\n";

        for(size_t i = 0; i < results.size(); i++)
        {
            cout << "    {\"id\": "
                 << results[i].id
                 << ", \"score\": "
                 << results[i].score
                 << "}";

            if(i + 1 != results.size())
                cout << ",";

            cout << "\n";
        }

        cout << "  ]\n";
        cout << "}\n";

        return 0;
    }

    //-------------------------------------------------------
    // GET
    //-------------------------------------------------------

    if(command == "get")
    {
        if(argc != 3)
        {
            cout << "{\"error\":\"Invalid arguments\"}" << endl;
            return 1;
        }

        uint64_t id = std::stoull(argv[2]);

        EmbeddingView view = db.getEmbedding(id);

        if(view.data == nullptr)
        {
            cout << "{\"status\":\"not_found\"}" << endl;
            return 0;
        }

        cout << "{\n";
        cout << "  \"id\": " << id << ",\n";
        cout << "  \"embedding\": [";

        for(uint32_t i = 0; i < view.size; i++)
        {
            cout << view.data[i];

            if(i + 1 != view.size)
                cout << ",";
        }

        cout << "]\n";
        cout << "}\n";

        return 0;
    }

    //-------------------------------------------------------
    // DELETE
    //-------------------------------------------------------

    if(command == "delete")
    {
        if(argc != 3)
        {
            cout << "{\"error\":\"Invalid arguments\"}" << endl;
            return 1;
        }

        uint64_t id = std::stoull(argv[2]);

        bool ok = db.erase(id);

        if(ok)
        {
            db.save();
            cout << "{\"status\":\"success\"}" << endl;
        }
        else
        {
            cout << "{\"status\":\"failed\"}" << endl;
        }

        return 0;
    }

    //-------------------------------------------------------
    // SAVE
    //-------------------------------------------------------

    if(command == "save")
    {
        db.save();

        cout << "{\"status\":\"saved\"}" << endl;

        return 0;
    }

    //-------------------------------------------------------
    // LOAD
    //-------------------------------------------------------

    if(command == "load")
    {
        db.load();

        cout << "{\"status\":\"loaded\"}" << endl;

        return 0;
    }

    //-------------------------------------------------------

    cout << "{\"error\":\"Unknown command\"}" << endl;

    return 1;
}