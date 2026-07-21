#include <iostream>
#include <vector>
#include <limits>

#include "../include/core/VectorDB.h"

using namespace std;
using namespace novadb;

void printMenu()
{
    cout << "\n=========================================\n";
    cout << "              NovaDB Console\n";
    cout << "=========================================\n";
    cout << "1. Insert Vector\n";
    cout << "2. Update Vector\n";
    cout << "3. Delete Vector\n";
    cout << "4. Get Vector\n";
    cout << "5. Search\n";
    cout << "6. Save Database\n";
    cout << "7. Load Database\n";
    cout << "8. Exit\n";
    cout << "=========================================\n";
    cout << "Enter choice: ";
}

vector<float> readEmbedding()
{
    uint32_t dimension;

    cout << "Enter dimension: ";
    cin >> dimension;

    vector<float> embedding(dimension);

    cout << "Enter " << dimension << " values:\n";

    for(uint32_t i = 0; i < dimension; i++)
    {
        cin >> embedding[i];
    }

    return embedding;
}

int main()
{
    VectorDB db;

    while(true)
    {
        printMenu();

        int choice;
        cin >> choice;

        switch(choice)
        {
            case 1:
            {
                uint64_t id;

                cout << "Enter ID: ";
                cin >> id;

                vector<float> embedding = readEmbedding();

                if(db.insert(id, embedding))
                    cout << "Vector inserted successfully.\n";
                else
                    cout << "Insertion failed.\n";

                break;
            }

            case 2:
            {
                uint64_t id;

                cout << "Enter ID: ";
                cin >> id;

                vector<float> embedding = readEmbedding();

                if(db.update(id, embedding))
                    cout << "Vector updated successfully.\n";
                else
                    cout << "Update failed.\n";

                break;
            }

            case 3:
            {
                uint64_t id;

                cout << "Enter ID: ";
                cin >> id;

                if(db.erase(id))
                    cout << "Vector deleted successfully.\n";
                else
                    cout << "Delete failed.\n";

                break;
            }

            case 4:
            {
                uint64_t id;

                cout << "Enter ID: ";
                cin >> id;

                EmbeddingView view = db.getEmbedding(id);

                if(view.data == nullptr)
                {
                    cout << "Vector not found.\n";
                }
                else
                {
                    cout << "Embedding:\n";

                    for(uint32_t i = 0; i < view.size; i++)
                    {
                        cout << view.data[i] << " ";
                    }

                    cout << endl;
                }

                break;
            }

            case 5:
            {
                vector<float> query = readEmbedding();

                int k;

                cout << "Top K: ";
                cin >> k;

                EmbeddingView queryView(
                    query.data(),
                    static_cast<uint32_t>(query.size())
                );

                vector<SearchResult> results =
                    db.search(queryView, k);

                cout << "\nResults\n";
                cout << "--------------------------\n";

                for(const auto& result : results)
                {
                    cout << "ID    : " << result.id << endl;
                    cout << "Score : " << result.score << endl;
                    cout << endl;
                }

                break;
            }

            case 6:
            {
                db.save();

                cout << "Database saved.\n";

                break;
            }

            case 7:
            {
                db.load();

                cout << "Database loaded.\n";

                break;
            }

            case 8:
            {
                cout << "Thank you for using NovaDB.\n";

                return 0;
            }

            default:
            {
                cout << "Invalid choice.\n";
            }
        }
    }

    return 0;
}