/* Copyright 2017 Andrei Petre */
/* don't modify this file. */

#include <assert.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "include/imdb.h"


template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}


std::string IMDb::read_input_line(const std::string& line) {
    std::vector<std::string> elems = split(line, ';');

    std::string operation_type = elems[0];
    std::string result = "";

    if (operation_type == ADD_MOVIE) {
        std::string movie_name = elems[1];
        std::string movie_id = elems[2];
        int timestamp = std::stoi(elems[3]);
        std::vector<std::string> categories = split(elems[4], ',');
        std::string director_name = elems[5];
        std::vector<std::string> actor_ids = split(elems[6], ',');
        add_movie(movie_name, movie_id, timestamp, categories,
                  director_name, actor_ids);

    } else if (operation_type == ADD_USER) {
        std::string user_id = elems[1];
        std::string name = elems[2];
        add_user(user_id, name);

    } else if (operation_type == ADD_ACTOR) {
        std::string actor_id = elems[1];
        std::string name = elems[2];
        add_actor(actor_id, name);

    } else if (operation_type == ADD_RATING ||
               operation_type == UPDATE_RATING) {
        std::string user_id = elems[1];
        std::string movie_id = elems[2];
        int rating = std::stoi(elems[3]);
        assert(rating <= 10 && rating >= 0);

        if (operation_type == ADD_RATING)
            add_rating(user_id, movie_id, rating);
        else
            update_rating(user_id, movie_id, rating);

    } else if (operation_type == REMOVE_RATING) {
        std::string user_id = elems[1];
        std::string movie_id = elems[2];
        remove_rating(user_id, movie_id);

    } else if (operation_type == GET_RATING) {
        std::string movie_id = elems[1];
        result = get_rating(movie_id);

    } else if (operation_type == GET_LONGEST_CAREER_ACTOR) {
        result = get_longest_career_actor();

    } else if (operation_type == GET_MOST_INFLUENTIAL_DIRECTOR) {
        result = get_most_influential_director();

    } else if (operation_type == GET_BEST_YEAR_FOR_CATEGORY) {
        std::string category = elems[1];
        result = get_best_year_for_category(category);

    } else if (operation_type == GET_2ND_DEGREE_COLLEAGUES) {
        std::string actor_id = elems[1];
        result = get_2nd_degree_colleagues(actor_id);

    } else if (operation_type == GET_TOP_K_MOST_RECENT_MOVIES) {
        int k = std::stoi(elems[1]);
        result = get_top_k_most_recent_movies(k);

    } else if (operation_type == GET_TOP_K_ACTOR_PAIRS) {
        int k = std::stoi(elems[1]);
        result = get_top_k_actor_pairs(k);

    } else if (operation_type == GET_TOP_K_PARTNERS_FOR_ACTOR) {
        int k = std::stoi(elems[1]);
        std::string actor_id = elems[2];
        result = get_top_k_partners_for_actor(k, actor_id);

    } else if (operation_type == GET_TOP_K_MOST_POPULAR_MOVIES) {
        int k = std::stoi(elems[1]);
        result = get_top_k_most_popular_movies(k);

    } else if (operation_type == GET_AVG_RATING_IN_RANGE) {
        int start = std::stoi(elems[1]);
        int end = std::stoi(elems[2]);
        result = get_avg_rating_in_range(start, end);

    } else {
        std::cerr << "Unknown operation type for " << operation_type << "\n";
        exit(1);
    }

    return result;
}

int main() {
    std::ifstream infile("input");
    std::ofstream outfile("output");
    std::string line;
    std::string result;

    IMDb imdb;

    if (infile.is_open() && outfile.is_open()) {
        while (std::getline(infile, line)) {
            if (line == "")
                continue;
            result = imdb.read_input_line(line);
            // result has a value only if it's a query.
            if (result != "")
                outfile << result << "\n";
        }
        infile.close();
        outfile.close();
    } else {
        std::cerr << "Unable to open input or output file.\n";
    }

    return 0;
}
