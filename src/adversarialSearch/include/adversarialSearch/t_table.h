// transpositionTable.h

#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include <mutex>
#include <stdexcept>
#include <iostream>

enum class entry_type {
    exact,
    l_bound,    // beta
    u_bound     // alpha
};

template <typename U, typename A>
struct tt_entry {
    long hash;
    entry_type type;
    int depth;
    U score;
    int best_action_index;
};

template <typename U, typename A>
class t_table {
private:
    std::mutex mtx;
    tt_entry<U, A>* table;
    int size;
    static const int preferredSize = 200000000;
    U unknown;

    inline int getIndex(long& hash) {
        return (u_long) hash % size;
    }

public:
    t_table(int size, U unknown) {
        this->unknown = unknown;
        this->size = size;
        if (size <= 0) {
            std::cout << "preferredSize: " << preferredSize << std::endl;
            throw std::invalid_argument("Size must be greater than 0, not " + std::to_string(size));
        }
        while (size > 0) {
            try {
                table = new tt_entry<U, A>[size];
                if (table == nullptr) {
                    std::cerr << "Failed to allocate memory for transposition table of size " << size << std::endl;
                    throw std::bad_alloc();
                }
                break;
            } catch (const std::bad_alloc&) {
                size /= 2;
            }
        }

        if (size == 0) {
            table = nullptr;
            throw std::runtime_error("Failed to allocate memory for transposition table");
        } else {
            for (int i = 0; i < size; i++) {
                table[i].hash = 0;
                table[i].type = entry_type::exact;
                table[i].depth = 0;
                table[i].score = unknown;
                table[i].best_action_index = 0;
            }
        }
    }

    t_table(U unknown) : t_table(t_table::preferredSize, unknown) {}

    ~t_table() {
        delete[] table;
    }


    void insert(int64_t hash, entry_type type, U score, int depth, int best_action_index) {

        #ifndef TT_LOCKLESS
            std::lock_guard<std::mutex> lock(mtx);
        #endif
        
        int index = getIndex(hash);
        auto& entry = table[index];

        entry.hash = hash;
        entry.type = type;
        entry.depth = depth;
        entry.score = score;
        entry.best_action_index = best_action_index;
    }

    void insert(int64_t hash, entry_type type, U score, int depth) {

        #ifndef TT_LOCKLESS
            std::lock_guard<std::mutex> lock(mtx);
        #endif
        
        int index = getIndex(hash);
        auto& entry = table[index];

        entry.hash = hash;
        entry.type = type;
        entry.depth = depth;
        entry.score = score;
        entry.best_action_index = 0;    // no best action index, clear the value
    }

    U probe(int64_t hash, U alpha, U beta, int depth, int& best_action_index) {
        
        #ifndef TT_LOCKLESS
            std::lock_guard<std::mutex> lock(mtx);
        #endif
        
        int index = getIndex(hash);
        auto& entry = table[index];

        if (entry.hash == hash) {

            if (entry.best_action_index != 0)
                best_action_index = entry.best_action_index;

            if (entry.depth >= depth) {
                
                if (entry.type == entry_type::exact) {
                    return entry.score;
                }
                
                if (entry.type == entry_type::u_bound && entry.score <= alpha) {
                    return entry.score;
                }
                
                if (entry.type == entry_type::l_bound && entry.score >= beta) {
                    return entry.score;
                }
            }
        }
        
        return unknown;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mtx);
        for (int i = 0; i < size; i++) {
            table[i].hash = 0;
            table[i].type = entry_type::exact;
            table[i].depth = 0;
            table[i].score = unknown;
            table[i].best_action_index = 0;
        }
    }

};



#endif // TRANSPOSITIONTABLE_H