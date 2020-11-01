#include "Session.h"

Session::Session()
= default;

Session::Session(ZoneCodeGeneratorArguments args)
    : m_args(std::move(args))
{
    
}
