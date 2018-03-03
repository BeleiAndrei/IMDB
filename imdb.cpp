// Copyright 2017 <Belei Andrei & Birzan Andrei>
#include <iterator>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <cmath>
#include <utility>
#include "include/imdb.h"


#define YEAR 365
#define DAY (24 * 3600)

// Functie pentru conversia din Unix timestamp in ani
int toYear(int time){
    int year = 1970;
    time /= DAY;
    while (time - YEAR > 0){
        if (year%4 == 0)
            time--;
        year++;
        time -= YEAR;
    }
    return year;
}

// Functie pentru conversia din double in string rotunjit la 2 zecimale
std::string doubleToString(double f) {
    std::string s;
    f = std::round(f*100)/100;
    s = std::to_string(f);
    for (unsigned int i = 0 ; i < s.length(); i++) {
        if (s[i] == '.') {
            i += 3;
            s.erase(i, s.length());
        }
    }
    return s;
}

IMDb::IMDb() {
    // initialize what you need here.
}

IMDb::~IMDb() {}

void IMDb::add_movie(std::string movie_name,
                     std::string movie_id,
                     int timestamp,  // unix timestamp when movie was launched
                     std::vector<std::string> categories,
                     std::string director_name,
                     std::vector<std::string> actor_ids ) {
    std::map<std::string, actor_info<std::string>>::iterator actor_it;
    movie_info<std::string> movie(movie_name, timestamp,
    	director_name, categories, actor_ids);
    std::list <director_info<std::string>>::iterator dir_it;
    std::list <std::string>::iterator actor_it2;
    // Adaugam filmul intr-o lista ce contorizeaza numarul de rating-uri primite
    Popularity.push_back(Pop<std::string>(0, movie_id));
    // Adaugam filmul intr-o lista ce ce memoreaza data aparitiei
    Latest.push_back(Freshness<std::string>(timestamp, movie_id));
    // Reinitializam campurile ce mentioneaza daca listele sunt sortate la false
    Latest_Sorted = false;
    Pop_Sorted = false;

    // Verificam daca directorul a mai regizat
    for (dir_it = directors.begin(); dir_it != directors.end(); ++dir_it)
        if (dir_it->name == director_name) {
        	// Lista de actori cu care a lucrat nu va mai fii sortata
            dir_it->isSorted = false;
            break;
        }

    // Daca nu a fost gasit il adaugam
    if (dir_it == directors.end())
            directors.push_back(director_info<std::string>(director_name));

    // Parcurgem vectorul de actori din film
    for (uint i = 0; i < actor_ids.size(); i++) {
        actor_it = actors.find(actor_ids[i]);
        // Mentionam ca joaca intr-un film nou
        actor_it->second.new_movie(timestamp);
        // Actualizam listele de actori cu care a jucat
        for (uint j = 0; j < i; j++) {
            actor_it->second.actors.push_back(actor_ids[j]);
            actor_it->second.they_mate(actor_ids[j]);
        }
        for (uint j = i + 1; j < actor_ids.size() ; j++) {
            actor_it->second.actors.push_back(actor_ids[j]);
            actor_it->second.they_mate(actor_ids[j]);
        }
        // Mentionam ca cele doua liste nu mai sunt sortate
        actor_it->second.pairs_updated = false;
        actor_it->second.isSorted = false;

        if (dir_it == directors.end()) {
        	for (actor_it2 = directors.back().actors.begin();
        		actor_it2 != directors.back().actors.end(); actor_it2++) {
        		if (*actor_it2 == actor_ids[i]) {
        			break;
        		}
        	}
        	if (actor_it2 == directors.back().actors.end()) {
        		directors.back().actors.push_back(actor_ids[i]);
        		directors.back().num_actors++;
        	}
        } else {
        	for (actor_it2 = dir_it->actors.begin();
        		actor_it2 != dir_it->actors.end(); actor_it2++) {
        		if (*actor_it2 == actor_ids[i]) {
        			break;
        		}
        	}
        	if (actor_it2 == dir_it->actors.end()) {
        		dir_it->actors.push_back(actor_ids[i]);
        		dir_it->num_actors++;
        	}
        }
    }
    // Adaugam filmul in Hashtable
    movies.insert(std::pair<std::string, movie_info<std::string>>
    	(movie_id, movie));
}

void IMDb::add_user(std::string user_id, std::string name) {
    users.insert(std::pair<std::string, user_info<std::string>>
    	(user_id, user_info<std::string>(name)));
}

void IMDb::add_actor(std::string actor_id, std::string name) {
	actors.insert(std::pair<std::string, actor_info<std::string>>
		(actor_id, actor_info<std::string> (name)));
}

void IMDb::add_rating(std::string user_id, std::string movie_id, int rating) {
    std::map<std::string, user_info<std::string>>::iterator user_it;
    std::map<std::string, int>::iterator rating_it;
    std::map<std::string, movie_info<std::string>>::iterator movie_it;
    std::list<Pop<std::string>>::iterator Renown_it;
	std::list<Freshness<std::string>>::iterator Fresh_it;

    for (Fresh_it = Latest.begin(); Fresh_it != Latest.end(); ++Fresh_it) {
    	if (Fresh_it->movie_id == movie_id){
    		Fresh_it->totalRatings += rating;
    		Fresh_it->nrRatings++;
    		break;
    	}
    }

    // Cautam filmul in lista ce contorizeaza nr de ratinguri si o modificam
    for (Renown_it = Popularity.begin();
    	Renown_it != Popularity.end(); ++Renown_it) {
        if (Renown_it->movie_id == movie_id) {
            Renown_it->nrRatings++;
            Pop_Sorted = false;
            break;
        }
    }

    // Verificam daca userul a amai adaugar un rating la acelasi film
    user_it = users.find(user_id);
    rating_it = user_it->second.ratings.find(movie_id);
    if (rating_it == user_it->second.ratings.end()) {
        user_it->second.ratings.insert(std::pair<std::string,
        	int>(movie_id, rating));
        movie_it = movies.find(movie_id);
        movie_it->second.totalRatings += rating;
        movie_it->second.nrRatings++;
    }
}

void IMDb::update_rating(std::string user_id,
						std::string movie_id, int rating) {
    std::map<std::string, user_info<std::string>>::iterator user_it;
    std::map<std::string, int>::iterator rating_it;
    std::map<std::string, movie_info<std::string>>::iterator movie_it;
    std::list<Freshness<std::string>>::iterator Fresh_it;

    // Cautam ratingul anterior
    user_it = users.find(user_id);
    rating_it = user_it->second.ratings.find(movie_id);
    if (rating_it != user_it->second.ratings.end()) {
        movie_it = movies.find(movie_id);
        movie_it->second.totalRatings -= rating_it->second;
        movie_it->second.totalRatings += rating;
        for (Fresh_it = Latest.begin(); Fresh_it != Latest.end(); ++Fresh_it) {
    		if (Fresh_it->movie_id == movie_id){
    			Fresh_it->totalRatings += rating - rating_it->second;
    			break;
    		}
    	}
        rating_it->second = rating;
    }
}

void IMDb::remove_rating(std::string user_id,
						std::string movie_id) {
    std::map<std::string, user_info<std::string>>::iterator user_it;
    std::map<std::string, int>::iterator rating_it;
    std::map<std::string, movie_info<std::string>>::iterator movie_it;
    std::list<Pop<std::string>>::iterator Renown_it;
    std::list<Freshness<std::string>>::iterator Fresh_it;

    // Cautam filmul in lista ce contorizeaza nr de ratinguri si o modificam
    for (Renown_it = Popularity.begin();
    	Renown_it != Popularity.end(); ++Renown_it) {
        if (Renown_it->movie_id == movie_id) {
            Renown_it->nrRatings--;
            Pop_Sorted = false;
            break;
        }
    }

    // Cautam ratingul respectiv
    user_it = users.find(user_id);
    rating_it = user_it->second.ratings.find(movie_id);
    if (rating_it != user_it->second.ratings.end()) {
        movie_it = movies.find(movie_id);
        movie_it->second.totalRatings -= rating_it->second;
        movie_it->second.nrRatings--;
        for (Fresh_it = Latest.begin(); Fresh_it != Latest.end(); ++Fresh_it) {
	    	if (Fresh_it->movie_id == movie_id){
	    		Fresh_it->totalRatings -= rating_it->second;
	    		Fresh_it->nrRatings--;
	    		break;
	    	}
	    }
        user_it->second.ratings.erase(rating_it);
    }
}

std::string IMDb::get_rating(std::string movie_id) {
    std::map<std::string, movie_info<std::string>>::iterator movie_it;

    double r;

    movie_it = movies.find(movie_id);
    if (movie_it->second.nrRatings != 0) {
        r = movie_it->second.totalRatings / (double)movie_it->second.nrRatings;
    } else {
        return "none";
    }
    return doubleToString(r);
}

std::string IMDb::get_longest_career_actor() {
    std::map<std::string, actor_info<std::string>>::iterator actor_it;
    std::string idExperiencedActor = "none";
	// max retine durata maxima, iar cur durata curenta
    int max = -1, cur;

    // Parcurgem lista de acotri
    for (actor_it = actors.begin(); actor_it != actors.end(); actor_it++) {
        if (actor_it->second.fmovie != -1){
        cur = actor_it->second.lmovie - actor_it->second.fmovie;
        if (max < cur){
            idExperiencedActor = actor_it->first;
            max = cur;
        } else {
        	if (max == cur)
                if (idExperiencedActor > actor_it->first) {
                    idExperiencedActor = actor_it->first;
                	}
           	}
        }
    }
    return idExperiencedActor;
}

std::string IMDb::get_most_influential_director() {
	int max = -1;
	std::string name = "none";
	std::list <director_info <std::string> >::iterator dir_it;

	// Parcurgem lista de directori
	for (dir_it = directors.begin(); dir_it != directors.end(); dir_it++){
        if (dir_it->num_actors > max) {
        	max = dir_it->num_actors;
        	name = dir_it->name;
        } else if (dir_it->num_actors == max && dir_it->name < name) {
        	name = dir_it->name;
        }
    }

	return name;
}


std::string IMDb::get_best_year_for_category(std::string category) {
    // year este un vector al carui index reprezinta anul si retine
    // ratingul total al fimelor din anul si categoria respectiva
    // precum si numarul de filme din categorie in acel an
    double year[68][2] = {0}, max = 0;
    int the_grand_year = -1;
    std::map<std::string, movie_info<std::string>>::iterator movie_it;

    // Parcurgem lista de filme
    for (movie_it = movies.begin();
    	movie_it != movies.end(); ++movie_it) {
    	// Parcurgem lista de categorii
        for (uint i = 0; i < movie_it->second.categories.size(); i++)
            if (movie_it->second.categories[i] == category){
            	if (movie_it->second.nrRatings != 0) {
            		// Daca filmul a primit cel putin un rating il adaugam in vector
                	year[toYear(movie_it->second.data) - 1970][0] +=
                	movie_it->second.totalRatings/
                	(double)movie_it->second.nrRatings;

                	year[toYear(movie_it->second.data) - 1970][1]++;
                }
                break;
            }
    }

    // Parcurgem descrescator vectorul pentru a retine anul minim
    for (int i = 67; i >= 0; i--)
        if (year[i][1] !=0 && max <= year[i][0]/year[i][1]){
            max = year[i][0]/year[i][1];
            the_grand_year = i + 1970;
        }
    if (the_grand_year == -1) {
        return "none";
    }
    return std::to_string(the_grand_year);
}

std::string IMDb::get_2nd_degree_colleagues(std::string actor_id) {
    std::map<std::string, actor_info<std::string>>::iterator actor_it;
    std::map<std::string, actor_info<std::string>>::iterator actor_it2;
    std::list<std::string>::iterator list_it;
    std::list<std::string>::iterator list_it2;
    std::list<std::string>::iterator list_it3;
    std::list<std::string> Coleagues;
    std::string col = "";

    actor_it = actors.find(actor_id);
    if (!actor_it->second.isSorted) {
       actor_it->second.actors.sort();
       actor_it->second.actors.unique();
       actor_it->second.isSorted = true;
    }
    for (list_it = actor_it->second.actors.begin();
    	list_it != actor_it->second.actors.end(); list_it++) {
            actor_it2 = actors.find(*list_it);
            if (!actor_it2->second.isSorted) {
                actor_it2->second.isSorted = true;
                actor_it2->second.actors.sort();
                actor_it2->second.actors.unique();
            }
            for (list_it2 = actor_it2->second.actors.begin();
            	list_it2 != actor_it2->second.actors.end(); list_it2++) {
                list_it3 = actor_it->second.actors.begin();
                if (*list_it2 != actor_id) {
                    while (list_it3 != actor_it->second.actors.end()
                    	&& *list_it2 > *list_it3) {
                        list_it3++;
                    }
                    if (list_it3 != actor_it->second.actors.end()) {
						if (*list_it2 < *list_it3) {
	                        Coleagues.push_back(*list_it2);
						}
                    } else {
						Coleagues.push_back(*list_it2);
					}
                }
            }
    }

    list_it = Coleagues.begin();
    if (list_it == Coleagues.end())
    	return "none";

    Coleagues.sort();
    Coleagues.unique();

    list_it = Coleagues.begin();
    col+= *list_it;
    list_it++;

    for (; list_it != Coleagues.end(); ++list_it) {
    	col += " ";
    	col += *list_it;
    }

    return col;
}

std::string IMDb::get_top_k_most_recent_movies(int k) {
    std::string Newest = "";
    std::list<Freshness<std::string>>::iterator Fresh_it;

    // Verificam daca lista e deja sortata
    if (!Latest_Sorted) {
    	// Daca nu o sortam
        Latest.sort();
        Latest_Sorted = true;
    }

    // Cazul in care lista e goala
    Fresh_it = Latest.begin();
    if (Fresh_it == Latest.end())
        return "none";

    // Cream stringul Newest
    Newest += Fresh_it->movie_id;
    Fresh_it++;
    k--;

    for (; Fresh_it != Latest.end() && k > 0; ++Fresh_it) {
        k--;
        Newest += " ";
        Newest += Fresh_it->movie_id;
    }

    return Newest;
}

std::string IMDb::get_top_k_actor_pairs(int k) {
	std::string top_mated = "";
	std::map<std::string, actor_info<std::string>>::iterator actor_it;
    std::list<pair_info<std::string>>::iterator pair_it;
    // Ps retine perechile
    std::list<Couple<std::string>> Ps;
    std::list<Couple<std::string>>::iterator M_it;

    int min = 0, ct, ok;

    for (actor_it = actors.begin(); actor_it != actors.end(); ++actor_it) {
    	if (actor_it->second.pairs.begin() != actor_it->second.pairs.end()) {
    		ct = k;
    		// Verificam daca lista e deja sortata
	    	if (!actor_it->second.pairs_updated) {
	    		// Daca nu o sortam
	        	actor_it->second.pairs.sort();
	        	actor_it->second.pairs_updated = true;
    		}
    		// Parcurgem lista de pairs a actorului
    		// Ne oprim daca am adaugat mai mult de k actori in PS
    		// si numarul de filme in care a jucat este diferit de min
			// sau ne oprim cand numarul de filme in care
			// au jucat este mai mic ca min
			for (pair_it = actor_it->second.pairs.begin();
				pair_it != actor_it->second.pairs.end()
				&& (ct > 0 || pair_it->Nrmovies >= min)
				&& pair_it->Nrmovies >= min; ++pair_it) {
				ct--;
				// Daca ok ramane 1 inseamna ca perechea nu a fost adaugata anterior
				ok = 1;
				for (M_it = Ps.begin(); M_it != Ps.end(); ++M_it) {
					if ((M_it->actor_id1 == actor_it->first
						&& M_it->actor_id2 == pair_it->actor_id)) {
						// Daca o gasim crestem numarul de filme comune
						M_it->Nrmovies += pair_it->Nrmovies;
						ok = 0;
					}
					if (M_it->actor_id2 == actor_it->first
						&& M_it->actor_id1 == pair_it->actor_id){
						// Cazul in care o gasim dar perechea e inversata
						ok = 0;
					}
				}
				if (ok) {
					if (actor_it->first < pair_it->actor_id)
						Ps.push_back(Couple<std::string>(actor_it->first,
							pair_it->actor_id, pair_it->Nrmovies));
					else
						Ps.push_back(Couple<std::string>(pair_it->actor_id,
							actor_it->first, pair_it->Nrmovies));
				}

				// Actualizarea minimului
				if (min < Ps.back().Nrmovies && ct == 0){
					min++;
				}
			}
		}
	}

	Ps.sort();

	if (Ps.begin() == Ps.end())
        return "none";
	M_it = Ps.begin();

	// Crearea stringului top_mated
    top_mated += "(" + M_it->actor_id1 + " " + M_it->actor_id2;
    top_mated += " " + std::to_string(M_it->Nrmovies) + ")";
    k--;
    ++M_it;

    for (; M_it != Ps.end() && k > 0; ++M_it) {
        k--;
        top_mated += " ";
        top_mated += "(" + M_it->actor_id1 + " " + M_it->actor_id2;
        top_mated += " " + std::to_string(M_it->Nrmovies) + ")";
    }

    return top_mated;
}

std::string IMDb::get_top_k_partners_for_actor(int k, std::string actor_id) {
    std::string Matchup = "";
    std::map<std::string, actor_info<std::string>>::iterator actor_it;
    std::list<pair_info<std::string>>::iterator pair_it;

    actor_it = actors.find(actor_id);

    pair_it = actor_it->second.pairs.begin();
    if (pair_it == actor_it->second.pairs.end())
        return "none";
    // Verificam daca lista e deja sortata
    if (!actor_it->second.pairs_updated) {
    	// Daca nu o sortam
        actor_it->second.pairs.sort();
        actor_it->second.pairs_updated = true;
    }

    // Cream stringul Matchup
    pair_it = actor_it->second.pairs.begin();
    Matchup+=pair_it->actor_id;
    pair_it++;
    k--;

    for (; pair_it != actor_it->second.pairs.end() && k > 0; ++pair_it) {
        k--;
        Matchup += " ";
        Matchup += pair_it->actor_id;
    }

    return Matchup;
}

std::string IMDb::get_top_k_most_popular_movies(int k) {
    std::string retur = "";
    std::list<Pop<std::string>>::iterator Renown_it;
    // Verificam daca lista e deja sortata
    if (!Pop_Sorted) {
    	// Daca nu o sortam
        Popularity.sort();
        Pop_Sorted = true;
    }
    Renown_it = Popularity.begin();
    if (Renown_it == Popularity.end())
        return "none";

    // Cream stringul retur
    retur += Renown_it->movie_id;
    Renown_it++;
    k--;

    for (; Renown_it!= Popularity.end() && k > 0; ++Renown_it) {
        k--;
        retur += " ";
        retur += Renown_it->movie_id;
    }

    return retur;
}

std::string IMDb::get_avg_rating_in_range(int start, int end) {
    double totalRating = 0;
    int numberRating = 0;
    std::map<std::string, movie_info<std::string>>::iterator m_it;
    std::list<std::string>::iterator list_it;


    std::list<Freshness<std::string>>::iterator Fresh_it;

    // Verificam daca lista e deja sortata
    if (!Latest_Sorted) {
    	// Daca nu o sortam
        Latest.sort();
        Latest_Sorted = true;
    }

    for (Fresh_it = Latest.begin(); Fresh_it != Latest.end()
    	&& Fresh_it->date >= start; ++Fresh_it) {
    	if (Fresh_it->date <= end && Fresh_it->nrRatings) {
            totalRating += Fresh_it->totalRatings/(double)Fresh_it->nrRatings;
            numberRating++;
    	}
    }

    if (numberRating)
        return doubleToString(totalRating/numberRating);
    return "none";
}
