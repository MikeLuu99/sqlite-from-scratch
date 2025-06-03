#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

typedef enum {
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNIZED,
  PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum {
  STATEMENT_SELECT,
  STATEMENT_INSERT
} StatementType;

typedef struct {
  uint32_t Id;
  char username[COLUMN_USERNAME_SIZE];
  char email[COLUMN_EMAIL_SIZE];

} Row;

typedef struct {
  StatementType type;
  Row row_to_insert;
} Statement;

void close_input_buffer(InputBuffer *input_buffer) {
  free(input_buffer->buffer);
  free(input_buffer);
}

InputBuffer *new_input_buffer() {
  InputBuffer *input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

void print_prompt() {
  printf("db > ");
}

ssize_t getline(char** lineptr, size_t* n, FILE* stream);

void read_input(InputBuffer* input_buffer) {
  ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("Error reading input \n");
    exit(EXIT_FAILURE);
  }

  // Asign the size of the buffer and the null terminator
  input_buffer->buffer_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    close_input_buffer(input_buffer);
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED;
  }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    int arg_assigned = scanf(input_buffer->buffer, "insert %u %s %s", &statement->row_to_insert.Id, statement->row_to_insert.username, statement->row_to_insert.email);

    if (arg_assigned != 3) {
      return PREPARE_SYNTAX_ERROR;
    }
    statement->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }
  return PREPARE_UNRECOGNIZED;
}

void execute_statement(Statement* statement) {
  switch (statement->type) {
    case(STATEMENT_INSERT) :
      printf("We do an insert here\n");
      break;
    case(STATEMENT_SELECT) :
      printf("We do a select here\n");
      break;
  }
}

int main (int argc, char* argv[]) {
  InputBuffer* input_buffer = new_input_buffer();
  while (true) {
    print_prompt();
    read_input(input_buffer);

    if (input_buffer->buffer[0] == '.') {
      switch (do_meta_command(input_buffer)) {
        case (META_COMMAND_SUCCESS) :
          continue;
        case (META_COMMAND_UNRECOGNIZED) :
          printf("Unrecognized command: '%s'\n", input_buffer->buffer);
          continue;
      }
    }

    Statement statement;
    switch (prepare_statement(input_buffer, &statement)) {
      case (PREPARE_SUCCESS) :
        break;
      case (PREPARE_SYNTAX_ERROR) :
        printf("Syntax error. Could not parse statement: '%s'\n", input_buffer->buffer);
        continue;
      case (PREPARE_UNRECOGNIZED) :
        printf("Unrecognized keyword at start of: '%s'\n", input_buffer->buffer);
        continue;
    }

    execute_statement(&statement);
    printf("Executed.\n");
  }
}
