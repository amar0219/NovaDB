
#include "search/HNSWIndex.h"
#include "search/Similarity.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace novadb {

HNSWIndex::HNSWIndex(const Config& config)
    : config_(config), rng_(config.seed)
{
    if (config_.M < 2) {
        throw std::invalid_argument("HNSW M must be >= 2");
    }

    if (config_.efConstruction == 0 ||
        config_.efSearch == 0) {
        throw std::invalid_argument(
            "HNSW ef parameters must be > 0");
    }
}

float HNSWIndex::distance(
    const EmbeddingView& a,
    const EmbeddingView& b)
{
    if (a.data == nullptr || b.data == nullptr ||
        a.size == 0 || b.size == 0) {
        throw std::invalid_argument(
            "HNSW embeddings must be non-empty");
    }

    const float similarity =
        Similarity::cosineSimilarity(a, b);

    if (!std::isfinite(similarity)) {
        throw std::invalid_argument(
            "Cosine similarity is not finite");
    }

    // Smaller distance means more similar vectors.
    return 1.0f - similarity;
}

int HNSWIndex::randomLevel()
{
    // Higher levels become exponentially less likely.
    std::uniform_real_distribution<double> distribution(
        std::numeric_limits<double>::min(), 1.0);

    const double u = distribution(rng_);
    const double multiplier =
        1.0 / std::log(static_cast<double>(config_.M));

    return static_cast<int>(
        -std::log(u) * multiplier);
}

std::vector<uint64_t> HNSWIndex::searchLayer(
    const EmbeddingView& query,
    const std::vector<uint64_t>& entries,
    uint32_t ef,
    int layer,
    const VectorGetter& getVector) const
{
    using Candidate = std::pair<float, uint64_t>;

    // Nearest candidate first.
    std::priority_queue<
        Candidate,
        std::vector<Candidate>,
        std::greater<Candidate>
    > candidates;

    // Farthest retained result first.
    std::priority_queue<Candidate> best;

    std::unordered_set<uint64_t> visited;

    for (uint64_t id : entries) {
        const auto it = nodes_.find(id);

        if (it == nodes_.end() ||
            it->second.level < layer) {
            continue;
        }

        if (!visited.insert(id).second) {
            continue;
        }

        const float d = distance(query, getVector(id));
        candidates.push({d, id});
        best.push({d, id});
    }

    while (!candidates.empty()) {
        const Candidate current = candidates.top();
        candidates.pop();

        // No unexplored candidate can improve our
        // current retained set.
        if (best.size() >= ef &&
            current.first > best.top().first) {
            break;
        }

        const auto nodeIt = nodes_.find(current.second);
        if (nodeIt == nodes_.end() ||
            layer >= static_cast<int>(
                nodeIt->second.neighbors.size())) {
            continue;
        }

        const std::vector<uint64_t>& neighbors =
            nodeIt->second.neighbors[
                static_cast<std::size_t>(layer)];

        for (uint64_t neighborId : neighbors) {
            if (!visited.insert(neighborId).second) {
                continue;
            }

            const auto neighborIt = nodes_.find(neighborId);

            if (neighborIt == nodes_.end() ||
                neighborIt->second.level < layer) {
                continue;
            }

            const float d =
                distance(query, getVector(neighborId));

            if (best.size() < ef ||
                d < best.top().first) {

                candidates.push({d, neighborId});
                best.push({d, neighborId});

                if (best.size() > ef) {
                    best.pop();
                }
            }
        }
    }

    std::vector<Candidate> ordered;

    while (!best.empty()) {
        ordered.push_back(best.top());
        best.pop();
    }

    // Return closest candidates first.
    std::sort(
        ordered.begin(), ordered.end(),
        [](const Candidate& a, const Candidate& b) {
            return a.first < b.first;
        });

    std::vector<uint64_t> result;
    result.reserve(ordered.size());

    for (const Candidate& candidate : ordered) {
        result.push_back(candidate.second);
    }

    return result;
}

void HNSWIndex::connect(
    uint64_t source,
    uint64_t target,
    int layer,
    const VectorGetter& getVector)
{
    if (source == target) {
        return;
    }

    auto sourceIt = nodes_.find(source);
    auto targetIt = nodes_.find(target);

    if (sourceIt == nodes_.end() ||
        targetIt == nodes_.end()) {
        return;
    }

    if (sourceIt->second.level < layer ||
        targetIt->second.level < layer) {
        return;
    }

    std::vector<uint64_t>& sourceNeighbors =
        sourceIt->second.neighbors[
            static_cast<std::size_t>(layer)];

    if (std::find(
            sourceNeighbors.begin(),
            sourceNeighbors.end(),
            target) == sourceNeighbors.end()) {
        sourceNeighbors.push_back(target);
    }

    // Standard HNSW degree limits.
    const std::size_t limit =
        layer == 0
            ? static_cast<std::size_t>(2) * config_.M
            : config_.M;

    if (sourceNeighbors.size() > limit) {
        const EmbeddingView sourceVector =
            getVector(source);

        std::vector<std::pair<float, uint64_t>> ranked;
        ranked.reserve(sourceNeighbors.size());

        for (uint64_t neighborId : sourceNeighbors) {
            ranked.push_back({
                distance(sourceVector, getVector(neighborId)),
                neighborId
            });
        }

        std::sort(
            ranked.begin(), ranked.end(),
            [](const std::pair<float, uint64_t>& a,
               const std::pair<float, uint64_t>& b) {
                return a.first < b.first;
            });

        sourceNeighbors.clear();

        for (std::size_t i = 0; i < limit; ++i) {
            sourceNeighbors.push_back(ranked[i].second);
        }
    }
}

void HNSWIndex::add(
    uint64_t id,
    EmbeddingView embedding,
    const VectorGetter& getVector)
{
    if (embedding.data == nullptr || embedding.size == 0) {
        throw std::invalid_argument(
            "Cannot index an empty embedding");
    }

    if (nodes_.find(id) != nodes_.end()) {
        throw std::invalid_argument(
            "HNSW node ID already exists");
    }

    if (!nodes_.empty()) {
        // Enforce consistent dimensions.
        const EmbeddingView reference =
            getVector(entryPoint_);

        if (reference.size != embedding.size) {
            throw std::invalid_argument(
                "HNSW embedding dimensions do not match");
        }
    }

    const int level = randomLevel();

    if (nodes_.empty()) {
        Node node;
        node.id = id;
        node.level = level;
        node.neighbors.resize(
            static_cast<std::size_t>(level) + 1);

        nodes_.emplace(id, std::move(node));
        entryPoint_ = id;
        maxLevel_ = level;
        return;
    }

    uint64_t ep = entryPoint_;

    // Greedily descend through layers above the new
    // node's maximum level.
    for (int layer = maxLevel_; layer > level; --layer) {
        const std::vector<uint64_t> nearest =
            searchLayer(embedding, {ep}, 1, layer, getVector);

        if (!nearest.empty()) {
            ep = nearest.front();
        }
    }

    Node node;
    node.id = id;
    node.level = level;
    node.neighbors.resize(
        static_cast<std::size_t>(level) + 1);

    nodes_.emplace(id, std::move(node));

    // Connect the new node at every layer it occupies.
    const int lowestLayer = std::min(level, maxLevel_);

    for (int layer = lowestLayer; layer >= 0; --layer) {
        const std::vector<uint64_t> candidates =
            searchLayer(
                embedding,
                {ep},
                config_.efConstruction,
                layer,
                getVector);

        const std::size_t limit =
            layer == 0
                ? static_cast<std::size_t>(2) * config_.M
                : config_.M;

        const std::size_t count =
            std::min(limit, candidates.size());

        for (std::size_t i = 0; i < count; ++i) {
            const uint64_t neighborId = candidates[i];

            // Create reciprocal connections. Each endpoint
            // applies its own degree limit.
            connect(id, neighborId, layer, getVector);
            connect(neighborId, id, layer, getVector);
        }

        if (!candidates.empty()) {
            ep = candidates.front();
        }
    }

    // A node above the previous maximum becomes the
    // new entry point.
    if (level > maxLevel_) {
        entryPoint_ = id;
        maxLevel_ = level;
    }
}

std::vector<SearchResult> HNSWIndex::search(
    const EmbeddingView& query,
    uint32_t k,
    const VectorGetter& getVector) const
{
    std::vector<SearchResult> results;

    if (k == 0 || nodes_.empty()) {
        return results;
    }

    if (query.data == nullptr || query.size == 0) {
        throw std::invalid_argument(
            "Query embedding must be non-empty");
    }

    uint64_t ep = entryPoint_;

    const EmbeddingView reference = getVector(ep);

    if (reference.size != query.size) {
        throw std::invalid_argument(
            "Query embedding dimensions do not match");
    }

    // Navigate upper layers greedily.
    for (int layer = maxLevel_; layer > 0; --layer) {
        const std::vector<uint64_t> nearest =
            searchLayer(query, {ep}, 1, layer, getVector);

        if (!nearest.empty()) {
            ep = nearest.front();
        }
    }

    // Explore the bottom layer more broadly.
    const uint32_t ef = std::max(
        k, config_.efSearch);

    const std::vector<uint64_t> candidates =
        searchLayer(query, {ep}, ef, 0, getVector);

    std::vector<SearchResult> ranked;
    ranked.reserve(candidates.size());

    for (uint64_t id : candidates) {
        const float similarity =
            Similarity::cosineSimilarity(query, getVector(id));

        ranked.push_back(SearchResult(id, similarity));
    }

    std::sort(
        ranked.begin(), ranked.end(),
        [](const SearchResult& a, const SearchResult& b) {
            return a.score > b.score;
        });

    if (ranked.size() > k) {
        ranked.resize(k);
    }

    return ranked;
}

void HNSWIndex::clear()
{
    nodes_.clear();
    entryPoint_ = 0;
    maxLevel_ = -1;
}

bool HNSWIndex::empty() const
{
    return nodes_.empty();
}

std::size_t HNSWIndex::size() const
{
    return nodes_.size();
}

} // namespace novadb
