#!/usr/bin/env python3
import sys
statementTypes = [
    {
        "name":"function_declaration",
        "arguments": [
            {"name": "function_name", "type": "char*", "free": False},
            {"name": "parameters", "type": "char**", "free": "simple"},
            {"name": "parameters_count", "type": "int", "free": False},
            {"name": "body", "type": "Statement**", "free": "call_array"},
            {"name": "body_count", "type": "int", "free": False},
            {"name": "return_type", "type": "char*", "free": False},
        ]
    },
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
    }
];


types = [
    {"name": "statement", "definition": statementTypes},
    {"name": "expression", "definition": expressionTypes},
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


def generate_function_implementation(d, name):

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
def generate_header():
    print("#pragma once")
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
