
// Generated from E:/Projects/VS/OpenAssetTools/src/ObjLoading/Game/IW3/PhysPreset/PhysPreset.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "PhysPresetParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by PhysPresetParser.
 */
class  PhysPresetListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterPhysPreset(PhysPresetParser::PhysPresetContext *ctx) = 0;
  virtual void exitPhysPreset(PhysPresetParser::PhysPresetContext *ctx) = 0;

  virtual void enterPair(PhysPresetParser::PairContext *ctx) = 0;
  virtual void exitPair(PhysPresetParser::PairContext *ctx) = 0;

  virtual void enterKey(PhysPresetParser::KeyContext *ctx) = 0;
  virtual void exitKey(PhysPresetParser::KeyContext *ctx) = 0;

  virtual void enterValue(PhysPresetParser::ValueContext *ctx) = 0;
  virtual void exitValue(PhysPresetParser::ValueContext *ctx) = 0;


};

