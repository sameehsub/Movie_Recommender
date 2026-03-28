#ifndef USERSLOADER_H
#define USERSLOADER_H

#include <memory>
#include <string>
#include <vector>

#include "RecommendationSystem.h"
#include "User.h"

class UsersLoader
{
public:
    UsersLoader() = delete;

    static std::vector<User>
    create_users(const std::string& users_file_path,
                 std::unique_ptr<RecommendationSystem> rs);
};

#endif // USERSLOADER_H
