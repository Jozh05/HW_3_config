#include "pch.h"
#include "..\Functions.cpp"
#include <sstream>
#include <string>

namespace function_tests {

	TEST(test_function_parseArray, filled_array) {
		std::string input = "[12, 'test string', [22, 23], table(key => 'value')]";

		std::vector<std::string> result = parseArray(input);
		std::vector<std::string> expected_result = { "12", "'test string'", "[22, 23]", "table(key => 'value')" };
		EXPECT_EQ(result, expected_result);
	}

	TEST(test_function_parseArray, empty_array) {
		std::string input = "[]";
		std::vector<std::string> result = parseArray(input);
		std::vector<std::string> expected_result = { "" };
		EXPECT_EQ(result, expected_result);
	}

	TEST(test_function_parseTable, filled_table) {
		
		std::string input = "table(key_1 => 12, key_2 => 'string', key_3 => [1, 2, 3], key_4 => table(key => value))";
		std::vector<std::pair<std::string, std::string>> result = parseTable(input);
		std::vector<std::pair<std::string, std::string>> expected_result =
{ {"key_1", "12"}, {"key_2", "'string'"}, {"key_3", "[1, 2, 3]"}, {"key_4", "table(key => value)"} };
		EXPECT_EQ(result, expected_result);
	}

	TEST(test_function_parseTable, empty_table) {
		std::string input = "table()";
		std::vector<std::pair<std::string, std::string>> result = parseTable(input);
		std::vector<std::pair<std::string, std::string>> expected_result = {};
		EXPECT_EQ(result, expected_result);
	}

	TEST(test_function_parseVariable, number) {
		std::string input = "42";
		Value result = parseVariable(input);
		Value expected_result = Value(stoi(input));
		EXPECT_EQ(result.data, expected_result.data);
	}

	TEST(test_function_parseVariable, string) {
		std::string input = "'test string'";
		Value result = parseVariable(input);
		Value expected_result = Value(input.substr(1, input.length() - 2));
	}

	TEST(test_function_parseVariable, simple_array) {
		std::string input = "[1, 2, 3]";
		Value result = parseVariable(input);

		const auto& vectorData = std::get<std::vector<std::shared_ptr<Value>>>(result.data);
		ASSERT_EQ(vectorData.size(), 3);

		EXPECT_EQ(std::get<int>(vectorData[0]->data), 1);
		EXPECT_EQ(std::get<int>(vectorData[1]->data), 2);
		EXPECT_EQ(std::get<int>(vectorData[2]->data), 3);
	}

	TEST(test_function_parseVariable, hard_array) {
		std::string input = "[1, 'test string', [1, 2, 3], table(key => 'value')]";
		Value result = parseVariable(input);

		ASSERT_TRUE(std::holds_alternative<std::vector<std::shared_ptr<Value>>>(result.data));

		// ��������� ������ �� ����������
		const auto& vectorData = std::get<std::vector<std::shared_ptr<Value>>>(result.data);

		// ���������, ��� ������ �������� 4 ��������
		ASSERT_EQ(vectorData.size(), 4);

		// 1-� �������: ����� �����
		EXPECT_EQ(std::get<int>(vectorData[0]->data), 1);

		// 2-� �������: ������
		EXPECT_EQ(std::get<std::string>(vectorData[1]->data), "test string");

		// 3-� �������: ��������� ������
		ASSERT_TRUE(std::holds_alternative<std::vector<std::shared_ptr<Value>>>(vectorData[2]->data));
		const auto& nestedVector = std::get<std::vector<std::shared_ptr<Value>>>(vectorData[2]->data);
		ASSERT_EQ(nestedVector.size(), 3);

		EXPECT_EQ(std::get<int>(nestedVector[0]->data), 1);
		EXPECT_EQ(std::get<int>(nestedVector[1]->data), 2);
		EXPECT_EQ(std::get<int>(nestedVector[2]->data), 3);

		// 4-� �������: �������
		const auto& table = std::get<std::unordered_map<std::string, std::shared_ptr<Value>>>(vectorData[3]->data);
		ASSERT_EQ(table.size(), 1);

		// ��������� ���� � �������� �������
		auto it = table.find("key");
		ASSERT_NE(it, table.end());  // ��������, ��� ���� ������
		EXPECT_EQ(std::get<std::string>(it->second->data), "value");
	}

	TEST(test_function_parseVariable, table) {
		std::string input = "table(key_1 => [1, 2, 3], name => 'Mikhail', age => 19)";

		// ����� ������� parseVariable � ������� �������
		Value result = parseVariable(input);


		// ��������� ������� �� ����������
		const auto& tableData = std::get<std::unordered_map<std::string, std::shared_ptr<Value>>>(result.data);

		// ���������, ��� � ������� ��� ��������
		ASSERT_EQ(tableData.size(), 3);

		// �������� �������� � ������ "key_1" - ��� ������ ���� ������
		auto it1 = tableData.find("key_1");
		ASSERT_NE(it1, tableData.end());  // ��������, ��� ���� ������
		ASSERT_TRUE(std::holds_alternative<std::vector<std::shared_ptr<Value>>>(it1->second->data));  // ��� ������ ���� ������
		const auto& key1Array = std::get<std::vector<std::shared_ptr<Value>>>(it1->second->data);
		ASSERT_EQ(key1Array.size(), 3);
		EXPECT_EQ(std::get<int>(key1Array[0]->data), 1);
		EXPECT_EQ(std::get<int>(key1Array[1]->data), 2);
		EXPECT_EQ(std::get<int>(key1Array[2]->data), 3);

		// �������� �������� � ������ "name" - ��� ������ ���� ������
		auto it2 = tableData.find("name");
		ASSERT_NE(it2, tableData.end());  // ��������, ��� ���� ������
		EXPECT_EQ(std::get<std::string>(it2->second->data), "Mikhail");

		// �������� �������� � ������ "age" - ��� ������ ���� �����
		auto it3 = tableData.find("age");
		ASSERT_NE(it3, tableData.end());  // ��������, ��� ���� ������
		EXPECT_EQ(std::get<int>(it3->second->data), 19);
	}
}