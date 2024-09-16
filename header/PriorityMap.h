/*

    Container that iterates over Elements under one or more Keys.

*/

#ifndef PRIORITYMAP_H
#define PRIORITYMAP_H

#include <vector>
#include <unordered_map>

using std::vector, std::unordered_map;

template <typename Key, typename Element>
class PriorityMap {
private:
    const size_t level;
    const vector<Key> key;
    unordered_map<Key, PriorityMap*> pmaps;
    vector<Element> elements;

public:
    PriorityMap()
        : level(0) {     } 
        
    PriorityMap(size_t _level, vector<Key> _key)
        : level(_level), key(_key) {       }

    ~PriorityMap();

    void push(vector<Key> &label, Element elmt);
    
    bool contains(const vector<Key> &label) const;
    size_t size() const;
    size_t size(const vector<Key> &label) const;
    void clear();


    class Iterator {
    private:
        friend class PriorityMap;
        PriorityMap* owner;
        typename vector<Element>::iterator elmt_itr;
        typename unordered_map<Key, PriorityMap*>::iterator pmap_itr;
        PriorityMap::Iterator* next_itr;
        bool base;


    public:
        Iterator(const Iterator& iterator);
        Iterator()
            : owner(nullptr), next_itr(nullptr) {       }

        ~Iterator();

        const vector<Key>& get_key() const;

        Element& operator*() const;

        Iterator& operator++();

        Iterator& operator=(const Iterator rhs);

        bool operator==(Iterator rhs) const;

        bool operator!=(Iterator rhs) const;

    private:
        Iterator(PriorityMap<Key, Element>* _owner, typename vector<Element>::iterator elmt, 
            typename unordered_map<Key, PriorityMap*>::iterator pmap);

        bool done();
    };
    
    
    Iterator begin();
    Iterator end();
    Iterator begin(const vector<Key> &key);
    Iterator end(const vector<Key> &key);
};
 

/*

    PRIORITYMAP

*/

template <typename Key, typename Element>
PriorityMap<Key, Element>::~PriorityMap<Key, Element>() {
    for (auto& [key, pmap] : pmaps) {
        delete pmap;
    }
}

template <typename Key, typename Element>
void PriorityMap<Key, Element>::push(vector<Key> &label, Element elmt) {
    if (label.size() == level) { // this is pmap to push elmt
        elements.push_back(elmt);
    } else if (pmaps.count(label[level])) { // next pmap exists
        pmaps[label[level]]->push(label, elmt);
    } else { // create new pmap
        vector<Key> next_vector(label.begin(), label.begin() + level + 1);
        pmaps[label[level]] = new PriorityMap(level + 1, next_vector);
        pmaps[label[level]]->push(label, elmt);
    }
}

template <typename Key, typename Element>
bool PriorityMap<Key, Element>::contains(const vector<Key> &label) const {
    if (level < label.size() && pmaps.count(label[level]))
        return pmaps.at(label[level])->contains(label);
    else 
        return level == label.size();
}

template <typename Key, typename Element>
size_t PriorityMap<Key, Element>::size() const {
    size_t total = elements.size();
    for (const auto& [key, pmap] : pmaps)
        total += pmap->size();
    return total;
}

template <typename Key, typename Element>
size_t PriorityMap<Key, Element>::size(const vector<Key> &label) const {
    if (level < label.size()) {
        return pmaps.count(label[level]) ? pmaps.at(label[level])->size(label) : 0;
    } else {
        return size();
    }
}

template <typename Key, typename Element>
void PriorityMap<Key, Element>::clear() {
    elements.clear();
    for (const auto& [key, pmap] : pmaps)
        delete pmap;
    pmaps.clear();
}

template <typename Key, typename Element>
typename PriorityMap<Key, Element>::Iterator PriorityMap<Key, Element>::begin() {
    return Iterator(this, elements.begin(), pmaps.begin());
}

template <typename Key, typename Element>
typename PriorityMap<Key, Element>::Iterator PriorityMap<Key, Element>::end() {
    return Iterator(this, elements.end(), pmaps.end());
}

template <typename Key, typename Element>
typename PriorityMap<Key, Element>::Iterator PriorityMap<Key, Element>::begin(const vector<Key> &label) {
    if (level == label.size()) // last label
        return Iterator(this, elements.begin(), pmaps.begin());
    else if (level < label.size() && pmaps.count(label[level])) // not last label
        return pmaps.at(label[level])->begin(label);
    else
        return end();
}

template <typename Key, typename Element>
typename PriorityMap<Key, Element>::Iterator PriorityMap<Key, Element>::end(const vector<Key> &label) {
    if (level == label.size()) // last label
        return Iterator(this, elements.end(), pmaps.end());
    else if (level < label.size() && pmaps.count(label[level])) // not last label
        return pmaps.at(label[level])->end(label);
    else
        return end();
}


/*

    ITERATOR

*/

template <typename Key, typename Element>
PriorityMap<Key, Element>::Iterator::Iterator(const Iterator& iterator) {
    owner = iterator.owner;
    elmt_itr = iterator.elmt_itr;
    pmap_itr = iterator.pmap_itr;
    base = iterator.base;
    if (iterator.next_itr)
        next_itr = new Iterator(iterator.next_itr->owner, iterator.next_itr->elmt_itr, iterator.next_itr->pmap_itr);
    else
        next_itr = nullptr;
}

template <typename Key, typename Element>
PriorityMap<Key, Element>::Iterator::Iterator(PriorityMap<Key, Element>* _owner, typename vector<Element>::iterator elmt, 
            typename unordered_map<Key, PriorityMap*>::iterator pmap)
    : owner(_owner), elmt_itr(elmt), pmap_itr(pmap), next_itr(nullptr), base(true) {
    if (elmt_itr == owner->elements.end())
        ++(*this);
}

template <typename Key, typename Element>
PriorityMap<Key, Element>::Iterator::~Iterator() {
    if (next_itr) {
        delete next_itr;
        next_itr = nullptr;
    }
}

template <typename Key, typename Element>
const vector<Key>& PriorityMap<Key, Element>::Iterator::get_key() const {
    return next_itr ? next_itr->get_key() : owner->key;
}

template <typename Key, typename Element>
Element& PriorityMap<Key, Element>::Iterator::operator*() const {
    return next_itr ? *(*next_itr) : *elmt_itr;
}

template <typename Key, typename Element>
typename PriorityMap<Key, Element>::Iterator& PriorityMap<Key, Element>::Iterator::operator=(const Iterator rhs) {
    owner = rhs.owner;
    elmt_itr = rhs.elmt_itr;
    pmap_itr = rhs.pmap_itr;
    if (next_itr)
        delete next_itr;

    if (rhs.next_itr)
        next_itr = new Iterator(rhs.next_itr->owner, rhs.next_itr->elmt_itr, rhs.next_itr->pmap_itr);
    else
        next_itr = nullptr;

    return *this;
}

template <typename Key, typename Element>
typename PriorityMap<Key, Element>::Iterator& PriorityMap<Key, Element>::Iterator::operator++() {
    if (elmt_itr != owner->elements.end()) { // more base elements
        elmt_itr++;

        if (elmt_itr == owner->elements.end()) // next_itr is nullptr
            ++(*this); // moves next_itr to pmap_itr if able
    } else if (next_itr) { // more elements in next pmap
        ++(*next_itr);
        if (next_itr->done()) {
            delete next_itr;
            next_itr = nullptr;
            ++(*this);
        }
    } else if (pmap_itr != owner->pmaps.end()) { // more pmaps
        if (base)
            base = false;
        else
            ++pmap_itr;

        if (pmap_itr != owner->pmaps.end()) { // more elements in pmaps
            auto next_pmap = pmap_itr->second;
            auto next_elmt_itr = pmap_itr->second->elements.begin();
            auto next_pmap_itr = pmap_itr->second->pmaps.begin();
            next_itr = new Iterator(next_pmap, next_elmt_itr, next_pmap_itr);
        }
    }

    return *this;
}

template <typename Key, typename Element>
bool PriorityMap<Key, Element>::Iterator::operator==(Iterator rhs) const {
    if ((next_itr && !rhs.next_itr) || (!next_itr && rhs.next_itr)) {
        return false;
    } else if (next_itr && *next_itr != *(rhs.next_itr)) {
        return false;
    }

    return elmt_itr == rhs.elmt_itr && pmap_itr == rhs.pmap_itr;
}

template <typename Key, typename Element>
bool PriorityMap<Key, Element>::Iterator::operator!=(Iterator rhs) const {
    if (elmt_itr != rhs.elmt_itr || pmap_itr != rhs.pmap_itr) {
        return true;
    } else if ((next_itr && !rhs.next_itr) || (!next_itr && rhs.next_itr)) {
        return true;
    }

    return next_itr ? *next_itr != *(rhs.next_itr) : false;
}

template <typename Key, typename Element>
bool PriorityMap<Key, Element>::Iterator::done() {
    bool next_done = next_itr ? next_itr->done() : true;
    return elmt_itr == owner->elements.end() && pmap_itr == owner->pmaps.end() && next_done;
}

#endif