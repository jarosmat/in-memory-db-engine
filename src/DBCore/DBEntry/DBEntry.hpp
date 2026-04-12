#ifndef PROJECT_DBENTRY_HPP
#define PROJECT_DBENTRY_HPP
#include "ValueTypes.hpp"
#include "SwissTable.hpp"
#include "Timepoint.hpp"

enum DBTypes {
    INTEGER,
    FLOAT,
    STRING,
    OBJECT,
    VECTOR_INT,
    VECTOR_FLOAT,
    VECTOR_STRING
};


template<typename E, typename T>
    requires (!std::is_reference_v<E> && !std::is_reference_v<T>)
class DBEntry_t {
    E payload;
    Timepoint timepoint_{};
    T ttl;

public:
    template<typename EC, typename TC>
        requires (std::is_convertible_v<std::remove_reference_t<EC>, E> &&
                  std::is_convertible_v<std::remove_reference_t<TC>, T>)
    explicit DBEntry_t(EC &&payload, TC &&ttl) : payload(std::forward<EC>(payload)), ttl(std::forward<TC>(ttl)) {
    }

    auto GetRef() -> E & {
        return payload;
    }

    template<typename ES>
        requires std::is_convertible_v<std::remove_reference_t<ES>, E>
    auto SetVal(ES &&val) -> void {
        payload = std::forward<ES>(val);
    }

    [[nodiscard]] auto Expired() const -> bool {
        return timepoint_.Elapsed(ttl);
    }
};

template<typename E>
class DBEntry {
public:
    struct HolderBase {
        virtual ~HolderBase() = default;

        virtual auto GetRef() -> E & = 0;

        virtual auto SetVal(std::remove_reference_t<E> &&val) -> void = 0;

        virtual auto SetVal(const std::remove_reference_t<E> &val) -> void = 0;

        virtual bool Expired() = 0;
    };

    template<typename T>
    struct Holder final : public HolderBase {
        explicit Holder(const DBEntry_t<std::remove_reference_t<E>, std::remove_reference_t<T> > &val) : obj(val) {
        }

        explicit Holder(
            DBEntry_t<std::remove_reference_t<E>, std::remove_reference_t<T> > &&val) : obj(std::move(val)) {
        }

        auto GetRef() -> E & override { return obj.GetRef(); }

        auto SetVal(std::remove_reference_t<E> &&val) -> void override { obj.SetVal(std::move(val)); }

        auto SetVal(const std::remove_reference_t<E> &val) -> void override { obj.SetVal(val); }

        auto Expired() -> bool override { return obj.Expired(); }

    private:
        DBEntry_t<std::remove_reference_t<E>, std::remove_reference_t<T> > obj;
    };

    template<typename T>
    explicit DBEntry(const DBEntry_t<std::remove_reference_t<E>, T> &val) : entry(std::make_unique<Holder<T> >(val)) {
    }

    template<typename T>
    explicit DBEntry(DBEntry_t<std::remove_reference_t<E>, T> &&val) : entry(
        std::make_unique<Holder<T> >(std::move(val))) {
    }

    E &GetRef() {
        return entry->GetRef();
    }

    void SetVal(std::remove_reference_t<E> &&val) {
        entry->SetVal(std::move(val));
    }

    void SetVal(const std::remove_reference_t<E> &val) {
        entry->SetVal(val);
    }

    bool Expired() {
        return entry->Expired();
    }
private:
    std::unique_ptr<HolderBase> entry;
};

template<typename E>
    requires (!std::is_reference_v<E>)
class DBEntry_t_im {
    E payload;

public:
    template<typename EC>
        requires (std::is_convertible_v<std::remove_reference_t<EC>, E>)
    explicit DBEntry_t_im(EC &&payload) : payload(std::forward<EC>(payload)) {
    }

    auto GetRef() -> E & {
        return payload;
    }

    template<typename ES>
        requires std::is_convertible_v<std::remove_reference_t<ES>, E>
    auto SetVal(ES &&val) -> void {
        payload = std::forward<ES>(val);
    }
};
#endif //PROJECT_DBENTRY_HPP
