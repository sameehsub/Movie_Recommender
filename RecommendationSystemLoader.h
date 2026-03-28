#ifndef RECOMMENDATIONSYSTEMLOADER_H
#define RECOMMENDATIONSYSTEMLOADER_H

#include <memory>
#include <string>

#include "RecommendationSystem.h"

class RecommendationSystemLoader
{
public:
    RecommendationSystemLoader() = delete;

    static std::unique_ptr<RecommendationSystem>
    create_rs_from_movies(const std::string& movies_file_path);
};

#endif // RECOMMENDATIONSYSTEMLOADER_H
