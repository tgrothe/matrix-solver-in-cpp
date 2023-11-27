#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <variant>
#include <algorithm>
enum class Unknown
{
    POSITIVE,
    NEGATIVE,
};
using Value = std::variant<int, Unknown>;
using Row = std::vector<Value>;
using Matrix = std::vector<std::vector<Value>>;

struct System
{
    Matrix coefficients;
    Row constant_terms;
};

std::ostream &operator<<(std::ostream &out, const Unknown &unknown)
{
    switch (unknown)
    {
    case Unknown::POSITIVE:
        out << "++";
        break;
    case Unknown::NEGATIVE:
        out << "--";
        break;
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
            if (std::holds_alternative<int>(value))
            {
                out << std::setw(4) << std::setfill(' ') << std::get<int>(value);
            }
            else if (std::holds_alternative<Unknown>(value))
            {
                out << "  " << std::get<Unknown>(value);
            }
        }
        out << '\n';
    }
    return out << '\n';
}

std::ostream &operator<<(std::ostream &out, const Row &row)
{
    out << "Matrix: (1 x " << row.size() << ")\n";
    for (int i = 0; i < row.size(); i++)
    {
        out << "   " << (char)('a' + i);
    }
    out << '\n';
    for (auto &value : row)
    {
        if (std::holds_alternative<int>(value))
        {
            out << std::setw(4) << std::setfill(' ') << std::get<int>(value);
        }
        else if (std::holds_alternative<Unknown>(value))
        {
            out << "  " << std::get<Unknown>(value);
        }
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

    printSystem(result);

    for (int i = 0; i < result.coefficients.size(); i++)
    {
        for (int j = 0; j < result.coefficients[i].size(); j++)
        {
            if (std::holds_alternative<int>(result.coefficients[i][j]) && std::get<int>(result.coefficients[i][j]) != 0)
            {
                if (std::holds_alternative<int>(result.constant_terms[j]))
                {
                    result.coefficients[i][j] = std::get<int>(result.coefficients[i][j]) * std::get<int>(result.constant_terms[j]);
                }
                else if (std::get<int>(result.coefficients[i][j]) < 0)
                {
                    result.coefficients[i][j] = Unknown::NEGATIVE;
                }
                else
                {
                    result.coefficients[i][j] = Unknown::POSITIVE;
                }
            }
        }
    }

    printSystem(result);

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (int i = 0; i < result.coefficients.size(); i++)
        {
            int magic_n = 0;
            int magic_i = 0;
            for (int j = 0; j < result.coefficients[i].size(); j++)
            {
                if (std::holds_alternative<Unknown>(result.coefficients[i][j]))
                {
                    magic_n++;
                    magic_i = j;
                }
            }

            if (magic_n == 1)
            {
                int sum = 0;
                for (int j = 0; j < result.coefficients[i].size(); j++)
                {
                    if (j != magic_i)
                    {
                        sum += std::get<int>(result.coefficients[i][j]);
                    }
                }

                const bool is_negative = std::get<Unknown>(result.coefficients[i][magic_i]) == Unknown::NEGATIVE;
                for (int j = 0; j < result.coefficients.size(); j++)
                {
                    if (std::holds_alternative<Unknown>(result.coefficients[j][magic_i]))
                    {
                        if (std::get<Unknown>(result.coefficients[j][magic_i]) == Unknown::POSITIVE)
                        {
                            result.coefficients[j][magic_i] = is_negative ? sum : -sum;
                        }
                        else
                        {
                            result.coefficients[j][magic_i] = is_negative ? -sum : sum;
                        }
                    }
                }

                if (std::holds_alternative<Unknown>(result.constant_terms[magic_i]))
                {
                    result.constant_terms[magic_i] = is_negative ? sum : -sum;
                }

                changed = true;
                break;
            }
        }
    }

    printSystem(result);

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
            r.push_back(stoi(segment));
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
            if (segment == "-")
            {
                input.constant_terms.push_back(Unknown::POSITIVE);
            }
            else
            {
                input.constant_terms.push_back(stoi(segment));
            }
        }
    }

    System result = solveAndPrint(input);

    return 0;
}
