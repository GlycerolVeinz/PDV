//
// Created by karel on 12.2.18.
//

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include "decryption.h"
#include <cmath>

using namespace std;

void decrypt_sequential(const PDVCrypt &crypt, std::vector<std::pair<std::string, enc_params>> &encrypted, unsigned int) {
    // V sekvencni verzi je pocet vlaken (numThreads) ignorovany
    const auto size = static_cast<unsigned long>(encrypted.size());
    for(unsigned long i = 0 ; i < size ; i++) {
        auto & enc = encrypted[i];
        crypt.decrypt(enc.first, enc.second);
    }
}

void decrypt_openmp(const PDVCrypt &crypt, std::vector<std::pair<std::string, enc_params>> &encrypted, unsigned int numThreads) {
    const long size = static_cast<long>(encrypted.size());

    #pragma omp parallel for num_threads(numThreads)
    for(long i = 0 ; i < size ; i++) {
        auto & enc = encrypted[i];
        crypt.decrypt(enc.first, enc.second);
    }
}

void decrypt_threads_1(const PDVCrypt &crypt, std::vector<std::pair<std::string, enc_params>> &encrypted, unsigned int numThreads) {
    const unsigned long size = static_cast<unsigned long>(encrypted.size());
    unsigned long i = 0;
    std::vector<std::thread> threads;

    auto process = [&]() {
        while(i < size) {
            auto & enc = encrypted[i];
            i++;
            crypt.decrypt(enc.first, enc.second);
        }
    };

    for (size_t t = 0; t < numThreads; t++) {
        threads.emplace_back(process);
    }

    for (auto & t : threads) {
        t.join();
    }
}

void decrypt_threads_2(const PDVCrypt &crypt, std::vector<std::pair<std::string, enc_params>> &encrypted, unsigned int numThreads) {
    const unsigned long size = static_cast<unsigned long>(encrypted.size());
    unsigned long i = 0;

    std::mutex mutex;
    std::vector<std::thread> threads;

    auto process = [&]() {
        while(i < size) {
            mutex.lock();
            auto & enc = encrypted[i];
            i++;
            mutex.unlock();
            crypt.decrypt(enc.first, enc.second);
        }
    };

    for (size_t t = 0; t < numThreads; t++) {
        threads.emplace_back(process);
    }

    for (auto & t : threads) {
        t.join();
    }
}

void decrypt_threads_3(const PDVCrypt &crypt, std::vector<std::pair<std::string, enc_params>> &encrypted, unsigned int numThreads) {
    const unsigned long size = static_cast<unsigned long>(encrypted.size());
    std::atomic<unsigned long> i{0};
    std::vector<std::thread> threads;

    auto process = [&]() {
        while(i < size) {
            unsigned long local_i = i++;
            auto & enc = encrypted[local_i];
            crypt.decrypt(enc.first, enc.second);
        }
    };

    for (size_t t = 0; t < numThreads; t++) {
        threads.emplace_back(process);
    }

    for (auto & t : threads) {
        t.join();
    }
}

void decrypt_threads_4(const PDVCrypt &crypt, std::vector<std::pair<std::string, enc_params>> &encrypted, unsigned int numThreads) {

    // Doplnte logiku vypoctu disjunktnich rozsahu pro jednotliva vlakna
    // Kod pro spusteni vlaken (a predani rozsahu) je jiz hotovy. Vasim
    // cilem je spocitat hodnoty promennych 'begin' a 'end' pro kazde
    // vlakno.

    const auto size = static_cast<unsigned long>(encrypted.size());
    vector<thread> threads;
    const auto chunkSize = static_cast<size_t>(std::floor(static_cast<double>(size)/numThreads));
    for(unsigned int t = 0 ; t < numThreads ; t++) {

        // Doplnte vypocet rozsahu pro t-te vlakno zde:
        unsigned long begin = t * (chunkSize);
        unsigned long end = (t + 1) * chunkSize;

        threads.emplace_back([&](unsigned long begin, unsigned long end) {
            for(unsigned int i = begin ; i < end ; i++) {
                auto & enc = encrypted[i];
                crypt.decrypt(enc.first, enc.second);
            }
        }, begin, end);
    }

    for(unsigned int t = 0 ; t < numThreads ; t++) {
        threads[t].join();
    }

}
