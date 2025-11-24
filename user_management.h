#ifndef USER_MANAGEMENT_H
#define USER_MANAGEMENT_H

#include "structures.h"

void load_users(User users[], int *user_count);
void save_users(User users[], int user_count);
int login(User users[], int user_count, char *logged_username);
void register_user(User users[], int *user_count);
void create_new_strategy(User *user);
void edit_strategy(User *user);
void show_user_strategies(User *user);
void delete_strategy(User *user);
Strategy select_user_strategy(User *user);
void strategy_management_menu(User *user, User users[], int user_count);

#endif