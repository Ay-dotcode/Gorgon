#pragma once
#include <string>
#include <vector>

enum Operation {
  FunctionCall,
  Assignment
};

enum ValueType {
  Temp,
  Literal,
  Variable,
  Constant,
};

struct Value {
  ValueType Type;
  std::string name;
}

struct OperationBase {
  Operation type;
  Value name;
  bool store;
  union {
    Value value;
    std::vector<Value> parameters;
  };
}

