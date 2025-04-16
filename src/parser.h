#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "llvm/Support/raw_ostream.h"

class Parser {
    Lexer &lex;
    Token tok;
    bool state;

    void error() {
        state = false;
        llvm::errs() << "Unexpected: " << tok.getText() << "\n";
    }

    void advance() {
        lex.next(tok);
    }

    bool expect(Token::TokenKind kind) {
        if(tok.is(kind)) {
            return false;
        }
        error();
        return true;
    }

    bool consume(Token::TokenKind kind) {
        if(expect(kind)) {
            return true;
        }
        advance();
        return false;
    }

    AST *parseCalc();
    Expr *parseExpr();
    Expr *parseTerm();
    Expr *parseFactor();

    public:
        Parser(Lexer &lexer) : lex(lexer), state(true) {
            advance();
        }

        bool hasError() {
            return !state;
        }

        AST *parse();
};

#endif