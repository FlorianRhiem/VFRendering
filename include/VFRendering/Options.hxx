#ifndef VECTORFIELDRENDERING_OPTIONS_H
#define VECTORFIELDRENDERING_OPTIONS_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>

namespace VFRendering {
namespace Utilities {

template<typename T>
class Options {
private:
    template<int i>
    struct Type;

public:
    Options();

    template<int index>
    static Options withOption(typename Type<index>::type value);

    template<int index>
    typename Type<index>::type get() const;

    template<int index>
    bool has() const;

    template<int index>
    void set(typename Type<index>::type value);

    template<int index>
    void clear();

    std::vector<int> keys() const;

    std::vector<int> update(const Options<T> &other);

private:
    template<int i>
    struct Option;

    template<int i>
    struct Type {
        typedef decltype(Option<i>().default_value) type;
    };

    struct IStorableOption {
        virtual ~IStorableOption() {}
    };

    template<int i>
    struct StorableOption : public IStorableOption {
        StorableOption(const typename Type<i>::type& value) : m_value(value) {}
        typename Type<i>::type m_value;
    };

    std::unordered_map<int, std::shared_ptr<IStorableOption>> m_options;
};

template<typename T>
Options<T>::Options() {
}


template<typename T>
std::vector<int> Options<T>::keys() const {
    std::vector<int> keys;
    for (const auto& index_option : m_options) {
        keys.push_back(index_option.first);
    }
    return keys;
}

template<typename T>
template<int index>
Options<T> Options<T>::withOption(typename Type<index>::type value) {
    Options<T> options;
    options.set<index>(value);
    return options;
}

template<typename T>
template<int index>
typename Options<T>::template Type<index>::type Options<T>::get() const {
    if (has<index>()) {
        if (auto storableOption = static_cast<StorableOption<index> *>(m_options.at(index).get())) {
            return storableOption->m_value;
        }
    }
    return Option<index>().default_value;
}

template<typename T>
template<int index>
bool Options<T>::has() const {
    return (m_options.find(index) != m_options.end());
}

template<typename T>
template<int index>
void Options<T>::set(typename Type<index>::type value) {
    m_options[index] = std::make_shared<StorableOption<index>>(value);
}

template<typename T>
template<int index>
void Options<T>::clear() {
    m_options.erase(index);
}

template<typename T>
std::vector<int> Options<T>::update(const Options<T> &other) {
    std::vector<int> updatedOptions;
    for (const auto& index_option : other.m_options) {
        const auto& index = index_option.first;
        const auto& option = index_option.second;
        m_options[index] = option;
        updatedOptions.push_back(index);
    }
    return updatedOptions;
}

}
}

#endif
