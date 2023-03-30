#ifndef __LIST_HPP__
#define __LIST_HPP__

#include <iostream>
#include <list>
#include <map>

class NList
{
public:
    std::list<std::list<int>> data;
    size_t size;
    
    NList() : data(), size(0) {};
    ~NList() = default;

    void insert(int node)
    {
        std::list<int> childList;
        childList.emplace_back(node);
        ++size;
        data.emplace_back(childList);
    }

    int insert(int parent, int node)
    {
        for (auto i = data.begin(); i != data.end(); i++) {
            for (auto j = i->begin(); j != i->end(); j++) {
                if (*j == parent) {
                    i->insert(++j, node);
                    ++size;
                    return 1;
                }
            }
        }
        return 0;
    }
    
    int erace(int node)
    {
        for (auto i = data.begin(); i != data.end(); i++) {
            for (auto j = i->begin(); j != i->end(); j++) {
                if (*j == node) {
                    if (i->size() > 1) {
                        i->erase(j);
                    } else {
                        data.erase(i);
                    }
                    --size;
                    return 1;
                }
            }
        }
        return 0;
    }

    int find(int node)
    {
        int index = 0;
        for (auto i: data) {
            for (auto j: i) {
                if (j == node) {
                    return index;
                }
            }
            ++index;
        }
        return -1;
    }

    friend std::ostream &operator<<(std::ostream &out, NList list)
    {
        for (auto &i: list.data) {
            for (auto &j: i) {
                out << j << " ";
            }
            out << "\n";
        }
        return out;
    }
};

#endif