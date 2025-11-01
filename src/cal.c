#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Configuration Constants ---
#define USER_NAME "john"
#define USER_LASTNAME "doe"
#define USER_STUDENTID "98765"
#define MAX_PATH 1024
#define FLOAT_EPSILON 1e-9

// --- Type System ---

typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_INVALID } ValueType;

typedef struct {
  ValueType type;
  union {
    long long i_val;
    double f_val;
  } data;
} Value;

// --- Tokenization (Lexer) ---

typedef enum {
  TOKEN_EOF,
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_POWER,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_UNKNOWN
} TokenType;

typedef struct {
  TokenType type;
  Value value;
  size_t pos;  // 1-based starting position
} Token;

// --- Parsing Context ---

typedef struct {
  const char* content;
  size_t content_len;
  size_t current_char_pos;  // 1-based index (for char being read next)
  size_t error_pos;
  Token current_token;
} Context;

// --- Function Prototypes for Recursive Descent Parser (Highest to Lowest
// Precedence) ---
Value parse_expression(Context* ctx);
Value parse_term(Context* ctx);
Value parse_power(Context* ctx);
Value parse_unary(Context* ctx);
Value parse_primary(Context* ctx);

// --- Centralized Error Handling & Utilities ---

Value set_error(Context* ctx, size_t pos, const char* message) {
  if (ctx->error_pos == 0) {
    ctx->error_pos = pos;
    fprintf(stderr, "Error at pos %zu: %s\n", pos, message);
  }
  Value err = {.type = TYPE_INVALID};
  return err;
}

int is_integral(double f) { return fabs(f - (long long)f) < FLOAT_EPSILON; }

Value promote_to(Value val, ValueType target_type) {
  if (val.type == target_type) return val;
  if (val.type == TYPE_INT && target_type == TYPE_FLOAT) {
    return (Value){.type = TYPE_FLOAT, .data.f_val = (double)val.data.i_val};
  }
  return set_error(NULL, 0, "Internal type error.");
}

Value apply_op(Context* ctx, Value left, TokenType op, Value right) {
  if (left.type == TYPE_INVALID || right.type == TYPE_INVALID) return left;

  ValueType result_type = (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT)
                              ? TYPE_FLOAT
                              : TYPE_INT;
  left = promote_to(left, result_type);
  right = promote_to(right, result_type);

  Value result = {.type = result_type};
  size_t op_pos = ctx->current_token.pos;

  if (result_type == TYPE_INT) {
    long long l = left.data.i_val;
    long long r = right.data.i_val;

    if ((op == TOKEN_SLASH || op == TOKEN_POWER) && r == 0) {
      return set_error(ctx, op_pos, "Integer division by zero.");
    }

    switch (op) {
      case TOKEN_PLUS:
        result.data.i_val = l + r;
        break;
      case TOKEN_MINUS:
        result.data.i_val = l - r;
        break;
      case TOKEN_STAR:
        result.data.i_val = l * r;
        break;
      case TOKEN_SLASH:
        result.data.i_val = l / r;
        break;
      case TOKEN_POWER:
        result.type = TYPE_FLOAT;
        result.data.f_val = pow((double)l, (double)r);
        break;
      default:
        return set_error(ctx, op_pos, "Unknown operator.");
    }
  } else {  // TYPE_FLOAT
    double l = left.data.f_val;
    double r = right.data.f_val;

    if (op == TOKEN_SLASH && fabs(r) < FLOAT_EPSILON) {
      return set_error(ctx, op_pos, "Float division by zero.");
    }

    switch (op) {
      case TOKEN_PLUS:
        result.data.f_val = l + r;
        break;
      case TOKEN_MINUS:
        result.data.f_val = l - r;
        break;
      case TOKEN_STAR:
        result.data.f_val = l * r;
        break;
      case TOKEN_SLASH:
        result.data.f_val = l / r;
        break;
      case TOKEN_POWER:
        result.data.f_val = pow(l, r);
        break;
      default:
        return set_error(ctx, op_pos, "Unknown operator.");
    }
  }
  return result;
}

// --- Lexer Implementation ---

void get_next_token(Context* ctx) {
  const char* expr = ctx->content;
  size_t start_pos;

  // Skip whitespace and comments
  while (1) {
    start_pos = ctx->current_char_pos;
    if (start_pos > ctx->content_len) {
      ctx->current_token.type = TOKEN_EOF;
      ctx->current_token.pos = ctx->content_len + 1;
      return;
    }

    char current_char = expr[start_pos - 1];

    if (isspace((unsigned char)current_char)) {
      ctx->current_char_pos++;
    } else if (current_char == '#') {
      while (start_pos <= ctx->content_len && expr[start_pos - 1] != '\n') {
        start_pos++;
        ctx->current_char_pos++;
      }
    } else {
      break;
    }
  }

  start_pos = ctx->current_char_pos;
  ctx->current_token.pos = start_pos;
  char start_char = expr[start_pos - 1];

  // Operators and Parentheses
  switch (start_char) {
    case '+':
      ctx->current_token.type = TOKEN_PLUS;
      ctx->current_char_pos++;
      return;
    case '-':
      ctx->current_token.type = TOKEN_MINUS;
      ctx->current_char_pos++;
      return;
    case '*':
      if (start_pos < ctx->content_len && expr[start_pos] == '*') {
        ctx->current_token.type = TOKEN_POWER;
        ctx->current_char_pos += 2;
        return;
      }
      ctx->current_token.type = TOKEN_STAR;
      ctx->current_char_pos++;
      return;
    case '/':
      ctx->current_token.type = TOKEN_SLASH;
      ctx->current_char_pos++;
      return;
    case '(':
      ctx->current_token.type = TOKEN_LPAREN;
      ctx->current_char_pos++;
      return;
    case ')':
      ctx->current_token.type = TOKEN_RPAREN;
      ctx->current_char_pos++;
      return;
  }

  // Numbers
  if (isdigit((unsigned char)start_char) || start_char == '.') {
    const char* start_ptr = expr + start_pos - 1;
    char* end_ptr;
    double f_val = strtod(start_ptr, &end_ptr);

    if (end_ptr > start_ptr) {
      ctx->current_char_pos = (size_t)(end_ptr - expr) + 1;

      int is_float_literal =
          (strchr(start_ptr, '.') != NULL || strchr(start_ptr, 'e') != NULL ||
           strchr(start_ptr, 'E') != NULL);

      if (is_float_literal) {
        ctx->current_token.type = TOKEN_NUMBER;
        ctx->current_token.value.type = TYPE_FLOAT;
        ctx->current_token.value.data.f_val = f_val;
      } else {
        long long i_val;
        if (sscanf(start_ptr, "%lld", &i_val) == 1) {
          ctx->current_token.type = TOKEN_NUMBER;
          ctx->current_token.value.type = TYPE_INT;
          ctx->current_token.value.data.i_val = i_val;
        } else {
          ctx->current_token.type = TOKEN_NUMBER;
          ctx->current_token.value.type = TYPE_FLOAT;
          ctx->current_token.value.data.f_val = f_val;
        }
      }
      return;
    }
  }

  // Unknown character
  ctx->current_token.type = TOKEN_UNKNOWN;
  set_error(ctx, start_pos, "Unknown token found.");
  return;
}

int consume(Context* ctx, TokenType expected_type) {
  if (ctx->current_token.type == expected_type) {
    get_next_token(ctx);
    return 1;
  }
  if (ctx->error_pos == 0) {
    set_error(ctx, ctx->current_token.pos,
              "Unexpected token or missing operand/operator.");
  }
  return 0;
}

// --- Recursive Descent Parser ---

// Primary -> Number | ( Expression )
Value parse_primary(Context* ctx) {
  if (ctx->current_token.type == TOKEN_NUMBER) {
    Value result = ctx->current_token.value;
    get_next_token(ctx);
    return result;
  }

  if (ctx->current_token.type == TOKEN_LPAREN) {
    consume(ctx, TOKEN_LPAREN);
    Value result = parse_expression(ctx);
    if (result.type == TYPE_INVALID) return result;
    if (!consume(ctx, TOKEN_RPAREN))
      return set_error(ctx, ctx->error_pos, "Mismatched parenthesis.");
    return result;
  }

  return set_error(ctx, ctx->current_token.pos,
                   "Expected number or opening parenthesis.");
}

// Unary -> (+ | -)? Primary
Value parse_unary(Context* ctx) {
  if (ctx->current_token.type == TOKEN_PLUS ||
      ctx->current_token.type == TOKEN_MINUS) {
    TokenType op = ctx->current_token.type;
    consume(ctx, op);
    Value result = parse_primary(ctx);
    if (result.type == TYPE_INVALID) return result;

    if (op == TOKEN_MINUS) {
      if (result.type == TYPE_INT)
        result.data.i_val = -result.data.i_val;
      else
        result.data.f_val = -result.data.f_val;
    }
    return result;
  }
  return parse_primary(ctx);
}

// Power -> Unary { ** Power }? (Right-associative)
Value parse_power(Context* ctx) {
  Value left = parse_unary(ctx);
  if (left.type == TYPE_INVALID) return left;

  if (ctx->current_token.type == TOKEN_POWER) {
    Token op_token = ctx->current_token;
    consume(ctx, TOKEN_POWER);

    Value right = parse_power(ctx);  // Recursive call for right-associativity
    if (right.type == TYPE_INVALID) return right;

    return apply_op(ctx, left, op_token.type, right);
  }

  return left;
}

// Term -> Power { (* | /) Power }* (Left-associative)
Value parse_term(Context* ctx) {
  Value left = parse_power(ctx);
  if (left.type == TYPE_INVALID) return left;

  while (ctx->current_token.type == TOKEN_STAR ||
         ctx->current_token.type == TOKEN_SLASH) {
    Token op_token = ctx->current_token;
    consume(ctx, op_token.type);

    Value right = parse_power(ctx);
    if (right.type == TYPE_INVALID) return right;

    left = apply_op(ctx, left, op_token.type, right);
    if (left.type == TYPE_INVALID) return left;
  }

  return left;
}

// Expression -> Term { (+ | -) Term }* (Left-associative)
Value parse_expression(Context* ctx) {
  Value left = parse_term(ctx);
  if (left.type == TYPE_INVALID) return left;

  while (ctx->current_token.type == TOKEN_PLUS ||
         ctx->current_token.type == TOKEN_MINUS) {
    Token op_token = ctx->current_token;
    consume(ctx, op_token.type);

    Value right = parse_term(ctx);
    if (right.type == TYPE_INVALID) return right;

    left = apply_op(ctx, left, op_token.type, right);
    if (left.type == TYPE_INVALID) return left;
  }

  return left;
}

// --- Main Execution Logic ---

char* read_file(const char* filepath, size_t* out_len) {
  FILE* file = fopen(filepath, "r");
  if (!file) {
    perror("Error opening input file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);
  *out_len = 0;
  if (file_size <= 0) {
    fclose(file);
    return strdup("");
  }

  char* content = (char*)malloc(file_size + 1);
  if (!content) {
    perror("Memory allocation failed");
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(content, 1, file_size, file);
  content[bytes_read] = '\0';
  *out_len = bytes_read;
  fclose(file);
  return content;
}

void create_output_filepath(const char* input_filepath, char* buffer,
                            size_t buffer_size) {
  const char* filename_start = strrchr(input_filepath, '/');
  if (!filename_start) filename_start = strrchr(input_filepath, '\\');
  if (!filename_start)
    filename_start = input_filepath;
  else
    filename_start++;

  char* ext_start = strrchr(filename_start, '.');
  size_t base_len =
      ext_start ? (size_t)(ext_start - filename_start) : strlen(filename_start);

  snprintf(buffer, buffer_size, "%.*s_%s_%s_%s.txt", (int)base_len,
           filename_start, USER_NAME, USER_LASTNAME, USER_STUDENTID);
}

int write_result_file(const char* filepath, const Value result,
                      size_t error_pos) {
  FILE* file = fopen(filepath, "w");
  if (!file) {
    perror("Error opening output file");
    return -1;
  }

  char output_content[256];

  if (error_pos != 0 || result.type == TYPE_INVALID) {
    snprintf(output_content, sizeof(output_content), "ERROR:%zu", error_pos);
  } else {
    if (result.type == TYPE_INT) {
      snprintf(output_content, sizeof(output_content), "%lld",
               result.data.i_val);
    } else if (result.type == TYPE_FLOAT) {
      if (is_integral(result.data.f_val)) {
        snprintf(output_content, sizeof(output_content), "%lld",
                 (long long)result.data.f_val);
      } else {
        snprintf(output_content, sizeof(output_content), "%.15g",
                 result.data.f_val);
      }
    }
  }

  if (fprintf(file, "%s\n", output_content) < 0) {
    fclose(file);
    return -1;
  }
  fclose(file);
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file.txt>\n", argv[0]);
    return 1;
  }

  char* input_file = argv[1];
  size_t content_len;
  char* expression = read_file(input_file, &content_len);
  if (!expression) return 1;

  Context ctx = {.content = expression,
                 .content_len = content_len,
                 .current_char_pos = 1,
                 .error_pos = 0};
  get_next_token(&ctx);

  Value result = parse_expression(&ctx);

  if (ctx.error_pos == 0 && ctx.current_token.type != TOKEN_EOF) {
    set_error(&ctx, ctx.current_token.pos,
              "Unexpected tokens after expression end.");
  }

  char output_filepath[MAX_PATH];
  create_output_filepath(input_file, output_filepath, sizeof(output_filepath));

  int status = write_result_file(output_filepath, result, ctx.error_pos);

  if (status == 0) {
    printf("Processed '%s'. Output written to '%s'.\n", input_file,
           output_filepath);
  } else {
    fprintf(stderr, "Failed to write output for '%s'.\n", input_file);
  }

  free(expression);
  return (ctx.error_pos == 0 && status == 0) ? 0 : 1;
}