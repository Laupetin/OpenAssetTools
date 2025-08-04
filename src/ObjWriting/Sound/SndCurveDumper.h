#pragma once

#include "Dumping/AbstractTextDumper.h"

#include <ostream>

class SndCurveDumper : AbstractTextDumper
{
    static constexpr auto DEFAULT_PRECISION = 4;

    size_t m_precision;
    size_t m_current_knot;
    size_t m_total_knots;

public:
    explicit SndCurveDumper(std::ostream& stream);
    SndCurveDumper(std::ostream& stream, size_t precision);

    void Init(size_t totalKnots);
    void WriteKnot(float x, float y);
    void WriteKnot(double x, double y);
};
