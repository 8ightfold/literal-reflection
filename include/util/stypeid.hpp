#ifndef LITERAL_TYPE_HPP_STYPEID_HPP
#define LITERAL_TYPE_HPP_STYPEID_HPP

#include <iostream>
#include <optional>
#include <stdio.h>
#include <string>
#include <typeinfo>

struct demangler {
protected:
    std::optional<std::string> demangle(const char* mangled);
    std::optional<std::string> demangled;
};

#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
std::optional<std::string> demangler::demangle(const char* mangled) {
    int status = 0; std::string demangled;
    char* tmp = abi::__cxa_demangle(mangled, 0, 0, &status);
    switch(status) {
        case  0: demangled = std::string(tmp); free(tmp);                 return demangled;
        case -1: printf("Memory allocation failed for %s.\n", mangled);   return {};
        case -2: printf("%s is an invalid name under ABI.\n", mangled);   return {};
        default: printf("Fatal error occured demangling %s.\n", mangled); return {};
    }
}
#else
std::optional<std::string> demangler::demangle(const char* mangled) { return std::string{mangled}; }
#endif

struct stypeid final : demangler {
    template<typename Ty> stypeid(Ty &&ty) { this->demangled = demangle(typeid(ty).name()); };
    std::string name() { return this->demangled.value_or("(null)"); }
    std::string name() const { return this->demangled.value_or("(null)"); }
    friend std::ostream& operator << (std::ostream& os, const stypeid id) { return os << id.name(); }
};

#endif //LITERAL_TYPE_HPP_STYPEID_HPP
