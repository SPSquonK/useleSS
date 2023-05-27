#pragma once
#include <concepts>
#include <vector>

namespace sqktd {
  /**
   * A simplist Key-Value associative data structure that stores all keys
   * in a contiguous vector and all values in another contiguous vector.
   * Supposed to provide fast access to elements with a given key.
   */
  template<std::integral Key, typename Value>
  struct flatter_map {
  private:
    /** List of keys, ordered */
    std::vector<Key> m_keys;
    /**
     * List of values,
     * ordered such that it is in the same order as it would be in a
     * flat_map<Key, Value >
     */
    std::vector<Value> m_values;

  public:
    /** Clear the container */
    void clear() {
      m_keys.clear();
      m_values.clear();
    }

    /** Return the size of the container */
    [[nodiscard]] size_t size() const {
      return m_keys.size();
    }

    /** Emplace a new value with the given key. */
    template<typename ... Args>
    bool emplace(Key key, Args && ... args) {
      size_t i = 0;
      while (i < m_keys.size()) {
        if (m_keys[i] < key) {
          ++i;
        } else if (m_keys[i] == key) {
          return false;
        } else {
          break;
        }
      }

      m_keys.insert(m_keys.begin() + i, key);
      m_values.emplace(m_values.begin() + i, std::forward(args) ...);

      return true;
    }

    /*
     * If an element is stored at position key, returns a pointer to it.
     * Else returns nullptr.
    */
    [[nodiscard]] const Value * get_at(Key key) const {
      const auto it = std::binary_search(m_keys.begin(), m_keys.end(), key);
      if (it == m_keys.end()) return nullptr;
      return &*(m_values.begin() + (it - m_keys.begin()));
    }
  };
}
