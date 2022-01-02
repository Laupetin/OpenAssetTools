#pragma once

#include <istream>
#include <vector>
#include <memory>

#include "Utils/ClassUtils.h"

class SndCurveReader
{
public:
    class Result
    {
    public:
        struct Knot
        {
            double m_x;
            double m_y;
        };
        
        std::vector<Knot> m_knots;
    };

private:
    std::istream& m_stream;
    const std::string& m_filename;

public:
    SndCurveReader(std::istream& stream, const std::string& filename);

    _NODISCARD std::unique_ptr<Result> Read() const;
};
