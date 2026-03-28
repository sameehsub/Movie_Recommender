#include "User.h"
#include "RecommendationSystem.h"

#include <stdexcept>


User::User(const std::string& name,
           const rank_map& ranks,
           std::shared_ptr<RecommendationSystem> rs)
    : _name(name),
      _ranks(0, sp_movie_hash, sp_movie_equal),
      _rs(std::move(rs))
{
    for (const auto& p : ranks)
    {
        _ranks[p.first] = p.second;
    }
}


const std::string& User::get_name() const
{
    return _name;
}

const rank_map& User::rank_get() const
{
    return _ranks;
}

void User::add_movie_to_user(const std::string& name,
                             int year,
                             const std::vector<double>& features,
                             double rate)
{
    if (rate < 1.0 || rate > 10.0)
    {
        throw std::invalid_argument("rate must be in [1,10]");
    }

    for (double x : features)
    {
        if (x < 1.0 || x > 10.0)
        {
            throw std::invalid_argument("feature values must be in [1,10]");
        }
    }

    sp_movie m = _rs->add_movie_to_rs(name, year, features);

    _ranks[m] = rate;
}

sp_movie User::get_rs_recommendation_by_content()
{
    return _rs->recommend_by_content(*this);
}

sp_movie User::get_rs_recommendation_by_cf(int k)
{
    return _rs->recommend_by_cf(*this, k);
}

double User::get_rs_prediction_score_for_movie(const std::string& name, int year, int k)
{
    sp_movie m = _rs->get_movie(name, year);
    if (m == nullptr)
    {
        return 0.0;
    }
    return _rs->predict_movie_score(*this, m, k);
}

std::ostream& operator<<(std::ostream& os, const User& user)
{
    os << "name: " << user._name << "\n";

    os << *(user._rs);

    os << std::endl;
    return os;
}
