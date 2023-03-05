#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>

template <class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
  using Pair = std::pair<const KeyType, ValueType>;
  struct HashMapValue {
    Pair key_value;
    int64_t psl = -1;

    Pair &pair() { return key_value; }
    const Pair &pair() const { return key_value; }
    const bool operator==(const HashMapValue &other) {
      return (std::tie(key_value, psl) == std::tie(other.key_value, other.psl));
    }
    bool operator!=(const HashMapValue &other) {
      return (std::tie(key_value, psl) != std::tie(other.key_value, other.psl));
    }
    std::pair<KeyType &, ValueType &> ref() {
      return std::pair<KeyType &, ValueType &>(
          const_cast<KeyType &>(key_value.first), key_value.second);
    }
    HashMapValue &operator=(const HashMapValue &other) {
      ref().first = other.key_value.first;
      ref().second = other.key_value.second;
      psl = other.psl;
      return *this;
    }
  };

public:
  class iterator {
  public:
    explicit iterator(const typename std::vector<HashMapValue>::iterator &it) {
      _pointer = it;
    }
    iterator() : _pointer(nullptr) {}
    Pair &operator*() { return _pointer->pair(); }
    Pair *operator->() { return &_pointer->pair(); }

    iterator &operator=(iterator other) {
      _pointer = other._pointer;
      return *this;
    }

    iterator &operator++() {
      for (++_pointer;; ++_pointer) {
        if (_pointer->psl != -1) {
          return *this;
        }
      }
    }
    iterator operator++(int) {
      iterator temp = *this;
      for (++_pointer;; ++_pointer) {
        if (_pointer->psl != -1) {
          break;
        }
      }
      return temp;
    };

    size_t operator-(const iterator &other) const {
      return _pointer - other._pointer;
    }

    bool operator==(const iterator &other) const {
      return _pointer == other._pointer;
    };
    bool operator!=(const iterator &other) const {
      return _pointer != other._pointer;
    }

  private:
    typename std::vector<HashMapValue>::iterator _pointer;
  };

  class const_iterator {
  public:
    explicit const_iterator(
        const typename std::vector<HashMapValue>::const_iterator &it) {
      _pointer = it;
    }
    const_iterator() : _pointer(nullptr) {}
    const Pair &operator*() { return _pointer->pair(); }
    const Pair *operator->() { return &_pointer->pair(); }

    const_iterator &operator=(const_iterator other) {
      _pointer = other._pointer;
      return *this;
    }

    const_iterator &operator++() {
      for (++_pointer;; ++_pointer) {
        if (_pointer->psl != -1) {
          return *this;
        }
      }
    }
    const_iterator operator++(int) {
      const_iterator temp = *this;
      for (++_pointer;; ++_pointer) {
        if (_pointer->psl != -1) {
          break;
        }
      }
      return temp;
    };

    size_t operator-(const const_iterator &other) const {
      return _pointer - other._pointer;
    }

    bool operator==(const const_iterator &other) const {
      return _pointer == other._pointer;
    };
    bool operator!=(const const_iterator &other) const {
      return _pointer != other._pointer;
    }

  private:
    typename std::vector<HashMapValue>::const_iterator _pointer;
  };

  void init(size_t size) {
    clear();
    while (MOD - 1 <= size * 2) {
      MOD = (MOD - 1) * 2 + 1;
    }
    while (size * 4 < MOD - 1) {
      MOD = (MOD - 1) / 2 + 1;
    }
    if (MOD == 1) {
      MOD = 2;
    }
    _size = 0;
    _hash_map.assign(MOD, HashMapValue());
    _hash_map.back().psl = -2;
  }

  HashMap(Hash hash_func = Hash()) : _hash_func(hash_func) { init(2); }
  template <typename Iterator>
  HashMap(Iterator begin, Iterator end, Hash hash_func = Hash())
      : _hash_func(hash_func) {
    init(2);
    for (; begin != end; ++begin) {
      insert(*begin);
    }
  }
  HashMap(const std::initializer_list<Pair> &list, Hash hash_func = Hash())
      : _hash_func(hash_func) {
    init(list.size());
    for (const Pair &obj : list) {
      insert(obj);
    }
  }

  void insert(const Pair &value) {
    if (find(value.first) != end()) {
      return;
    }
    if ((_size + 1) * 2 >= (MOD - 1)) {
      std::vector<Pair> values;
      for (const auto &val : _hash_map) {
        if (val.psl != -1 && val.psl != -2) {
          values.push_back(val.key_value);
        }
      }
      values.push_back(value);
      clear();
      init(values.size());
      for (const Pair &obj : values) {
        insert(obj);
      }
    } else {
      ++_size;
      _insert(_hasher(value.first), value);
    }
  }

  iterator end() { return static_cast<iterator>(_hash_map.end() - 1); }
  iterator begin() {
    for (size_t ind = 0; ind < _hash_map.size(); ++ind) {
      if (_hash_map[ind].psl != -1) {
        return static_cast<iterator>(_hash_map.begin() + ind);
      }
    }
    return static_cast<iterator>(_hash_map.end() - 1);
  }
  const_iterator end() const {
    return static_cast<const_iterator>(_hash_map.end() - 1);
  }
  const_iterator begin() const {
    for (size_t ind = 0; ind < _hash_map.size(); ++ind) {
      if (_hash_map[ind].psl != -1) {
        return static_cast<const_iterator>(_hash_map.begin() + ind);
      }
    }
    return static_cast<const_iterator>(_hash_map.end() - 1);
  }

  iterator find(const KeyType &key) {
    size_t from = _hasher(key);
    int64_t psl = 0;
    for (size_t t = from;; t = (t + 1) % (MOD - 1)) {
      if (psl > _hash_map[t].psl) {
        break;
      }
      if (_hash_map[t].psl >= 0 && key == _hash_map[t].key_value.first) {
        return static_cast<iterator>(_hash_map.begin() + t);
      }
      ++psl;
    }
    return end();
  }

  const_iterator find(const KeyType &key) const {
    size_t from = _hasher(key);
    int64_t psl = 0;
    for (size_t t = from;; t = (t + 1) % (MOD - 1)) {
      if (psl > _hash_map[t].psl) {
        break;
      }
      if (_hash_map[t].psl >= 0 && key == _hash_map[t].key_value.first) {
        return static_cast<const_iterator>(_hash_map.begin() + t);
      }
      ++psl;
    }
    return end();
  }

  void erase(const KeyType &key) {
    iterator it = find(key);
    if (it == end()) {
      return;
    }

    size_t pos = it - static_cast<iterator>(_hash_map.begin());
    _hash_map[pos].psl = -1;
    for (size_t ind = pos;; ind = (ind + 1) % (MOD - 1)) {
      size_t next = (ind + 1) % (MOD - 1);
      if (_hash_map[next].psl == -1 || _hash_map[next].psl == 0) {
        break;
      }
      _hash_map[ind].ref() = _hash_map[next].ref();
      _hash_map[ind].psl = _hash_map[next].psl - 1;
      _hash_map[next].psl = -1;
    }

    --_size;
    if (_size * 4 < (MOD - 1)) {
      std::vector<Pair> values;
      for (const auto &val : _hash_map) {
        if (val.psl != -1 && val.psl != -2) {
          values.push_back(val.key_value);
        }
      }
      clear();
      init(values.size());
      for (const Pair &obj : values) {
        insert(obj);
      }
    }
  }

  ValueType &operator[](const KeyType &key) {
    if (find(key) == end()) {
      insert(Pair(key, ValueType()));
    }
    return find(key)->second;
  }

  const ValueType &at(const KeyType &key) const {
    if (find(key) == end()) {
      throw std::out_of_range("key does not exist");
    }
    return find(key)->second;
  }

  size_t size() const { return _size; }
  bool empty() const { return (_size == 0); }
  Hash hash_function() const { return _hash_func; }
  void clear() {
    _hash_map.clear();
    MOD = 2;
    _hash_map.assign(MOD, HashMapValue());
    _size = 0;
  }

private:
  size_t _hasher(const KeyType &key) const {
    return (_hash_func(key)) % (MOD - 1);
  }
  void _insert(const size_t &from, std::pair<KeyType, ValueType> value) {
    size_t where = _hasher(value.first);
    int64_t psl = 0;
    for (size_t ind = where;; ind = (ind + 1) % (MOD - 1)) {
      if (_hash_map[ind].psl == -1) {
        _hash_map[ind].ref().first = value.first;
        _hash_map[ind].ref().second = value.second;
        _hash_map[ind].psl = psl;
        break;
      }
      if (_hash_map[ind].key_value.first == value.first) {
        break;
      }
      if (_hash_map[ind].psl < psl) {
        HashMapValue temp = _hash_map[ind];
        _hash_map[ind].ref().first = value.first;
        _hash_map[ind].ref().second = value.second;
        _hash_map[ind].psl = psl;
        value = temp.key_value;
        psl = temp.psl;
      }
      ++psl;
    }
  }

  Hash _hash_func;
  std::vector<HashMapValue> _hash_map;
  size_t _size;
  size_t MOD = 2;
};
