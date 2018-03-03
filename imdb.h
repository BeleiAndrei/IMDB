// Copyright 2017 <Belei Andrei & Birzan Andrei>
#ifndef __IMDB__H__  // _HOME_ANDREI_DESKTOP_SD_TEMA3_IMDB_H_
#define __IMDB__H__

#include <string>
#include <vector>
#include <map>
#include <list>

#define NONE                            "none"

#define ADD_MOVIE                       "add_movie"
#define ADD_USER                        "add_user"
#define ADD_ACTOR                       "add_actor"
#define ADD_RATING                      "add_rating"
#define UPDATE_RATING                   "update_rating"
#define REMOVE_RATING                   "remove_rating"
// queries
#define GET_RATING                      "get_rating"
#define GET_LONGEST_CAREER_ACTOR        "get_longest_career_actor"
#define GET_MOST_INFLUENTIAL_DIRECTOR   "get_most_influential_director"
#define GET_BEST_YEAR_FOR_CATEGORY      "get_best_year_for_category"
#define GET_2ND_DEGREE_COLLEAGUES       "get_2nd_degree_colleagues"
#define GET_TOP_K_MOST_RECENT_MOVIES    "get_top_k_most_recent_movies"
#define GET_TOP_K_ACTOR_PAIRS           "get_top_k_actor_pairs"
#define GET_TOP_K_PARTNERS_FOR_ACTOR    "get_top_k_partners_for_actor"
#define GET_TOP_K_MOST_POPULAR_MOVIES   "get_top_k_most_popular_movies"
#define GET_AVG_RATING_IN_RANGE         "get_avg_rating_in_range"

std::string floatToString(float f);

// Structura ce retine toate datele filmului
template <typename T>
struct movie_info
{
    std::string name;
    int data;
    std::string director_name;
    std::vector<std::string> categories;
    std::vector<T> actor_ids;
    int totalRatings, nrRatings;

    movie_info(std::string name, int data, std::string director_name,
        std::vector<std::string> categories, std::vector<T> actor_ids){
        this->name = name;
        this->data = data;
        this->director_name = director_name;
        this->categories = categories;
        this->actor_ids = actor_ids;
        totalRatings = 0;
        nrRatings = 0;
    }
};

// Structura ce retine numarul de recenzii si id-ul filmului
// Utilitate in top_most_popular
template <typename T>
struct Pop
{
    int nrRatings;
    T movie_id;

    Pop(int nrRatings, T movie_id) {
        this->nrRatings = nrRatings;
        this->movie_id = movie_id;
    }

    // Operator pentru sortarea in lista
    bool operator< (const Pop& elem) const
    {
        if ( nrRatings == elem.nrRatings)
            return movie_id < elem.movie_id;
        return nrRatings > elem.nrRatings;
    };
};

// Structura ce retine timestampul,id-ul si ratingul filmului
// Utilitate in top_most_recent si avg_rating_in_range
template <typename T>
struct Freshness
{
    int date;
    T movie_id;
    int nrRatings, totalRatings;

    Freshness(int date, T movie_id) {
        this->date = date;
        this->movie_id = movie_id;
        this->nrRatings = 0;
        this->totalRatings = 0;
    }

    // Operator pentru sortarea in lista
    bool operator< (const Freshness& elem) const
    {
        if ( date == elem.date)
            return movie_id < elem.movie_id;
        return date > elem.date;
    };
};

// Structura ce retine numarul de filme in care au jucat impreuna
// doi actori
// Utilitate in top_k_pairs
template <typename T>
struct Couple
{
    int Nrmovies;
    T actor_id1;
    T actor_id2;

    Couple(T actor_id1, T actor_id2, int Nrmovies) {
        this->actor_id1 = actor_id1;
        this->actor_id2 = actor_id2;
        this->Nrmovies = Nrmovies;
    }

    // Operator pentru sortarea in lista
    bool operator< (const Couple& elem) const
    {
        if (Nrmovies == elem.Nrmovies) {
            if ( actor_id1 == elem.actor_id1)
                return actor_id2 < elem.actor_id2;
            else
                return actor_id1 < elem.actor_id1;
        } else {
        return Nrmovies > elem.Nrmovies;}
    };

    // Operator pentru unicizarea in lista
    bool operator== (const Couple& elem) const
    {
        return actor_id1 == elem.actor_id1 && actor_id2 == elem.actor_id2;
    };
};

// Structura ce retine datele utilizatorului
template <typename T>
struct user_info
{
    std::string name;
    std::map<T, int> ratings;

    explicit user_info(std::string name){
        this->name = name;
    }
};

// Structura ce retine datele directorului
template <typename T>
struct director_info
{
    std::string name;
    std::list<T> actors;
    int num_actors;
    bool isSorted;

    explicit director_info(std::string name){
        this->name = name;
        isSorted = false;
        num_actors = 0;
    }
};

// Structura ce retine de cate ori au jucat 2 actori in preuna
// Utilitate in top_partners_for_actor
template <typename T>
struct pair_info
{
    int Nrmovies;
    T actor_id;

    explicit pair_info(T actor_id) {
        this->Nrmovies = 1;
        this->actor_id = actor_id;
    }

    // Operator pentru sortarea in lista
    bool operator< (const pair_info& elem) const
    {
        if ( Nrmovies == elem.Nrmovies)
            return actor_id < elem.actor_id;
        return Nrmovies > elem.Nrmovies;
    };
};

// Structura ce retine datele despre un actor
template <typename T>
struct actor_info {
    std::string name;
    std::list<T> actors;
    // fmovie si lmovie retin timestamp-urile celui mai
    // recent si celui mai vechi film al actorului
    int fmovie, lmovie;
    bool isSorted;
    std::list<pair_info<std::string>> pairs;
    bool pairs_updated;

    explicit actor_info(std::string name){
        this->name = name;
        isSorted = false;
        fmovie = -1;
        lmovie = -1;
    }

    // Updatarea timestampurilor la adaugarea unui film nou
    void new_movie(int time){
        if (fmovie == -1){
            fmovie = time;
            lmovie = time;
        } else if (fmovie > time) {
            fmovie = time;
        } else if (lmovie < time) {
            lmovie = time;
        }
    }

    // Updatarea de actori cu care a mai jucat actorul
    void they_mate(T id) {
        std::list<pair_info<std::string>>::iterator pair_it;
        for (pair_it = pairs.begin(); pair_it !=pairs.end(); ++pair_it){
            if (pair_it->actor_id == id){
                pair_it->Nrmovies++;
                return;
            }
        }
        pairs.push_back(pair_info<std::string>(id));
    }
};

class IMDb {
 public:
    IMDb();
    ~IMDb();

    void add_movie(std::string movie_name, std::string movie_id, int timestamp,
                   std::vector<std::string> categories,
                   std::string director_name,
                   std::vector<std::string> actor_ids);

    void add_user(std::string user_id, std::string name);
    void add_actor(std::string actor_id, std::string name);

    void add_rating(std::string user_id, std::string movie_id, int rating);
    void update_rating(std::string user_id, std::string movie_id, int rating);
    void remove_rating(std::string user_id, std::string movie_id);

    // queries
    std::string get_rating(std::string movie_id);
    std::string get_longest_career_actor();
    std::string get_most_influential_director();
    std::string get_best_year_for_category(std::string category);
    std::string get_2nd_degree_colleagues(std::string actor_id);
    std::string get_top_k_most_recent_movies(int k);
    std::string get_top_k_actor_pairs(int k);
    std::string get_top_k_partners_for_actor(int k, std::string actor_id);
    std::string get_top_k_most_popular_movies(int k);
    std::string get_avg_rating_in_range(int start, int end);

    // input reader helper.
    std::string read_input_line(const std::string& line);


 private:
    std::map<std::string, user_info<std::string>> users;
    std::map<std::string, actor_info<std::string>> actors;
    std::list<director_info<std::string>> directors;
    std::map<std::string, movie_info<std::string>> movies;
    std::list<Pop<std::string>> Popularity;
    std::list<Freshness<std::string>> Latest;
    // Variabile ce retin daca listele sunt sortate sau nu
    bool Latest_Sorted = false;
    bool Pop_Sorted = false;
};

#endif  // _HOME_ANDREI_DESKTOP_SD_TEMA3_IMDB_H_
