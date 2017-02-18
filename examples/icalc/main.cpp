// Copyright (c) 2015-2017 Andrew Sutton
// All rights reserved

#include "lang.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#include <beaker/base/printing/print.hpp>

#include <iostream>


int
main()
{
  icalc::language lang;
  icalc::module mod(lang);

  std::string line;
  while (std::getline(std::cin, line)) {
    if (!std::cin)
      break;
    if (line.empty())
      continue;
    
    // Lex all of the tokens into a buffer.
    auto ss = beaker::make_stream(line);
    icalc::lexer lex(ss);
    beaker::token_seq toks;
    while (beaker::token tok = lex())
      toks.push_back(tok);

    auto ts = beaker::make_stream(toks);
    icalc::builder build(mod);
    icalc::parser parse(ts, build);
    auto& e = parse.expression();

    print(lang, e);

    // icalc::builder build(mod);
    // icalc::parser parse(build, toks.data(), toks.data() + toks.size());
    // auto& e = parse.expression();
    // print(lang, e);
  }
}