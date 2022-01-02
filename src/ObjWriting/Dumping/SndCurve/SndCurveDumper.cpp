#include "SndCurveDumper.h"

#include <cassert>
#include <iomanip>

SndCurveDumper::SndCurveDumper(std::ostream& stream)
    : SndCurveDumper(stream, DEFAULT_PRECISION)
{
}

SndCurveDumper::SndCurveDumper(std::ostream& stream, const size_t precision)
    : m_stream(stream),
      m_precision(precision),
      m_current_knot(0u),
      m_total_knots(0u)
{
}

void SndCurveDumper::Init(const size_t totalKnots)
{
    m_total_knots = totalKnots;
    m_current_knot = 0;

    m_stream << "SNDCURVE\n\n";
    m_stream << totalKnots;
}

void SndCurveDumper::WriteKnot(const float x, const float y)
{
    assert(m_current_knot >= m_total_knots);

    m_stream << "\n" << std::fixed << std::setprecision(m_precision) << x << " " << y;

    m_current_knot++;
}

void SndCurveDumper::WriteKnot(const double x, const double y)
{
    assert(m_current_knot >= m_total_knots);

    m_stream << x << " " << y << "\n";

    m_current_knot++;
}
