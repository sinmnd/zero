#include "lexer.h"

namespace CharInfo {
    LLVM_READNONE inline bool isSpace(char c) {
        return c == ' ' || c == '\t' || c == '\v' || 
        c == '\r' || c == '\n' || c == '\f';
    }

    LLVM_READNONE inline bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    LLVM_READNONE inline bool isLetter(char c) {
        return (c >= 'a' && c <='z') || (c >= 'A' && c <= 'Z');
    }
}

void Lexer::next(Token &token) {
    while(*bufferPtr && CharInfo::isSpace(*bufferPtr))
        ++bufferPtr;
    if(!*bufferPtr) {
        token.kind = Token::eof;
        return;
    }
    if(CharInfo::isLetter(*bufferPtr)) {
        const char *end = bufferPtr + 1;
        while(CharInfo::isLetter(*end) )
            ++end;
        llvm::StringRef ident(bufferPtr, end - bufferPtr);
        Token::TokenKind kind = (ident == "with" ? Token::KW_with : Token::ident);
        formToken(token, end, kind);
        return;
    } 
    else if(CharInfo::isDigit(*bufferPtr)) {
        const char *end = bufferPtr + 1;
        while(CharInfo::isDigit(*end) )
            ++end;
        formToken(token, end, Token::number);
        return;
    }
    else {
        switch(*bufferPtr) {
            case ',': formToken(token, bufferPtr + 1, Token::comma); break;
            case ':': formToken(token, bufferPtr + 1, Token::colon); break;
            case '+': formToken(token, bufferPtr + 1, Token::plus); break;
            case '-': formToken(token, bufferPtr + 1, Token::minus); break;
            case '*': formToken(token, bufferPtr + 1, Token::star); break;
            case '/': formToken(token, bufferPtr + 1, Token::slash); break;
            case '(': formToken(token, bufferPtr + 1, Token::l_paren); break;
            case ')': formToken(token, bufferPtr + 1, Token::r_paren); break;
            default: formToken(token, bufferPtr + 1, Token::unknown); break;
            return;
        }
    }
}

void Lexer::formToken(Token &tok, const char *tokEnd, Token::TokenKind kind) {
    tok.kind = kind;
    tok.text = llvm::StringRef(bufferPtr, tokEnd - bufferPtr);
    bufferPtr = tokEnd;
}