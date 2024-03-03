#include "CompilerParser.h"

#include <iostream>
using namespace std;

// NOTE: COMPILE WITH g++ -Wall -o CompilerParser.bin Main.cpp
// CompilerParser.cpp ParseTree.cpp Token.cpp

/**
 * Constructor for the CompilerParser
 * @param tokens A linked list of tokens to be parsed
 */
CompilerParser::CompilerParser(std::list<Token*> tokens) {
  this->tokens = tokens;
}

/**
 * Generates a parse tree for a single program
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileProgram() {
  ParseTree* tree = new ParseTree("class", "keyword");
  tree->addChild(mustBe("keyword", "class"));

  Token* currentToken = current();
  if (currentToken->getType() == "identifier") {
    tree->addChild(currentToken);
    next();
  } else {
    throw ParseException();
  }
  tree->addChild(mustBe("symbol", "{"));
  tree->addChild(mustBe("symbol", "}"));

  return tree;
}

/**
 * Generates a parse tree for a single class
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileClass() {
  ParseTree* tree = new ParseTree("class", "keyword");

  tree->addChild(mustBe("keyword", "class"));

  Token* currentToken = current();
  if (currentToken->getType() == "identifier") {
    tree->addChild(currentToken);
    next();
  } else {
    throw ParseException();
  }
  // starting symbol {
  tree->addChild(mustBe("symbol", "{"));

  // field or static variables
  while (have("keyword", "field") || have("keyword", "static")) {
    ParseTree* varDeclaration = compileClassVarDec();
    tree->addChild(varDeclaration);
  }

  // subroutines of a class (functions, constructors, methods)
  while (have("keyword", "function") || have("keyword", "method") ||
         have("keyword", "constructor")) {
    ParseTree* subroutineDeclaration = compileSubroutine();
    tree->addChild(subroutineDeclaration);
  }

  // ending symbol }
  tree->addChild(mustBe("symbol", "}"));
  return tree;
}

/**
 * Generates a parse tree for a static variable declaration or field declaration
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileClassVarDec() {
  ParseTree* tree = new ParseTree("classVarDec", "");

  // field or static variables
  if (have("keyword", "field") || have("keyword", "static")) {
    tree->addChild(current());
    next();
  } else {
    throw ParseException();
  }

  // handling variable types
  if (have("keyword", "boolean") || have("keyword", "char") ||
      have("keyword", "int")) {
    tree->addChild(current());
    next();
  } else {
    throw ParseException();
  }

  // handling matching variable name
  Token* currentToken = current();
  if (currentToken->getType() == "identifier") {
    tree->addChild(currentToken);
    next();
  } else {
    throw ParseException();
  }

  while (have("symbol", ",")) {
    Token* commaSymbol = current();
    next();
    Token* currentToken = current();
    if (currentToken->getType() == "identifier") {
      tree->addChild(commaSymbol);
      tree->addChild(currentToken);
      next();
    } else {
      throw ParseException();
    }
  }
  tree->addChild(mustBe("symbol", ";"));

  return tree;
}

/**
 * Generates a parse tree for a method, function, or constructor
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileSubroutine() {
  ParseTree* tree = new ParseTree("subroutine", "");

  // subroutines of a class (functions, constructors, methods)
  if (have("keyword", "function") || have("keyword", "method") ||
      have("keyword", "constructor")) {
    tree->addChild(current());
    next();
  } else {
    throw ParseException();
  }

  // handling variable types & also void
  Token* currentToken = current();
  if (have("keyword", "boolean") || have("keyword", "char") ||
      have("keyword", "int") || have("keyword", "void") ||
      currentToken->getType() == "identifier") {
    tree->addChild(current());
    next();
  } else {
    throw ParseException();
  }

  // checking for identifier (names)
  currentToken = current();
  if (currentToken->getType() == "identifier") {
    tree->addChild(currentToken);
    next();
  } else {
    throw ParseException();
  }

  // going over list of parameters
  tree->addChild(mustBe("symbol", "("));
  if (have("keyword", "boolean") || have("keyword", "char") ||
      have("keyword", "int")) {
    ParseTree* parameters = compileParameterList();
    tree->addChild(parameters);
  }
  tree->addChild(mustBe("symbol", ")"));

  // body of any subroutines
  if (have("symbol", "{")) {
    ParseTree* subroutines = compileSubroutineBody();
    tree->addChild(subroutines);
  } else {
    throw ParseException();
  }
  return tree;
}

/**
 * Generates a parse tree for a subroutine's parameters
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileParameterList() {
  ParseTree* tree = new ParseTree("parameterList", "");

  // handling variable types
  if (have("keyword", "boolean") || have("keyword", "char") ||
      have("keyword", "int")) {
    tree->addChild(current());
    next();
  } else {
    throw ParseException();
  }

  // checking for identifier (names)
  Token* currentToken = current();
  if (currentToken->getType() == "identifier") {
    tree->addChild(currentToken);
    next();
  } else {
    throw ParseException();
  }

  // handling comma symbol, variable types & void
  while (have("symbol", ",")) {
    Token* commaSymbol = current();
    next();

    Token* varType = current();
    // if any of these keywords present, dataType is true (exists)
    bool dataType = have("keyword", "boolean") || have("keyword", "char") ||
                    have("keyword", "int") || have("keyword", "void") ||
                    varType->getType() == "identifier";
    next();

    // adds the list of parameters
    Token* match = current();
    if (match->getType() == "identifier" && dataType == true) {
      tree->addChild(commaSymbol);
      tree->addChild(varType);
      tree->addChild(match);
      next();
    } else {
      throw ParseException();
    }
  }

  return tree;
}

/**
 * Generates a parse tree for a subroutine's body
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileSubroutineBody() {
  ParseTree* tree = new ParseTree("subroutineBody", "");
  // starting symbol {
  tree->addChild(mustBe("symbol", "{"));

  // while "var" exists
  while (have("keyword", "var")) {
    ParseTree* variableDeclaration = compileVarDec();
    tree->addChild(variableDeclaration);
  }

  // handles all statements in Jack grammar
  if (have("keyword", "while") || have("keyword", "if") ||
      have("keyword", "let") || have("keyword", "do") ||
      have("keyword", "return")) {
    ParseTree* statements = compileStatements();
    tree->addChild(statements);
  }

  // closing symbol }
  tree->addChild(mustBe("symbol", "}"));

  return tree;
}

/**
 * Generates a parse tree for a subroutine variable declaration
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileVarDec() {
  ParseTree* tree = new ParseTree("varDec", "");
  tree->addChild(mustBe("keyword", "var"));

  // handling variable types & identifier (name)
  Token* currentToken = current();
  if (have("keyword", "boolean") || have("keyword", "char") ||
      have("keyword", "int") || currentToken->getType() == "identifier") {
    tree->addChild(current());
    next();
  } else {
    throw ParseException();
  }

  // checking if current token is identifier
  currentToken = current();
  if (currentToken->getType() == "identifier") {
    tree->addChild(currentToken);
    next();
  } else {
    throw ParseException();
  }

  // handling comma symbol & var names as seen above
  while (have("symbol", ",")) {
    Token* commaSymbol = current();
    next();

    Token* currentToken = current();
    if (currentToken->getType() == "identifier") {
      tree->addChild(commaSymbol);
      tree->addChild(currentToken);
      next();
    } else {
      throw ParseException();
    }
  }

  tree->addChild(mustBe("symbol", ";"));

  return tree;
}

/**
 * Generates a parse tree for a series of statements
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileStatements() {
  ParseTree* tree = new ParseTree("statements", "");

  // while any of jack grammar statements present
  while (have("keyword", "while") || have("keyword", "if") ||
         have("keyword", "let") || have("keyword", "do") ||
         have("keyword", "return")) {
    // while statement case
    if (have("keyword", "while")) {
      ParseTree* WHILE = compileWhile();
      tree->addChild(WHILE);

      // if statement case
    } else if (have("keyword", "if")) {
      ParseTree* IF = compileIf();
      tree->addChild(IF);

      // let statement case
    } else if (have("keyword", "let")) {
      ParseTree* LET = compileLet();
      tree->addChild(LET);

      // do statement case
    } else if (have("keyword", "do")) {
      ParseTree* DO = compileDo();
      tree->addChild(DO);

      // return statement case
    } else if (have("keyword", "return")) {
      ParseTree* RETURN = compileReturn();
      tree->addChild(RETURN);
    }
  }

  return tree;
}

/**
 * Generates a parse tree for a let statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileLet() {
  ParseTree* tree = new ParseTree("letStatement", "");
  tree->addChild(mustBe("keyword", "let"));

  // handling identifier (names)
  Token* currentToken = current();
  if (currentToken->getType() == "identifier") {
    tree->addChild(currentToken);
    next();
  }

  // handling the expression lines - calls compileExpression()
  if (have("symbol", "[")) {
    tree->addChild(mustBe("symbol", "["));
    ParseTree* a = compileExpression();
    tree->addChild(a);
    tree->addChild(mustBe("symbol", "]"));
  }
  tree->addChild(mustBe("symbol", "="));

  // handling skip keyword
  if (have("keyword", "skip")) {
    ParseTree* b = compileExpression();
    tree->addChild(b);
  } else {
    throw ParseException();
  }

  tree->addChild(mustBe("symbol", ";"));
  return tree;
}

/**
 * Generates a parse tree for an if statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileIf() {
  ParseTree* tree = new ParseTree("ifStatement", "");
  tree->addChild(mustBe("keyword", "if"));
  tree->addChild(mustBe("symbol", "("));

  // handling skip from grammar -  if (this section)
  if (have("keyword", "skip")) {
    ParseTree* a = compileExpression();
    tree->addChild(a);
  } else {
    throw ParseException();
  }
  tree->addChild(mustBe("symbol", ")"));  // closes if statement internals

  // starts statement execution code
  tree->addChild(mustBe("symbol", "{"));
  ParseTree* statementCode = compileStatements();
  tree->addChild(statementCode);
  tree->addChild(mustBe("symbol", "}"));

  // for else {} add-on statments
  if (have("keyword", "else")) {
    tree->addChild(mustBe("keyword", "else"));
    tree->addChild(mustBe("symbol", "{"));
    ParseTree* elseCode = compileStatements();
    tree->addChild(elseCode);
    tree->addChild(mustBe("symbol", "}"));
  }
  return tree;
}

/**
 * Generates a parse tree for a while statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileWhile() {
  ParseTree* tree = new ParseTree("whileStatement", "");
  tree->addChild(mustBe("keyword", "while"));
  tree->addChild(mustBe("symbol", "("));

  // handling skip from grammar - while (this section)
  if (have("keyword", "skip")) {
    ParseTree* a = compileExpression();
    tree->addChild(a);
  } else {
    throw ParseException();
  }
  tree->addChild(mustBe("symbol", ")"));

  // statement code
  tree->addChild(mustBe("symbol", "{"));
  ParseTree* statementCode = compileStatements();
  tree->addChild(statementCode);
  tree->addChild(mustBe("symbol", "}"));

  return tree;
}

/**
 * Generates a parse tree for a do statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileDo() {
  ParseTree* tree = new ParseTree("doStatement", "");
  tree->addChild(mustBe("keyword", "do"));

  // handling skip from grammar
  if (have("keyword", "skip")) {
    ParseTree* a = compileExpression();
    tree->addChild(a);
  } else {
    throw ParseException();
  }

  tree->addChild(mustBe("symbol", ";"));
  return tree;
}

/**
 * Generates a parse tree for a return statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileReturn() {
  ParseTree* tree = new ParseTree("returnStatement", "");
  tree->addChild(mustBe("keyword", "return"));

  // handling skip from grammar
  if (have("keyword", "skip")) {
    ParseTree* a = compileExpression();
    tree->addChild(a);
  }
  tree->addChild(mustBe("symbol", ";"));
  return tree;
}

/**
 * Generates a parse tree for an expression
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileExpression() {
  ParseTree* tree = new ParseTree("expression", "");

  // matching grammar rule "skip" as required
  if (have("keyword", "skip"))
    tree->addChild(mustBe("keyword", "skip"));
  else {
    // expression term after skip rule
    ParseTree* a = compileTerm();
    tree->addChild(a);

    // implementing symbols in Jack Grammar
    while (have("symbol", "+") || have("symbol", "-") || have("symbol", "*") ||
           have("symbol", "/") || have("symbol", "<") || have("symbol", ">") ||
           have("symbol", "=") || have("symbol", "&") || have("symbol", "|")) {
      Token* symbols = current();
      tree->addChild(symbols);
      next();

      // expression term after symbols
      ParseTree* b = compileTerm();
      tree->addChild(b);
    }
  }

  return tree;
}

/**
 * Generates a parse tree for an expression term
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileTerm() {  // NOTE: REMEMBER TO REMOVE THE COUT
                                            // STATEMENTS AFTER TESTING
  ParseTree* tree = new ParseTree("term", "");
  Token* currentToken = current();

  // handling terms
  if (currentToken->getValue() == "this" ||
      currentToken->getValue() == "null" ||
      currentToken->getValue() == "true" ||
      currentToken->getValue() == "false") {
    tree->addChild(currentToken);
    next();

    // handling terms
  } else if (currentToken->getType() == "stringConstant") {
    tree->addChild(currentToken);
    next();
    // handling terms
  } else if (currentToken->getType() == "integerConstant") {
    tree->addChild(currentToken);
    next();
    // handling terms
  } else if (currentToken->getType() == "identifier") {
    tree->addChild(currentToken);
    next();

    // handling square bracket symbols []
    if (have("symbol", "[")) {
      tree->addChild(mustBe("symbol", "["));

      ParseTree* bracketSymbol = compileExpression();
      tree->addChild(bracketSymbol);
      tree->addChild(mustBe("symbol", "]"));
    }

    // handling round bracket symbols ()
    if (have("symbol", "(")) {
      tree->addChild(mustBe("symbol", "("));

      ParseTree* roundBracket = compileExpressionList();
      tree->addChild(roundBracket);
      tree->addChild(mustBe("symbol", ")"));
    }

    // handling full-stop symbol
    if (have("symbol", ".")) {
      tree->addChild(mustBe("symbol", "."));
      Token* currentToken = current();

      // checking for identifier (name)
      if (currentToken->getType() == "identifier") {
        tree->addChild(currentToken);
        next();
      } else {
        throw ParseException();
      }
      tree->addChild(mustBe("symbol", "("));

      ParseTree* a = compileExpressionList();
      tree->addChild(a);
      tree->addChild(mustBe("symbol", ")"));
    }
  } else if (have("symbol", "(")) {
    tree->addChild(mustBe("symbol", "("));

    ParseTree* a = compileExpression();
    tree->addChild(a);
    tree->addChild(mustBe("symbol", ")"));

    // make sure this works correctly
  } else if (currentToken->getValue() == "~" ||
             currentToken->getValue() == "-") {
    tree->addChild(currentToken);
    next();

    ParseTree* b = compileTerm();
    tree->addChild(b);
  }
  return tree;
}

/**
 * Generates a parse tree for an expression list
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileExpressionList() {
  ParseTree* tree = new ParseTree("expressionList", "");
  ParseTree* compileExpressions = compileExpression();
  tree->addChild(compileExpressions);
  // handles all comma symbols
  while (have("symbol", ",")) {
    tree->addChild(mustBe("symbol", ","));
    ParseTree* commaSymbol = compileExpression();
    tree->addChild(commaSymbol);
  }
  return tree;
}

/**
 * Advance to the next token
 */
void CompilerParser::next() {
  if (!tokens.empty()) {
    tokens.pop_front();
  }
  return;
}

/**
 * Return the current token
 * @return the Token
 */
Token* CompilerParser::current() {
  if (!tokens.empty()) {
    return tokens.front();
  }
  return NULL;
}

/**
 * Check if the current token matches the expected type and value.
 * @return true if a match, false otherwise
 */
bool CompilerParser::have(std::string expectedType, std::string expectedValue) {
  Token* currToken = current();

  // base case
  if (currToken == NULL) {
    return false;
  }

  if (currToken->getType() == expectedType) {
    if (currToken->getValue() == expectedValue) {
      return true;
    }
  }

  return false;
}

/**
 * Check if the current token matches the expected type and value.
 * If so, advance to the next token, returning the current token, otherwise
 * throw a ParseException.
 * @return the current token before advancing
 */
Token* CompilerParser::mustBe(std::string expectedType,
                              std::string expectedValue) {
  if (have(expectedType, expectedValue)) {
    Token* currentToken = current();
    next();
    return currentToken;
  } else {
    throw ParseException();
  }

  return NULL;

  return NULL;
}

/**
 * Definition of a ParseException
 * You can use this ParseException with `throw ParseException();`
 */
const char* ParseException::what() {
  return "An Exception occurred while parsing!";
}
