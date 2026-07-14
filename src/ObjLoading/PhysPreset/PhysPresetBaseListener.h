
// Generated from E:/Projects/VS/OpenAssetTools/src/ObjLoading/PhysPreset/PhysPreset.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "PhysPresetListener.h"


/**
 * This class provides an empty implementation of PhysPresetListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  PhysPresetBaseListener : public PhysPresetListener {
public:

  virtual void enterPhysPreset(PhysPresetParser::PhysPresetContext * /*ctx*/) override { }
  virtual void exitPhysPreset(PhysPresetParser::PhysPresetContext * /*ctx*/) override { }

  virtual void enterPair(PhysPresetParser::PairContext * /*ctx*/) override { }
  virtual void exitPair(PhysPresetParser::PairContext * /*ctx*/) override { }

  virtual void enterKey(PhysPresetParser::KeyContext * /*ctx*/) override { }
  virtual void exitKey(PhysPresetParser::KeyContext * /*ctx*/) override { }

  virtual void enterValue(PhysPresetParser::ValueContext * /*ctx*/) override { }
  virtual void exitValue(PhysPresetParser::ValueContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

