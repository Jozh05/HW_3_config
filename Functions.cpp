#pragma once
#include "Test_project/pch.h"
#include "Value.h"
#include <regex>

using std::cin, std::cout, std::endl;

std::unordered_map<std::string, Value> variables;

// ������� ��� ������� �������� � ������ � ����� ������
std::string GetLine() {

    std::string str;
    std::getline(std::cin, str);

    // ������� ������ ��-���������� ������
    size_t start = str.find_first_not_of(" \t\r\n");
    // ���� ������ ������� ������ �� ��������, ���������� ������ ������
    if (start == std::string::npos) return "";

    // ������� ��������� ��-���������� ������
    size_t end = str.find_last_not_of(" \t\r\n");

    // ���������� ��������� ��� ��������
    return str.substr(start, end - start + 1);
}


std::vector<std::string> parseArray(const std::string& input) {
    std::vector<std::string> result;

    // ������� ������� ���������� ������
    std::string trimmedInput = input;
    if (trimmedInput.front() == '[' && trimmedInput.back() == ']') {
        trimmedInput = trimmedInput.substr(1, trimmedInput.length() - 2);
    }

    size_t start = 0;
    size_t length = trimmedInput.length();
    int braceCount = 0;  // ��� ������������ ����������� ������

    // ��������� ������ �� �������, ��������� ������� ������ ��������� ��������
    for (size_t i = 0; i < length; ++i) {
        char ch = trimmedInput[i];

        // �����������/��������� ������� ����������� ��� ������� ������
        if (ch == '[' || ch == '(') {
            ++braceCount;
        }
        else if (ch == ']' || ch == ')') {
            --braceCount;
        }

        // ���� ������� ������ - ������� � ��� ��������� ������, �� ��� �����������
        if (ch == ',' && braceCount == 0) {
            std::string element = trimmedInput.substr(start, i - start);
            // ������� ������� � ������ � � �����
            element.erase(0, element.find_first_not_of(" \t"));
            element.erase(element.find_last_not_of(" \t") + 1);

            // ���� ������� ���������� � "!{" � ������������� �� "}", ��� ����������� �������
            if (element.size() > 2 && element.front() == '!' && element[1] == '{' && element.back() == '}') {
                result.push_back(element); // ��������� ������� ��� ����
            }
            else {
                result.push_back(element); // ��������� ��� ������� �������
            }

            start = i + 1;  // ����� ����� ����� �������
        }
    }

    // ������������ ��������� ������� (����� ��������� ������� ��� ����� ������)
    std::string lastElement = trimmedInput.substr(start);
    lastElement.erase(0, lastElement.find_first_not_of(" \t")); // ������� ������� �����
    lastElement.erase(lastElement.find_last_not_of(" \t") + 1); // ������� ������� ������

    // ���� ��������� ������� ���������� � "!{" � ������������� �� "}", ��� ����������� �������
    if (lastElement.size() > 2 && lastElement.front() == '!' && lastElement[1] == '{' && lastElement.back() == '}') {
        result.push_back(lastElement); // ��������� ������� ��� ����
    }
    else {
        result.push_back(lastElement); // ��������� ��� ������� �������
    }

    return result;
}


std::vector<std::pair<std::string, std::string>> parseTable(const std::string& input) {
    std::vector<std::pair<std::string, std::string>> result;

    // ��� ������ ������ � ��������, ������� ������, ����� � ��������� ���������
    std::regex pattern(R"((\w+)\s*=>\s*(('[^']*')|(\d+)|(\[[^\[\]]*\])|(\!\{[a-zA-Z_]+\})|(table\([^\)]*\))))");
    std::smatch match;

    std::string::const_iterator searchStart(input.cbegin());

    while (regex_search(searchStart, input.cend(), match, pattern)) {
        std::string key = match[1];   // ����
        std::string value = match[2];  // ��������

        // ��������� ���� ����-��������
        result.push_back({ key, value });

        // ���������� � ������� ����� �������� ����������
        searchStart = match.suffix().first;
    }

    return result;
}



Value parseVariable(std::string& element) {
    std::regex string_re("'[^']*'");
    std::regex int_re("^[+-]?\\d+$");
    std::regex arr_re(R"(\[[^\[\]]*(?:\[(?:[^\[\]]*(?:\[[^\[\]]*\])?[^\[\]]*)*\])?[^\[\]]*\])");
    std::regex empty_array_re(R"(\[\s*\])");
    std::regex table_re(R"(table\(([^()]*|table\([^\)]*\))+\))");
    std::regex var_re(R"((\!\{[a-zA-Z_]+\}))");

    if (regex_match(element, empty_array_re)) {
        std::vector<std::shared_ptr<Value>> result = {};
        return Value(result);
    }

    else if (regex_match(element, string_re)) {
        return Value(element.substr(1, element.length() - 2));
    }

    else if (regex_match(element, int_re)) {
        return Value(std::stoi(element));
    }

    else if (regex_match(element, arr_re)) {
        std::vector<std::shared_ptr<Value>> result;
        std::vector<std::string> elements = parseArray(element);
        for (auto& el : elements) {
            result.push_back(std::make_shared<Value>(parseVariable(el)));
        }
        return Value(result);
    }
    //TODO ���������� �������

    else if (regex_match(element, table_re)) {
        std::unordered_map<std::string, std::shared_ptr<Value>> result;
        std::vector<std::pair<std::string, std::string>> pairs = parseTable(element);
        for (auto& i : pairs) {
            result.emplace(i.first, std::make_shared<Value>(parseVariable(i.second)));
        }
        return Value(result);
    }

    else if (regex_match(element, var_re)) {
        std::string name = element.substr(2, element.length() - 3);
        return variables[name];
    }

    else {
        std::cerr << "Syntax error: invalid value" << endl;
        exit(-1);
    }
}