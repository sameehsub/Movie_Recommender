#ifndef EX5_MOVIE_H
#define EX5_MOVIE_H

#include <iostream>
#include <memory>
#include <string>

class Movie;


using sp_movie = std::shared_ptr<Movie>;

/**
 * These declarations and typedefs are given to you and should be used in the exercise.
 */
using hash_func = std::size_t (*)(const sp_movie& movie);
using equal_func = bool (*)(const sp_movie& m1, const sp_movie& m2);

std::size_t sp_movie_hash(const sp_movie& movie);
bool sp_movie_equal(const sp_movie& m1, const sp_movie& m2);

class Movie
{
private:
    std::string _name;
    int _year;

public:
    Movie(const std::string& name, int year);

    const std::string& get_name() const;
    int get_year() const;

    bool operator<(const Movie& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Movie& movie);
};

#endif // EX5_MOVIE_H
