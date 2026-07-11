#include "storage/StorageEngine.h"

#include "storage/FileManager.h"
#include "storage/BinarySerializer.h"

namespace novadb
{
    StorageEngine::StorageEngine(const std::string& recordsFile, const std::string& embeddingsFile): recordsFile_(recordsFile), embeddingsFile_(embeddingsFile)
    {
    }

    void StorageEngine::save(const std::unordered_map<uint64_t, Record>& records, const std::vector<float>& embeddings)
    {
        FileManager recordFile(recordsFile_);
        FileManager embeddingFile(embeddingsFile_);

        std::vector<uint8_t> recordBuffer = BinarySerializer::serializeRecords(records);
        std::vector<uint8_t> embeddingBuffer = BinarySerializer::serializeEmbeddings(embeddings);

        recordFile.write(recordBuffer);
        embeddingFile.write(embeddingBuffer);
    }

    void StorageEngine::load(std::unordered_map<uint64_t, Record>& records, std::vector<float>& embeddings)
    {
        FileManager recordFile(recordsFile_);
        FileManager embeddingFile(embeddingsFile_);

        if(recordFile.exists())
        {
            std::vector<uint8_t> recordBuffer = recordFile.read();
            
            records = BinarySerializer::deserializeRecords(recordBuffer);
        }

        if(embeddingFile.exists())
        {
            std::vector<uint8_t> embeddingBuffer = embeddingFile.read();

            embeddings = BinarySerializer::deserializeEmbeddings(embeddingBuffer);
        }
    }
}