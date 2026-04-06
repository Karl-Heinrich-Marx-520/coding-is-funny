struct TupleHash{
    template<typename T>
    static void hash_combine(size_t& seed, const T& v){
        seed ^= hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename Tuple, size_t Index = 0>
    static void hash_tuple(size_t& seed, const Tuple& t){
        if constexpr (Index < tuple_size_v<Tuple>) {
            hash_combine(seed, get<Index>(t));
            hash_tuple<Tuple, Index+1>(seed, t);
        }
    }

    template<typename... Ts>
    size_t operator()(const tuple<Ts...>& t) const{
        size_t seed = 0;
        hash_tuple(seed, t);
        return seed;
    }
}
