#!/usr/bin/env python3
import sys
statementTypes = [
    {
        "name":"function_declaration",
        "arguments": [
            {"name": "function_name", "type": "char*", "free": False},
            {"name": "parameters", "type": "char**", "free": "simple"},
            {"name": "parameter_count", "type": "int", "free": False},
            {"name": "body", "type": "Statement**", "free": "call_array"},
            {"name": "body_count", "type": "int", "free": False},
            {"name": "return_type", "type": "char*", "free": False},
        ]
    },
    {
        "name":"return_statement",
        "arguments": [
            {"name": "expression", "type": "int", "free": False},
        ]
    }
]
expressionTypes = [
    {
        "name":"primary_expression",
        "arguments": [
            {"name": "value", "type": "int", "free": False},
        ]
    }
];



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


def generate_definitions(types, name):
    print("typedef enum {")
    for t in types:
        print(f"  {snake2uppersnake(t["name"])},")
    print(f"}} {snake2pascal(name)}Type;")
    print("")

    print(f"typedef struct {snake2pascal(name)} {snake2pascal(name)};");
    print("")
    for t in types:
        print(f"struct {snake2pascal(t['name'])} {{")
        for argument in t["arguments"]:
            print(f"  {argument['type']} {snake2camel(argument['name'])};")
        print("};")
        print("")

    print(f"struct {snake2pascal(name)}{{")
    print(f"  {snake2pascal(name)}Type type;")
    print("  union {")
    for t in types:
        print(f"    struct {snake2pascal(t['name'])} {snake2camel(t['name'])};")
    print("  } as;")
    print("};")

def generate_function_declaration(types, name):
    print(f"void free{snake2pascal(name)}({snake2pascal(name)}* ast);")

def generate_function_implementation(types, name):

    # free
    for t in types:
        print(f"void free{snake2pascal(t['name'])}({snake2pascal(name)}* ast) {{")
        for argument in t["arguments"]:
            if argument["free"] == "simple":
                print(f"  free(ast->as.{snake2camel(t["name"])}.{snake2camel(argument['name'])});")
            elif argument["free"] == "call_array":
                print(f"  for(int i = 0; i < ast->as.{snake2camel(t['name'])}.{snake2camel(argument['name'])}Count; i++) {{")
                print(f"    free{snake2pascal(t['name'])}(ast->as.{snake2camel(t['name'])}.{snake2camel(argument['name'])}[i]);")
                print(f"  }}")
                print(f"  free(ast->as.{snake2camel(t['name'])}.{snake2camel(argument['name'])});")


        print("}")

    print(f"void free{snake2pascal(name)}({snake2pascal(name)}* ast) {{")
    print("  switch(ast->type) {")
    for t in types:
        # TODO: don't call any function if there are no arguments to free 
        print(f"    case {snake2uppersnake(t['name'])}:")
        print(f"      free{snake2pascal(t['name'])}(ast);")
        print("      break;")
    print("  }")
    print("  free(ast);")
    print("}")
    # ================================

def generate_header():
    print("#pragma once")
    print("")
    # generate statement
    print("// statement")
    generate_definitions(statementTypes, "statement");
    print("")
    print("")
    print("// Expression")
    generate_definitions(expressionTypes, "expression");
    print("")
    print("")

    generate_function_declaration(statementTypes, "statement")
    print("")
    generate_function_declaration(expressionTypes, "expression")
def generate_source():
    print("#include \"ast.h\"")
    print("#include <stdlib.h>")
    print("")
    generate_function_implementation(statementTypes, "statement")
    print("")
    generate_function_implementation(expressionTypes, "expression")

if sys.argv[1] == "header":
    generate_header()
elif sys.argv[1] == "source":
    generate_source()
else:
    print("Usage: python3 ast_generator.py [header or source]")
    exit(1)
