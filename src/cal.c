#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>

#define MAXBUF 1024

// ---------------- Token types ----------------
typedef enum {
    T_NUMBER, T_PLUS, T_MINUS, T_STAR, T_SLASH,
    T_LPAREN, T_RPAREN, T_EOF, T_INVALID
} TokenType;

typedef struct {
    TokenType type;
    double value;
} Token;

// ---------------- Lexer ----------------
typedef struct {
    const char *text;
    size_t pos;
    Token current;
} Lexer;

Token get_next_token(Lexer *lex) {
    const char *t = lex->text;
    while (isspace(t[lex->pos])) lex->pos++;

    char c = t[lex->pos];
    if (c == '\0') return (Token){T_EOF, 0};

    if (isdigit(c)) {
        double val = 0;
        while (isdigit(t[lex->pos])) {
            val = val * 10 + (t[lex->pos] - '0');
            lex->pos++;
        }
        return (Token){T_NUMBER, val};
    }

    lex->pos++;
    switch (c) {
        case '+': return (Token){T_PLUS, 0};
        case '-': return (Token){T_MINUS, 0};
        case '*': return (Token){T_STAR, 0};
        case '/': return (Token){T_SLASH, 0};
        case '(': return (Token){T_LPAREN, 0};
        case ')': return (Token){T_RPAREN, 0};
        default:  return (Token){T_INVALID, 0};
    }
}

// ---------------- Parser ----------------
typedef struct {
    Lexer *lexer;
    Token current;
} Parser;

double parse_expr(Parser *p); // forward declaration

void eat(Parser *p, TokenType type) {
    if (p->current.type == type)
        p->current = get_next_token(p->lexer);
    else {
        fprintf(stderr, "Unexpected token\n");
        exit(1);
    }
}

double parse_factor(Parser *p) {
    Token t = p->current;
    double result = 0;

    if (t.type == T_NUMBER) {
        result = t.value;
        eat(p, T_NUMBER);
    } else if (t.type == T_LPAREN) {
        eat(p, T_LPAREN);
        result = parse_expr(p);
        eat(p, T_RPAREN);
    } else {
        fprintf(stderr, "Syntax error: invalid factor\n");
        exit(1);
    }
    return result;
}

double parse_term(Parser *p) {
    double result = parse_factor(p);

    while (p->current.type == T_STAR || p->current.type == T_SLASH) {
        Token op = p->current;
        if (op.type == T_STAR) {
            eat(p, T_STAR);
            result *= parse_factor(p);
        } else {
            eat(p, T_SLASH);
            double divisor = parse_factor(p);
            if (divisor == 0) {
                fprintf(stderr, "Division by zero\n");
                exit(1);
            }
            result /= divisor;
        }
    }
    return result;
}

double parse_expr(Parser *p) {
    double result = parse_term(p);

    while (p->current.type == T_PLUS || p->current.type == T_MINUS) {
        Token op = p->current;
        if (op.type == T_PLUS) {
            eat(p, T_PLUS);
            result += parse_term(p);
        } else {
            eat(p, T_MINUS);
            result -= parse_term(p);
        }
    }
    return result;
}

// ---------------- Main ----------------
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <inputfile>\n", argv[0]);
        return 1;
    }

    const char *infile = argv[1];
    FILE *f = fopen(infile, "r");
    if (!f) {
        fprintf(stderr, "Cannot open input file: %s\n", infile);
        return 1;
    }

    char buffer[MAXBUF];
    if (!fgets(buffer, sizeof(buffer), f)) {
        fprintf(stderr, "Empty input file\n");
        fclose(f);
        return 1;
    }
    fclose(f);

    // Lexer and parser
    Lexer lexer = {buffer, 0};
    Parser parser = {&lexer, get_next_token(&lexer)};
    double result = parse_expr(&parser);

    // Build output directory and file
    char foldername[128];
    char outpath[256];
    char base[64];

    strncpy(base, infile, sizeof(base));
    base[strcspn(base, ".")] = '\0';

    snprintf(foldername, sizeof(foldername), "%s_Sandeep_241ADB010", base);
    mkdir(foldername, 0777);
    snprintf(outpath, sizeof(outpath), "%s/%s_Sandeep_Garg_241ADB010.txt", foldername, base);

    FILE *outf = fopen(outpath, "w");
    if (!outf) {
        fprintf(stderr, "Cannot create output file: %s\n", outpath);
        return 1;
    }
    fprintf(outf, "%.0f\n", result);
    fclose(outf);

    printf("Output written to: %s\n", outpath);
    return 0;
}
