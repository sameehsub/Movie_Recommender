#include "UsersLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

static void validate_rate_token(const std::string& token)
{

    if (token == "NA")
    {
        return;
    }

    double val = std::stod(token);
    if (val < 1.0 || val > 10.0)
    {
        throw std::invalid_argument("rate must be in [1,10] or NA");
    }
}

static std::pair<std::string, int> parse_movie_token(const std::string& token)
{
    size_t dash_pos = token.find_last_of('-');
    if (dash_pos == std::string::npos)
    {
        return {"", 0};
    }

    std::string name = token.substr(0, dash_pos);
    int year = std::stoi(token.substr(dash_pos + 1));
    return {name, year};
}

std::vector<User>
UsersLoader::create_users(const std::string& users_file_path,
                          std::unique_ptr<RecommendationSystem> rs)
{
    std::ifstream in(users_file_path);
    if (!in.is_open())
    {
        throw std::runtime_error("failed to open users file");
    }


    std::shared_ptr<RecommendationSystem> shared_rs(std::move(rs));

    std::string line;


    if (!std::getline(in, line))
    {
        return {};
    }

    std::istringstream header(line);
    std::vector<sp_movie> movies_in_order;
    std::string movie_token;

    while (header >> movie_token)
    {
        auto [name, year] = parse_movie_token(movie_token);
        sp_movie m = shared_rs->get_movie(name, year);
        movies_in_order.push_back(m);
    }

    std::vector<User> users;


    while (std::getline(in, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::istringstream iss(line);

        std::string username;
        iss >> username;
        if (username.empty())
        {
            continue;
        }

        rank_map ranks(0, sp_movie_hash, sp_movie_equal);
        int rated_count = 0;

        for (size_t i = 0; i < movies_in_order.size(); ++i)
        {
            std::string rate_token;
            if (!(iss >> rate_token))
            {
                break;
            }

            validate_rate_token(rate_token);

            if (rate_token != "NA")
            {
                double rate = std::stod(rate_token);
                sp_movie m = movies_in_order[i];
                if (m != nullptr)
                {
                    ranks[m] = rate;
                    rated_count++;
                }
            }
        }

        // assumption: each user rated at least 1 movie
        // (not throwing here because the PDF assumes valid input)
        users.emplace_back(username, ranks, shared_rs);
    }

    return users;
}
