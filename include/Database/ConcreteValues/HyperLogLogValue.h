#ifndef HYPERLOGLOG_VALUE_H
#define HYPERLOGLOG_VALUE_H

#include "../IHyperLogLogValue.h"
#include "../Serializer/ValueSerializationHelper.h"
#include <complex>

class HyperLogLogValue : public IHyperLogLogValue
{
public:
    // b is the number of bits to use for the register index; m = 2^b registers.
    // Typical used value is b = 14.
    explicit HyperLogLogValue(uint8_t b = 14)
        : b(b), m(1 << b), registers(1 << b, 0)
    {
        if (b < 4 || b > 16)
        {
            throw std::invalid_argument("Parameter b must be in [4, 16].");
        }
    }

    ~HyperLogLogValue() override {}

    std::string getType() const override
    {
        return "hyperloglog";
    }

    std::any get() const override
    {
        return std::make_any<HyperLogLogValue>(*this);
    }

    std::string toString() const override
    {
        // Return an approximate cardinality as a string.
        return std::to_string(estimate());
    }

    void add(const std::string &element) override
    {
        // Hash the element to get a 64-bit integer.
        uint64_t hash = std::hash<std::string>{}(element);
        // The index for the register is the first b bits of the hash.
        uint8_t index = hash >> (64 - b);
        // The remaining bits are used to count leading zeros.
        uint64_t remaining = (hash << b) | (1ULL << (b - 1));
        uint8_t leadingZeros = countLeadingZeros(remaining, 64 - b) + 1;
        if (leadingZeros > registers[index])
        {
            registers[index] = leadingZeros;
        }
    }

    uint64_t estimate() const override
    {
        double sum = 0.0;
        for (auto reg : registers)
        {
            sum += std::pow(2.0, -reg);
        }
        double alpha = getAlpha();
        double estimate = alpha * m * m / sum;

        // Small range correction.
        if (estimate <= (5.0 / 2.0) * m)
        {
            int zeros = 0;
            for (auto reg : registers)
            {
                if (reg == 0)
                    zeros++;
            }
            if (zeros != 0)
                estimate = m * std::log(static_cast<double>(m) / zeros);
        }
        return static_cast<uint64_t>(estimate);
    }

    // Serialize the HyperLogLog value.
    std::vector<uint8_t> serialize() const override
    {
        std::vector<uint8_t> buffer;
        // Write b and m to the buffer.
        ValueSerializationHelper::appendToBuffer(buffer, b);
        ValueSerializationHelper::appendToBuffer(buffer, m);
        // Write the registers to the buffer.
        for (const auto &reg : registers)
        {
            ValueSerializationHelper::appendToBuffer(buffer, reg);
        }
        return buffer;
    }

    // Deserialize the HyperLogLog value.
    void deserialize(const std::vector<uint8_t> &buffer) override
    {
        // Read b and m from the buffer.
        size_t offset = 0;
        size_t bVal, mVal;
        if (!ValueSerializationHelper::readFromBuffer(buffer, offset, bVal) ||
            !ValueSerializationHelper::readFromBuffer(buffer, offset, mVal))
        {
            throw std::runtime_error("Failed to deserialize HyperLogLogValue: invalid buffer.");
        }
        b = static_cast<uint8_t>(bVal);
        m = static_cast<uint32_t>(mVal);
        registers.resize(m);
        // Read the registers from the buffer.
        for (size_t i = 0; i < m; ++i)
        {
            size_t regVal;
            if (!ValueSerializationHelper::readFromBuffer(buffer, offset, regVal))
            {
                throw std::runtime_error("Failed to deserialize HyperLogLogValue: invalid buffer.");
            }
            registers[i] = static_cast<uint8_t>(regVal);
        }
    }

private:
    uint8_t b;                      // Number of bits for the index
    uint32_t m;                     // Number of registers (m = 2^b)
    std::vector<uint8_t> registers; // Registers storing the maximum number of leading zeros

    // Count leading zeros in a 64-bit number over a given number of bits.
    uint8_t countLeadingZeros(uint64_t x, uint8_t bits) const
    {
        uint8_t count = 0;
        for (int i = bits - 1; i >= 0; --i)
        {
            if ((x >> i) & 1)
                break;
            ++count;
        }
        return count;
    }

    // Get bias correction constant alpha for m registers.
    double getAlpha() const
    {
        // Predefined constants based on m.
        switch (m)
        {
        case 16:
            return 0.673;
        case 32:
            return 0.697;
        case 64:
            return 0.709;
        default:
            return 0.7213 / (1 + 1.079 / m);
        }
    }
};
#endif // HYPERLOGLOG_VALUE_H