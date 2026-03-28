#ifndef USER_H
#define USER_H

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Movie.h"

class RecommendationSystem;

using rank_map = std::unordered_map<sp_movie, double, hash_func, equal_func>;

class User
{
private:
	std::string _name;
	rank_map _ranks;
	std::shared_ptr<RecommendationSystem> _rs;

public:
	User(const std::string& name,
		 const rank_map& ranks,
		 std::shared_ptr<RecommendationSystem> rs);

	const std::string& get_name() const;
	const rank_map& rank_get() const;

	void add_movie_to_user(const std::string& name,
						   int year,
						   const std::vector<double>& features,
						   double rate);

	sp_movie get_rs_recommendation_by_content();
	sp_movie get_rs_recommendation_by_cf(int k);
	double get_rs_prediction_score_for_movie(const std::string& name, int year, int k);

	friend std::ostream& operator<<(std::ostream& os, const User& user);
};

#endif // USER_H
