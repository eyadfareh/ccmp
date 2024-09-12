#!/usr/bin/env python3
import sys


typeTypes = [
    {
        "name": "basic_type",
        "arguments": [
            {"name": "name", "type": "char*", "free": False},
        ]
    },
    {
        "name": "array_type",
        "arguments": [
            {"name": "type", "type": "Type*", "free": "call"},
            {"name": "size", "type": "int", "free": False},
        ]
    },
    {
        "name": "struct_type",
        "arguments": [

        ]
    },
    {
        "name": "function_type",
        "arguments": [
            {"name": "return_type", "type": "Type*", "free": "call"},
            {"name": "parameters", "type": "Type**", "free": "call_array"},
            {"name": "parameters_count", "type": "int", "free": False},
        ]
    },
    {
        "name": "pointer_type",
        "arguments": [
            {"name": "type", "type": "Type*", "free": "call"},
        ]
    },
]
declarationTypes = [
    # Technically not a declaration
    {
        "name":"function_definition",
        "arguments": [
            {"name": "function_name", "type": "char*", "free": False},
            {"name": "parameters", "type": "char**", "free": "simple"},
            {"name": "parameters_count", "type": "int", "free": False},
            {"name": "body", "type": "Statement*", "free": "call"},
            {"name": "return_type", "type": "Type*", "free": "call"},
        ]
    },
    {
        "name":"function_declaration",
        "arguments": [
            {"name": "function_name", "type": "char*", "free": False},
            {"name": "parameters", "type": "char**", "free": "simple"},
            {"name": "parameters_count", "type": "int", "free": False},
            {"name": "return_type", "type": "char*", "free": False},
        ]
    }
]
statementTypes = [
    {
        "name":"return_statement",
        "arguments": [
            {"name": "expression", "type": "Expression*", "free": "call"},
        ]
    },
    {
        "name":"expression_statement",
        "arguments": [
            {"name": "expression", "type": "Expression*", "free": "call"}
        ]
    },
    {
        "name":"compound_statement",
        "arguments": [
            {"name": "statements", "type": "Statement**", "free": "call_array"},
            {"name": "statements_count", "type": "int", "free": False},
        ]
    }
]
expressionTypes = [
    {
        "name":"literal_expression",
        "arguments": [
            {"name": "value", "type": "int", "free": False},
        ]
    },
    {
        "name": "identifier_expression",
        "arguments":[
            {"name":"name", "type": "char*", "free": False}
        ]
    },
    {
        "name":"call_expression",
        "arguments": [
            {"name": "callee", "type": "Expression*", "free": "call"},
            {"name": "parameters", "type": "Expression**", "free": "call_array"},
            {"name": "parameters_count", "type": "int", "free": False},
        ]
    },
    {
        "name":"unary_expression",
        "arguments": [
            {"name": "operator", "type": "TokenType", "free": False},
            {"name": "right", "type": "Expression*", "free": "call"},
        ]
    },
    {
        "name": "postfix_expression",
        "arguments": [
            {"name": "left", "type": "Expression*", "free": "call"},
            {"name": "operator", "type": "TokenType", "free": False},
        ]
    },
    {
        "name": "binary_expression",
        "arguments": [
            {"name": "left", "type": "Expression*", "free": "call"},
            {"name": "operator", "type": "TokenType", "free": False},
            {"name": "right", "type": "Expression*", "free": "call"},
        ]
    },
    {
        "name": "conditional_expression",
        "arguments": [
            {"name": "condition", "type": "Expression*", "free": "call"},
            {"name": "left", "type": "Expression*", "free": "call"},
            {"name": "right", "type": "Expression*", "free": "call"},
        ]
    }
];


types = [
    {"name": "statement", "definition": statementTypes},
    {"name": "expression", "definition": expressionTypes},
    {"name": "declaration", "definition": declarationTypes},
    {"name": "type", "definition": typeTypes},
]

if len(sys.argv) < 2:
    print("Usage: python3 ast_generator.py [header or source]")
    exit(1)


def snake2pascal(name):
    return "".join(x.capitalize() for x in name.lower().split("_"))
def snake2camel(name):
    pascal = snake2pascal(name)
    return pascal[0].lower() + pascal[1:]
def snake2uppersnake(name):
    return name.upper()


def generate_definitions(d, name):
    print("typedef enum {")
    for t in d:
        print(f"  {snake2uppersnake(t["name"])},")
    print(f"}} {snake2pascal(name)}Type;")
    print("")

    print("")
    for t in d:
        print(f"struct {snake2pascal(t['name'])} {{")
        for argument in t["arguments"]:
            print(f"  {argument['type']} {snake2camel(argument['name'])};")
        print("};")
        print("")

    print(f"struct {snake2pascal(name)}{{")
    print(f"  {snake2pascal(name)}Type type;")
    print("  union {")
    for t in d:
        print(f"    struct {snake2pascal(t['name'])} {snake2camel(t['name'])};")
    print("  } as;")
    print("};")
    print("")



    print("typedef struct {")
    print(f"  {snake2pascal(name)}** {snake2camel(name)}s;")
    print("  int size;")
    print("  int capacity;")
    print(f"}} {snake2pascal(name)}List;")


def generate_function_declaration(d, name):
    print(f"void free{snake2pascal(name)}({snake2pascal(name)}* ast);")
    print(f"void add{snake2pascal(name)}({snake2pascal(name)}List* list, {snake2pascal(name)}* e);")
    print(f"{snake2pascal(name)}* createEmpty{snake2pascal(name)}();")
    for t in d:
        print(f"{snake2pascal(name)}* create{snake2pascal(t['name'])}(", end="")
        for argument in t["arguments"]:
            print(f"{argument['type']} {snake2camel(argument['name'])}", end="")
            if argument != t["arguments"][-1]:
                print(", ", end="")

        print(");")
    print("")

    print(f"{snake2pascal(name)}List createEmpty{snake2pascal(name)}List(size_t capacity);")

def generate_function_implementation(d, name):

    # =====================
    # free
    for t in d:
        print(f"void free{snake2pascal(t['name'])}({snake2pascal(name)}* ast) {{")
        for argument in t["arguments"]:
            if argument["free"] == "simple":
                print(f"  free(ast->as.{snake2camel(t["name"])}.{snake2camel(argument['name'])});")
            elif argument["free"] == "call_array":
                print(f"  for(int i = 0; i < ast->as.{snake2camel(t['name'])}.{snake2camel(argument['name'])}Count; i++) {{")
                print(f"    free{snake2pascal(argument["type"][:-2])}(ast->as.{snake2camel(t['name'])}.{snake2camel(argument['name'])}[i]);")
                print(f"  }}")
                print(f"  free(ast->as.{snake2camel(t['name'])}.{snake2camel(argument['name'])});")
            elif argument["free"] == "call":
                print(f"  free{snake2pascal(argument["type"][:-1])}(ast->as.{snake2camel(t['name'])}.{snake2camel(argument['name'])});")


        print("}")

    print(f"void free{snake2pascal(name)}({snake2pascal(name)}* ast) {{")
    print("  switch(ast->type) {")
    for t in d:
        # TODO: don't call any function if there are no arguments to free
        print(f"    case {snake2uppersnake(t['name'])}:")
        print(f"      free{snake2pascal(t['name'])}(ast);")
        print("      break;")
    print("  }")
    print("  free(ast);")
    print("}")
    # ================================
    # Add item to list
    print(f"void add{snake2pascal(name)}({snake2pascal(name)}List* list, {snake2pascal(name)}* e) {{")
    print("  if (list->size == list->capacity) {")
    print("    list->capacity *= 2;")
    print(f"    list->{snake2camel(name)}s = realloc(list->{snake2camel(name)}s, sizeof({snake2pascal(name)}*) * list->capacity);")
    print(f"    if (list->{snake2camel(name)}s == NULL) {{")
    print("      perror(\"realloc\");")
    print("      exit(1);")
    print("    }")
    print("  }")
    print(f"  list->{snake2camel(name)}s[list->size] = e;")
    print("  list->size++;")
    print("}")
    print("")

    # ================================
    # Create empty expression
    print(f"{snake2pascal(name)}* createEmpty{snake2pascal(name)}() {{")
    print(f"  {snake2pascal(name)}* ast = malloc(sizeof({snake2pascal(name)}));")
    print(f"  return ast;")
    print("}")

    # ================================
    # Create type
    for t in d:
        print(f"{snake2pascal(name)}* create{snake2pascal(t['name'])}(", end="")
        for argument in t["arguments"]:
            print(f"{argument['type']} {snake2camel(argument['name'])}", end="")
            if argument != t["arguments"][-1]:
                print(", ", end="")

        print(") {")
        print(f"  {snake2pascal(name)}* ast = createEmpty{snake2pascal(name)}();")
        print(f"  ast->type = {snake2uppersnake(t['name'])};")
        for argument in t["arguments"]:
            print(f"  ast->as.{snake2camel(t['name'])}.{snake2camel(argument['name'])} = {snake2camel(argument['name'])};")
        print("  return ast;")
        print("}")
        print("")
    #============================
    # Create list
    print(f"{snake2pascal(name)}List createEmpty{snake2pascal(name)}List(size_t capacity) {{")
    print(f"  {snake2pascal(name)}List list;")
    print(f"  list.size = 0;")
    print(f"  list.capacity = capacity;")
    print(f"  list.{snake2camel(name)}s = malloc(sizeof({snake2pascal(name)}*) * list.capacity);")
    print(f"  if (list.{snake2camel(name)}s == NULL) {{")
    print("    perror(\"malloc\");")
    print("    exit(1);")
    print("  }")
    print("  return list;")
    print("}")

def generate_header():
    print("#pragma once")
    print("#include \"lexer.h\"")
    print("#include <stdio.h>")
    print("")

    for t in types:
        print(f"typedef struct {snake2pascal(t['name'])} {snake2pascal(t['name'])};")
    print("")
    # generate statement
    for t in types:
        generate_definitions(t["definition"], t["name"])
        print("")
        print("")

    for t in types:
        generate_function_declaration(t["definition"], t["name"])
        print("")
        print("")
def generate_source():
    print("#include \"ast.h\"")
    print("#include <stdlib.h>")
    print("#include <stdio.h>")
    for t in types:
        generate_function_implementation(t["definition"], t["name"])
        print("")

if sys.argv[1] == "header":
    generate_header()
elif sys.argv[1] == "source":
    generate_source()
else:
    print("Usage: python3 ast_generator.py [header or source]")
    exit(1)
