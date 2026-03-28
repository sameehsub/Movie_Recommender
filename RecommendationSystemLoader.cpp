#include "RecommendationSystemLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

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

std::unique_ptr<RecommendationSystem>
RecommendationSystemLoader::create_rs_from_movies(const std::string& movies_file_path)
{
    std::ifstream in(movies_file_path);
    if (!in.is_open())
    {
        throw std::runtime_error("failed to open movies file");
    }

    auto rs = std::make_unique<RecommendationSystem>();

    std::string line;

    if (!std::getline(in, line))
    {
        return rs;
    }

    while (std::getline(in, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::istringstream iss(line);

        std::string movie_token;
        iss >> movie_token;
        if (movie_token.empty())
        {
            continue;
        }

        size_t dash_pos = movie_token.find_last_of('-');
        if (dash_pos == std::string::npos)
        {
            continue;
        }

        std::string name = movie_token.substr(0, dash_pos);
        int year = std::stoi(movie_token.substr(dash_pos + 1));

        std::vector<double> features;
        double x;
        while (iss >> x)
        {
            features.push_back(x);
        }

        validate_features(features);
        rs->add_movie_to_rs(name, year, features);
    }

    return rs;
}
