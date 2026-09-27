#include <iostream>
#include <unordered_map>
#include <list>
#include <chrono>
#include <thread>
using namespace std;
using Clock = chrono::steady_clock;

class Cache {
private:
    int capacity;
    list<tuple<string, int, Clock::time_point, bool>> lru;
    unordered_map<string, list<tuple<string, int, Clock::time_point, bool>>::iterator> mp;

public:
    Cache(int cap) {
        if (cap <= 0) throw invalid_argument("capacity must be positive");
        capacity = cap;
    }

    int get(string key) {
        if (mp.find(key) == mp.end())
            return -1;
        auto it = mp[key];
        auto [k, value, expiry, hasExpiry] = *it;
        if (hasExpiry && Clock::now() > expiry) {
            lru.erase(it);
            mp.erase(key);
            return -1;
        }
        lru.erase(it);
        lru.push_front({key, value, expiry, hasExpiry});
        mp[key] = lru.begin();
        return value;
    }

    void put(string key, int value, int ttlSeconds = 0) {
        if (mp.find(key) != mp.end()) {
            lru.erase(mp[key]);
        }
        bool hasExpiry = ttlSeconds > 0;
        Clock::time_point expiry = hasExpiry ? Clock::now() + chrono::seconds(ttlSeconds) : Clock::time_point{};
        lru.push_front({key, value, expiry, hasExpiry});
        mp[key] = lru.begin();
        if (lru.size() > capacity) {
            auto last = lru.back();
            mp.erase(std::get<0>(last));
            lru.pop_back();
        }
    }
};

int main() {
    Cache cache(2);
    cache.put("A", 10);
    cout << "put(A,10)" << endl;
    cache.put("B", 20);
    cout << "put(B,20)" << endl;
    cout << "get(A) = " << cache.get("A") << endl;
    cache.put("C", 30);
    cout << "put(C,30) -> B removed" << endl;
    cout << "get(B) = " << cache.get("B") << endl;
    cout << "get(C) = " << cache.get("C") << endl;
    cout << "get(A) = " << cache.get("A") << endl;

    cout << "--- TTL demo ---" << endl;
    Cache ttlCache(2);
    ttlCache.put("X", 100, 2);
    cout << "put(X,100, ttl=2 sec)" << endl;
    cout << "get(X) immediately = " << ttlCache.get("X") << endl;
    cout << "waiting 3 seconds..." << endl;
    this_thread::sleep_for(chrono::seconds(3));
    cout << "get(X) after 3 sec = " << ttlCache.get("X") << endl;
    return 0;
}