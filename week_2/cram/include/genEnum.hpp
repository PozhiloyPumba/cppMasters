#ifndef __HELPERS_GEN_ENUM_HPP__
#define __HELPERS_GEN_ENUM_HPP__

#define GEN_ENUM(e) e,
#define ENUM_TO_STRING(e) case e: return #e;

#define CREATE_ENUM(name, members)                      \
enum class name {                                       \
    members(GEN_ENUM)                                   \
};                                                      \
constexpr static const char *toString(name e) {         \
    using enum name;                                    \
    switch(e) {                                         \
    members(ENUM_TO_STRING)                             \
    default:;                                           \
    }                                                   \
    return "Something went wrong oops....";             \
}

#endif