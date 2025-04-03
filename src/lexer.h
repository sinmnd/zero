#ifndef LEXER_H
#define LEXER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"

class Lexer;

class Token {
    friend class Lexer;

    public:
        enum TokenKind {
            eof, unknown, ident, number, comma, colon, plus, minus,
            star, slash, l_paren, r_paren, KW_with
        };

    private:
        TokenKind kind;
        llvm::StringRef text;

    public:
        TokenKind getKind() const { return kind; }
        llvm::StringRef getText() const { return text; }
        bool is(TokenKind k) const { return kind == k; }
        bool isOneOf(TokenKind k1, TokenKind k2) const {
            return is(k1) || is(k2);
        }
        template <typename... Args>
        bool isOneOf(TokenKind k1, TokenKind k2, Args... args) const {
            return is(k1) || isOneOf(k2, args...);
        }

};

class Lexer {
    const char *bufferStart;
    const char *bufferPtr;

    public:
        Lexer(const llvm::StringRef &buffer){
            bufferStart = buffer.begin();
            bufferPtr = bufferStart;
        }

        void next(Token &tok);

    private:
        void formToken(Token &tok, const char *tokEnd, Token::TokenKind kind);
};

#endif