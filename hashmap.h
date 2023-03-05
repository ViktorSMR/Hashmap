#include <functional>
#include <initializer_list>
#include <vector>

template <class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
  using Pair = std::pair<KeyType, ValueType>;
  using ConstPair = std::pair<const KeyType, ValueType>;
  class HashMapValue {
    KeyType key;
    ValueType value;
    size_t psl = -1;
    HashMapValue() {}
    HashMapValue(KeyType key, ValueType value, size_t psl)
        : key(key), value(value), psl(psl) {}
    Pair pair() { return Pair(key, value); }
    ConstPair const_pair() { return ConstPair(key, value); }
  };

public:
  class iterator {
    explicit iterator(HashMapValue *pointer) { _pointer = pointer; }
    iterator() { _pointer = nullptr; }
    Pair &operator*() { return _pointer; }
    // Pair *operator->() { return *(*_pointer); }

    iterator &operator=(iterator other) {
      _pointer = other._pointer;
      return *this;
    }

    iterator &operator++() { return *this; }
    iterator operator++(int);
    iterator &operator--();
    iterator operator--(int);

    bool operator==(const iterator &other) const;
    bool operator!=(const iterator &other) const;

  private:
    HashMapValue *_pointer;
  };

  HashMap(Hash hash_func = Hash()) {
    _hash_func = hash_func;
    MOD = 5;
    _hash_map.assign(MOD, HashMapValue());
  }
  template <typename Iterator>
  HashMap(Iterator begin, Iterator end, Hash hash_func = Hash()) {
    _hash_func = hash_func;
    MOD = _find_nearest_prime((end - begin) * 2);
    _hash_map.assign(MOD, HashMapValue());
    for (; begin != end; ++begin) {
      insert(*begin);
    }
  }
  HashMap(const std::initializer_list<Pair> &list, Hash hash_func = Hash()) {
    _hash_func = hash_func;
    MOD = _find_nearest_prime(list.size() * 2);
    _hash_map.assign(MOD, HashMapValue());
    for (const Pair &obj : list) {
      insert(obj);
    }
  }

  void insert(const Pair &value) {
    if ((_size + 1) * 2 >= MOD) {
      std::vector<Pair> values;
      for (const auto &val : _hash_map) {
        if (val.psl != -1) {
          values.emplace_back(val.key, val.value);
        }
      }
      clear();
      MOD = _find_nearest_prime(values.size() * 2);
      _hash_map.assign(MOD, HashMapValue());
      for (const Pair &obj : values) {
        insert(obj);
      }
    } else {
      ++_size;
      _insert(_hasher(value.first), value);
    }
  }

  void erase(const Pair &value) {}

  size_t size() const { return _size; }
  bool empty() const { return (_size == 0); }
  size_t hash_function(const KeyType &key) { return _hasher(key); }
  void clear() {
    _hash_map.clear();
    MOD = 1;
    _size = 0;
  }

private:
  size_t _hasher(const KeyType &key) { return Hash(key) % MOD; }
  void _insert(const size_t &from, const Pair &value) {
    size_t where = _hasher(value.first);
    for (size_t ind = where;; ind = (ind + 1) % MOD) {
      if (_hash_map[ind].key == value.first) {
        break;
      }
      if (_hash_map[ind].psl < (ind - where)) {
        HashMapValue temp = _hash_map[ind];
        _hash_map[ind] = {value.first, value.second, (ind - where)};
        if (temp.psl != -1) {
          _insert(ind, std::make_pair(temp.key, temp.value));
        }
        break;
      }
    }
  }
  void _erase(const size_t &where, const Pair &value) {}

  Hash _hash_func;
  std::vector<HashMapValue> _hash_map;
  size_t _size;
  size_t MOD;

  bool _check_prime(const size_t &x) {
    for (size_t i = 1; i * i <= x; ++i) {
      if (x % i == 0) {
        return false;
      }
    }
    return true;
  }
  size_t _find_nearest_prime(const size_t &from) {
    for (size_t x = from;; ++x) {
      if (_check_prime(x)) {
        return x;
      }
    }
    return -1; // it will never happen
  }
};
