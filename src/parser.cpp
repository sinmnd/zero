#include "parser.h"

// calc : ("with" ident ("," ident)* ":")? expr;
// expr : term (("+" | "-") term)*;
// term : factor (("*" | "/") factor)*;
// factor : ident | number | "(" expr ")";
// ident : ([a-zA-Z])+;
// number : ([0-9])+;

AST *Parser::parse() {
    AST *res = parseCalc();
    expect(Token::eof);
    return res;
}

// calc : ("with" ident ("," ident)* ":")? expr;
AST *Parser::parseCalc() {
    Expr *E;
    llvm::SmallVector<llvm::StringRef, 8> vars;
    if(tok.is(Token::KW_with)) {
        advance();
        if(expect(Token::ident)) {
            goto _error;
        }
        vars.push_back(tok.getText());
        advance();
        while(tok.is(Token::comma)) {
            advance();
            if(expect(Token::ident)) {
                goto _error;
            }
            vars.push_back(tok.getText());
            advance();
        }
        if(consume(Token::colon)) {
            goto _error;
        }
    }
    E = parseExpr();

    if(vars.empty()) {
        return E;
    }
    else {
        return new WithDecl(vars, E);
    }
    
_error:
    while(!tok.is(Token::eof)) {
        advance();
    }
    return nullptr;

}


// expr : term (("+" | "-") term)*;
Expr *Parser::parseExpr() {
    Expr *left = parseTerm();
    while(tok.isOneOf(Token::plus, Token::minus)) {
        BinaryOp::Operator op = (tok.is(Token::plus)? BinaryOp::Add : BinaryOp::Sub);
        advance();
        Expr *right = parseTerm();
        left = new BinaryOp(op, left, right);
    }

    return left;
}

// term : factor (("*" | "/") factor)*;
Expr *Parser::parseTerm() {
    Expr *left = parseFactor();
    while(tok.isOneOf(Token::star, Token::slash)) {
        BinaryOp::Operator op = (tok.is(Token::star)? BinaryOp::Mul : BinaryOp::Div);
        advance();
        Expr *right = parseFactor();
        left = new BinaryOp(op, left, right);
    }

    return left;
}

// factor : ident | number | "(" expr ")";
Expr *Parser::parseFactor() {
    Expr *res = nullptr;
    switch(tok.getKind()) {
        case Token::number:
            res = new Factor(Factor::Number, tok.getText());
            advance();
            break;
        case Token::ident:
            res = new Factor(Factor::Ident, tok.getText());
            advance();
            break;
        case Token::l_paren:
            advance();
            res = parseExpr();
            if(!consume(Token::r_paren)) {
                break;
            }
        default:
            if(!res) {
                error();
            }
            while(!tok.isOneOf(Token::r_paren, Token::star, Token::slash, Token::plus, Token::minus, Token::eof)) {
                advance();
            }
    }
    return res;
}


// ident : ([a-zA-Z])+;
// number : ([0-9])+;

