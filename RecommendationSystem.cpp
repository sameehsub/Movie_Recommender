#include "RecommendationSystem.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>

/* ===========================
 *   Helper math functions
 * =========================== */

static double dot_product(const std::vector<double>& a,
                          const std::vector<double>& b)
{
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i)
    {
        sum += a[i] * b[i];
    }
    return sum;
}

static double vector_norm(const std::vector<double>& v)
{
    return std::sqrt(dot_product(v, v));
}

static double cosine_similarity(const std::vector<double>& a,
                                const std::vector<double>& b)
{
    double denom = vector_norm(a) * vector_norm(b);
    if (denom == 0.0)
    {
        return 0.0;
    }
    return dot_product(a, b) / denom;
}

static void validate_features(const std::vector<double>& features)
{
    for (double x : features)
    {
        if (x < 1.0 || x > 10.0)
        {
            throw std::invalid_argument("feature values must be in [1,10]");
        }
    }
}

/* ===========================
 *   RecommendationSystem
 * =========================== */

sp_movie RecommendationSystem::get_movie(const std::string& name, int year) const
{

    sp_movie key = std::make_shared<Movie>(name, year);

    auto it = _movies.find(key);
    if (it == _movies.end())
    {
        return nullptr;
    }
    return it->first;
}

sp_movie RecommendationSystem::add_movie_to_rs(const std::string& name,
                                               int year,
                                               const std::vector<double>& features)
{
    validate_features(features);

    sp_movie key = std::make_shared<Movie>(name, year);

    auto it = _movies.find(key);
    if (it != _movies.end())
    {

        it->second = features;
        return it->first;
    }

    _movies.insert({key, features});
    return key;
}

sp_movie RecommendationSystem::recommend_by_content(const User& user) const
{
    const rank_map& ranks = user.rank_get();


    double sum = 0.0;
    for (const auto& pair : ranks)
    {
        sum += pair.second;
    }
    double avg = sum / static_cast<double>(ranks.size());


    size_t dim = 0;
    for (const auto& pair : ranks)
    {
        auto it = _movies.find(pair.first);
        if (it != _movies.end())
        {
            dim = it->second.size();
            break;
        }
    }

    std::vector<double> pref(dim, 0.0);
    for (const auto& pair : ranks)
    {
        auto it = _movies.find(pair.first);
        if (it == _movies.end())
        {
            continue;
        }

        double norm_rank = pair.second - avg;
        const std::vector<double>& feat = it->second;

        for (size_t i = 0; i < dim; ++i)
        {
            pref[i] += norm_rank * feat[i];
        }
    }

    sp_movie best_movie = nullptr;
    double best_score = -std::numeric_limits<double>::infinity();

    for (const auto& pair : _movies)
    {
        const sp_movie& movie = pair.first;
        const std::vector<double>& feat = pair.second;


        if (ranks.find(movie) != ranks.end())
        {
            continue;
        }

        double score = cosine_similarity(feat, pref);
        if (score > best_score)
        {
            best_score = score;
            best_movie = movie;
        }
    }

    return best_movie;
}

double RecommendationSystem::predict_movie_score(const User& user,
                                                 const sp_movie& movie,
                                                 int k) const
{
    const rank_map& ranks = user.rank_get();

    auto target_it = _movies.find(movie);
    if (target_it == _movies.end())
    {
        return 0.0;
    }
    const std::vector<double>& target_feat = target_it->second;

    std::vector<std::pair<double, double>> sims;
    sims.reserve(ranks.size());

    for (const auto& pair : ranks)
    {
        const sp_movie& watched = pair.first;
        double rating = pair.second;

        auto it = _movies.find(watched);
        if (it == _movies.end())
        {
            continue;
        }

        const std::vector<double>& watched_feat = it->second;
        double sim = cosine_similarity(target_feat, watched_feat);
        sims.push_back({sim, rating});
    }

    if (sims.empty())
    {
        return 0.0;
    }

    int use_k = std::min(k, static_cast<int>(sims.size()));

    std::partial_sort(
        sims.begin(),
        sims.begin() + use_k,
        sims.end(),
        [](const auto& a, const auto& b)
        {
            return a.first > b.first;
        });

    double numerator = 0.0;
    double denom = 0.0;

    for (int i = 0; i < use_k; ++i)
    {
        numerator += sims[i].first * sims[i].second;
        denom += sims[i].first;
    }

    if (denom == 0.0)
    {
        return 0.0;
    }

    return numerator / denom;
}

sp_movie RecommendationSystem::recommend_by_cf(const User& user, int k) const
{
    const rank_map& ranks = user.rank_get();

    sp_movie best_movie = nullptr;
    double best_score = -std::numeric_limits<double>::infinity();

    for (const auto& pair : _movies)
    {
        const sp_movie& movie = pair.first;

        if (ranks.find(movie) != ranks.end())
        {
            continue;
        }

        double score = predict_movie_score(user, movie, k);
        if (score > best_score)
        {
            best_score = score;
            best_movie = movie;
        }
    }

    return best_movie;
}

std::ostream& operator<<(std::ostream& os, const RecommendationSystem& rs)
{
    for (const auto& pair : rs._movies)
    {
        os << *(pair.first);
    }
    return os;
}
