#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include "hashtable.h"

template <typename K, typename V>
HashTable<K, V>::HashTable(size_t size) : currentSize(0)
{
    size_t prime = prime_below(size);
    if (prime == 0)
        theLists.resize(default_capacity); // Use default capacity if prime_below returns 0
    else
    {
        theLists.resize(prime);
    }
    cout << "In total, there are " << theLists.size() << " rows in the vector." << endl;
}

template <typename K, typename V>
HashTable<K, V>::~HashTable()
{
    clear();
}

template <typename K, typename V>
bool HashTable<K, V>::contains(const K &k) const
{
    size_t index = myhash(k);
    for (auto &ele : theLists[myhash(k)])
    {
        if (ele.first == k)
            return true;
    }
    return false;
}

template <typename K, typename V>
bool HashTable<K, V>::match(const std::pair<K, V> &kv) const
{
    size_t index = myhash(kv.first);
    for (auto &ele : theLists[index])
    {
        if (ele == kv)
            return true;
    }
    return false;
}

template <typename K, typename V>
bool HashTable<K, V>::insert(const std::pair<K, V> &kv)
{
    if (match(kv) == false)
    {
        return false;
    }
    if (contains(kv.first))
    {
        auto &whichList = theLists[myhash(kv.first)];
        for (auto &ele : whichList)
        {

            if (ele.first == kv.first)
            {
                ele.second = kv.second;
                return true;
            }
        }
    }
    else
    {
        auto &whichList = theLists[myhash(kv.first)];
        whichList.push_back(kv);
        currentSize++;

        if (currentSize > theLists.size())
        {
            rehash();
        }
        return true;
    }
    return false;
}

template <typename K, typename V>
bool HashTable<K, V>::insert(std::pair<K, V> &&kv)
{
    if (match(kv) == false)
    {
        return false;
    }
    size_t idx = myhash(kv.first); // get index of vector
    if (contains(kv.first))
    {
        auto &whichList = theLists[idx];
        for (auto &ele : whichList)
        {
            if (ele.first == kv.first)
            {
                ele.second = std::move(kv.second);
                return true;
            }
        }
    }
    else
    {
        auto &whichList = theLists[idx];
        whichList.push_back(std::move(kv));
        ++currentSize;
        if (currentSize > theLists.size())
        {
            rehash();
        }
        return true;
    }
    return false;
}
//----------------------------CHECK THIS---------
template <typename K, typename V>
bool HashTable<K, V>::remove(const K &k)
{
    auto &whichList = theLists[myhash(k)];
    for (auto it = whichList.begin(); it != whichList.end(); ++it)
    {
        if ((*it).first == k)
        {
            whichList.erase(it);
            --currentSize;
            return true;
        }
    }
    return false;
}

template <typename K, typename V>
void HashTable<K, V>::clear()
{
    makeEmpty();
}

template <typename K, typename V> // need to fix
bool HashTable<K, V>::load(const char *filename)
{
    K k;
    V v;

    ifstream fh(filename);
    if (!fh.is_open())
    {
        cerr << "Unable to open file " << filename << endl;
        return false;
    }
    else
    {
        while (fh >> k >> v)
        {
            if (contains(k))
            {
                cout << "error" << endl;
                continue;
            }
            else
            {
                insert(std::make_pair(k, v));
            }
        }
        fh.close();
        return true;
    }
}
//-------------------------------------//

template <typename K, typename V>
void HashTable<K, V>::dump() const
{ // need to check
    cout << "In dump, in total there are " << theLists.size() << " rows in the vector" << endl;
    for (size_t i = 0; i < theLists.size(); ++i)
    {
        cout << "v[" << i << "]";
        if (theLists[i].size() >= 1)
        {
            auto it = theLists[i].begin();
            cout << (*it).first << " " << (*it).second;
            ++it;
            for (; it != theLists[i].end(); ++it)
            {
                cout << " : " << (*it).first << " " << (*it).second;
            }
        }
        cout << endl;
    }
}

template <typename K, typename V>
size_t HashTable<K, V>::size() const
{
    return currentSize;
}

template <typename K, typename V> // need to fix
bool HashTable<K, V>::write_to_file(const char *filename) const
{
    ofstream fh(filename);
    if (fh.is_open())
    {
        for (const auto &list : theLists)
        {
            for (auto &kv : list)
            {
                fh << kv.first << " " << kv.second << '\n';
            }
        }
        fh.close();
        return true;
    }
    return false;
}

template <typename K, typename V>
void HashTable<K, V>::makeEmpty()
{
    for (auto &thisList : theLists)
    {
        thisList.clear();
    }
}

template <typename K, typename V>
void HashTable<K, V>::rehash()
{
    vector<list<pair<K, V>>> oldLists = theLists;
    // create new double-sized but prime, empty table
    // theLists.resize(nextPrime(2 * theLists.size()));
    for (auto &thisList : theLists)
        thisList.clear();

    // copy table over
    currentSize = 0;
    for (auto &thisList : oldLists)
        for (auto &x : thisList)
            insert(std::move(x));
}
template <typename K, typename V>
size_t HashTable<K, V>::myhash(const K &k) const
{
    static hash<K> hf;
    cout << "hashed position of " << k << " is " << hf(k) << endl;
    return hf(k) % theLists.size();
}

template <typename K, typename V>
unsigned long HashTable<K, V>::prime_below(unsigned long n)
{
    if (n > max_prime)
    {
        std::cerr << "** input too large for prime_below()\n";
        return 0;
    }
    if (n == max_prime)
    {
        return max_prime;
    }
    if (n <= 1)
    {
        std::cerr << "** input too small \n";
        return 0;
    }

    // now: 2 <= n < max_prime
    std::vector<unsigned long> v(n + 1);
    setPrimes(v);
    while (n > 2)
    {
        if (v[n] == 1)
            return n;
        --n;
    }

    return 2;
}

template <typename K, typename V>
void HashTable<K, V>::setPrimes(vector<unsigned long> &vprimes)
{
    int i = 0;
    int j = 0;

    vprimes[0] = 0;
    vprimes[1] = 0;
    int n = vprimes.capacity();

    for (i = 2; i < n; ++i)
        vprimes[i] = 1;

    for (i = 2; i * i < n; ++i)
    {
        if (vprimes[i] == 1)
            for (j = i + i; j < n; j += i)
                vprimes[j] = 0;
    }
}
#endif