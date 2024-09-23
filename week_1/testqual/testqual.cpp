#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include "tokens.hpp"
#include "lexer.hpp"

namespace TestQual {

bool testqual(const std::string &a, const std::string &b) {
    auto parseToLexem = [](const auto &str) {
        std::vector<Lexem> tokens;
        std::istringstream inSS(str);
        Lexer l(inSS);
        Lexem token;
        while((token = l.get_next_token()) != Lexem::END_OF_INPUT) {
            tokens.push_back(token);
        }

        return tokens;
    };
    auto checkArrayCorrectPlace = [](const auto &tokens) {
        bool arrayGood = true;
        for (auto it = tokens.rbegin(), end = tokens.rend(); it != end; ++it) {
            switch (*it) {
                case Lexem::ARRAY:  
                    if (!arrayGood)
                        throw std::runtime_error("[] uncorrect");
                    break;
                case Lexem::CONST:
                case Lexem::POINTER:
                case Lexem::CHAR:
                    arrayGood = false;
                    break;
                default:;
            }
        }
    };
    auto cvDecompose = [&checkArrayCorrectPlace](const auto &tokens) {
        int n = std::count(tokens.begin(), tokens.end(), Lexem::POINTER);
        int arrayN = std::count(tokens.begin(), tokens.end(), Lexem::ARRAY);
        checkArrayCorrectPlace(tokens);
        std::vector<bool> haveCV(n+1+arrayN, false);
        auto cvIt = haveCV.begin();
        bool hasChar = false;
        for (auto it = tokens.rbegin(), end = tokens.rend(); it != end; ++it) {
            switch (*it) {
                case Lexem::ARRAY:
                case Lexem::POINTER:    ++cvIt; break;
                case Lexem::CONST:
                    if(*cvIt)
                        throw std::runtime_error("Too many const at one place");
                    *cvIt = true;
                    break;
                case Lexem::CHAR:
                    if(hasChar) throw std::runtime_error("is not a type two char lexems");
                    hasChar = true;
                    break;
                default:;
            }
        }
        if((tokens[0] != Lexem::CHAR) && (tokens[0] != Lexem::CONST || tokens[1] != Lexem::CHAR)) 
            throw std::runtime_error("It is not a const char or char type");
        return std::make_pair(haveCV, arrayN);
    };

    const auto tokenFirst = parseToLexem(a);
    const auto tokenSecond = parseToLexem(b);
    const auto [cvFirst, arrayCountFirst] = cvDecompose(tokenFirst);
    const auto [cvSecond, arrayCountSecond] = cvDecompose(tokenSecond);

    auto testSimilar = [](const auto &first, const auto &second) {
        return first.size() == second.size();
    };
    auto checkConversibleCV = [](const auto &first, const auto &second) {
        auto fst = first.rbegin(), snd = second.rbegin();
        for (auto fstEnd = first.rend(); fst != fstEnd; ++fst, ++snd)
            if (*fst != *snd)
                break;

        if(fst == first.rend()) return true;

        auto sndPrevEnd = second.rbegin();
        std::advance(sndPrevEnd, second.size() - 1);
        while (snd != sndPrevEnd)
            if (!(*snd++))
                return false;
        return true;
    };
    if(arrayCountFirst > 1)
        throw std::runtime_error("Uncorrect first type (Too many empty brackets)");
    if(arrayCountSecond > 1)
        throw std::runtime_error("Uncorrect second type (Too many empty brackets)");

    return  (arrayCountSecond == 0) &&
            testSimilar(cvFirst, cvSecond) && 
            checkConversibleCV(cvFirst, cvSecond);
}
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        throw std::runtime_error("bad command args\n Usage: ./testqual \"first type\" \"second type\"");
    }

    std::cout << std::boolalpha << TestQual::testqual(argv[1], argv[2]) << std::endl;
    return 0;
}