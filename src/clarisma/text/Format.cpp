//
// Created by marti on 9/29/2024.
//

#include <clarisma/text/Format.h>
#include <clarisma/math/Math.h>

namespace clarisma::Format {

/*
char* formatDouble(char* buf, double d, int precision, bool zeroFill)
{
    assert(precision >= 0 && precision <= 15);
    double multiplier = Math::POWERS_OF_10[precision];
    long long roundedScaled = static_cast<long long>(round(d * multiplier));
    long long intPart = static_cast<long long>(roundedScaled / multiplier);
    unsigned long long fracPart = static_cast<unsigned long long>(
        abs(roundedScaled - intPart * multiplier));
    char* end = buf + sizeof(buf);
    char* start = formatFractionalReverse(fracPart, &end, precision, zeroFill);
    if (start != end) *(--start) = '.';
    start = formatLongReverse(intPart, start, d < 0);
    writeBytes(start, end - start);
}

static char* formatFractionalReverse(unsigned long long d, char** pEnd, int precision, bool zeroFill)
{
    char* end = *pEnd;
    char* start = end - precision;
    char* p = end;
    while(p > start)
    {
        lldiv_t result = lldiv(d, 10);
        if (p == end && result.rem == 0 && !zeroFill)
        {
            end--;		// skip trailing zeroes
            p--;
        }
        else
        {
            *(--p) = static_cast<char>('0' + result.rem);
        }
        d = result.quot;
    }
    *pEnd = end;
    return p;
}
*/

} // namespace clarisma