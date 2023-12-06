#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <variant>
#include <cmath>

struct Value
{
    double value = 0.0;
    bool is_unknown = false;
};

using Row = std::vector<Value>;
using Matrix = std::vector<std::vector<Value>>;

struct System
{
    Matrix coefficients;
    Row constant_terms;
};

double round_to(double value, double precision = 0.01)
{
    return std::round(value / precision) * precision;
}

std::ostream &operator<<(std::ostream &out, const Value &v)
{
    if (!v.is_unknown)
    {
        out << std::setw(6) << std::setfill(' ') << round_to(v.value);
    }
    else
    {
        out << "    --";
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const Matrix &matrix)
{
    out << "Matrix: (" << matrix.size() << " x " << matrix[0].size() << ")\n";
    for (auto &row : matrix)
    {
        for (auto &value : row)
        {
            out << value;
        }
        out << '\n';
    }
    return out << '\n';
}

std::ostream &operator<<(std::ostream &out, const Row &row)
{
    out << "Matrix: (1 x " << row.size() << ")\n";
    for (size_t i = 0; i < row.size(); i++)
    {
        out << "     " << (char)('a' + i);
    }
    out << '\n';
    for (auto &value : row)
    {
        out << value;
    }
    out << '\n';
    return out << '\n';
}

void printSystem(const System &system)
{
    std::cout << system.coefficients;
    std::cout << system.constant_terms;
}

System solveAndPrint(const System &input)
{
    System result = input;

    bool changed = true;
    while (changed)
    {
        changed = false;
        printSystem(result);

        for (size_t i = 0; i < result.coefficients.size(); i++)
        {
            size_t magic_n = 0;
            size_t magic_i = 0;
            for (size_t j = 0; j < result.coefficients[i].size(); j++)
            {
                if (result.coefficients[i][j].value != 0 && result.constant_terms[j].is_unknown)
                {
                    magic_n++;
                    magic_i = j;
                }
            }

            if (magic_n == 1)
            {
                double sum = 0;
                for (size_t j = 0; j < result.coefficients[i].size(); j++)
                {
                    if (j != magic_i && result.coefficients[i][j].value != 0)
                    {
                        sum -= result.coefficients[i][j].value * result.constant_terms[j].value;
                    }
                }

                result.constant_terms[magic_i].value = sum / result.coefficients[i][magic_i].value;
                result.constant_terms[magic_i].is_unknown = false;

                changed = true;
                break;
            }
        }
    }

    return result;
}

int main(int argc, char const *argv[])
{
    std::vector<std::string> args({"1 1 -1 0 0 0 0 0 0 -1",
                                   "0 0 1 -1 -1 0 0 0 1 0",
                                   "0 0 0 0 1 -1 1 0 0 0",
                                   "0 0 0 0 0 0 -1 -1 -1 1",
                                   "0",
                                   "10 7 - 3 2 3 - - - 14"});
    // std::vector<std::string> args({"2 6 -8",
    //                                "1 0 -1",
    //                                "0",
    //                                "2 - -"});
    System input;
    for (int i = 1;; i++)
    {
        std::cout << "Koeffizienten " << i << ". Zeile oder 0:\n";
        std::string l;
        // getline(std::cin, l);
        l = args[i - 1];
        std::cout << l << '\n';

        if (l == "0")
        {
            break;
        }
        std::stringstream ss;
        ss << l;
        std::string segment;
        Row r;
        while (getline(ss, segment, ' '))
        {
            r.push_back(Value());
            r.back().value = stod(segment);
        }
        input.coefficients.push_back(r);
    }
    {
        std::cout << "Bekannte, oder nur -, wenn unbekannt:\n";
        std::string l;
        // getline(std::cin, l);
        l = args[args.size() - 1];
        std::cout << l << '\n';

        std::stringstream ss;
        ss << l;
        std::string segment;
        while (getline(ss, segment, ' '))
        {
            input.constant_terms.push_back(Value());
            if (segment == "-")
            {
                input.constant_terms.back().is_unknown = true;
            }
            else
            {
                input.constant_terms.back().value = stod(segment);
            }
        }
    }

    System result = solveAndPrint(input);

    return 0;
}
