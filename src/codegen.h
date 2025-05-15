#pragma once
#include <stdio.h>
#include "parser.h"

void codegen_init(FILE *outfile);
void codegen_write_and_deinit();
void codegen_generate(Node* node);
