#ifndef RECOMMENDATIONSYSTEM_H
#define RECOMMENDATIONSYSTEM_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Movie.h"
#include "User.h"

class RecommendationSystem
{
private:
    struct SpMovieCompare
    {
        bool operator()(const sp_movie& a, const sp_movie& b) const
        {
            return (*a) < (*b);
        }
    };

    std::map<sp_movie, std::vector<double>, SpMovieCompare> _movies;

public:
    RecommendationSystem() = default;

    sp_movie get_movie(const std::string& name, int year) const;
    sp_movie add_movie_to_rs(const std::string& name, int year, const std::vector<double>& features);

    sp_movie recommend_by_content(const User& user) const;
    double predict_movie_score(const User& user, const sp_movie& movie, int k) const;
    sp_movie recommend_by_cf(const User& user, int k) const;

    friend std::ostream& operator<<(std::ostream& os, const RecommendationSystem& rs);
};

#endif // RECOMMENDATIONSYSTEM_H
