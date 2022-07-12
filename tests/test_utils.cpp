#include <string>
#include <vector>
#include "pch.h"
#include "CppUnitTest.h"
#include "../Hypoinput/utils.h"
#include "../Hypoinput/utils.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests {

TEST_CLASS(tests) {
public:

	TEST_METHOD(boolToString)
	{
		// When given true
		// Should return "true".
        Assert::AreEqual(utils::boolToString(true), std::string("true"));

		// When given false
		// Should return "false".
		Assert::AreEqual(utils::boolToString(false), std::string("false"));
	}

	TEST_METHOD(ltrim)
    {
        // When given no argument for char
        // Should trim spaces on left side.
        std::string s = "  Hello  ";
        std::string expected = "Hello  ";
        std::string actual = utils::ltrim(s);
        Assert::AreEqual(expected, actual);

        s = "Hello  ";
        actual = utils::ltrim(s);
        Assert::AreEqual(expected, actual);

        // When given a char
        // Should trim it from left side.
        char ch = 'a';
        s = "aaHelloaa";
        expected = "Helloaa";
        actual = utils::ltrim(s, ch);
        Assert::AreEqual(expected, actual);

        s = "Helloaa";
        actual = utils::ltrim(s, ch);
        Assert::AreEqual(expected, actual);
    }

    TEST_METHOD(replace)
    {
        // When given string
        // Should replace all matches with an empty string.
        std::string s = "abcHelloabc";
        std::string expected = "Hello";
        std::string actual = utils::replace(s, "abc");
        Assert::AreEqual(expected, actual);
    }

    TEST_METHOD(rtrim)
    {
        // When given no argument for char
        // Should trim spaces on right side.
        std::string s = "  Hello  ";
        std::string expected = "  Hello";
        std::string actual = utils::rtrim(s);
        Assert::AreEqual(expected, actual);

        s = "  Hello";
        actual = utils::rtrim(s);
        Assert::AreEqual(expected, actual);

        // When given a char
        // Should trim it from right side.
        char ch = 'a';
        s = "aaHelloaa";
        expected = "aaHello";
        actual = utils::rtrim(s, ch);
        Assert::AreEqual(expected, actual);

        s = "aaHello";
        actual = utils::rtrim(s, ch);
        Assert::AreEqual(expected, actual);
    }

    TEST_METHOD(split)
    {
        // When given no argument for char
        // Should split string based on spaces.
        std::string s = "Hello world";
        size_t expected = 2;
        std::vector<std::string> splits = utils::split(s);
        Assert::AreEqual(expected, splits.size());

        // When given char
        // Should split string based on char.
        s = "Helloaworld";
        expected = 2;
        splits = utils::split(s, 'a');
        Assert::AreEqual(expected, splits.size());
    }

    TEST_METHOD(startsWith)
    {
        // When given a string that begins with a specified string
        // Should return true.
        std::string text = "This is some text";
        std::string match = "This";
        Assert::IsTrue(utils::startsWith(text, match));

        // When given a string that doesn't begin with a specified string
        // Should return false.
        match = "this";
        Assert::IsFalse(utils::startsWith(text, match));

        // When the text is an empty string
        // Should return false.
        Assert::IsFalse(utils::startsWith(std::string(), match));

        // When the match is an empty string
        // Should return false.
        Assert::IsFalse(utils::startsWith(text, std::string()));
    }

    TEST_METHOD(stringToBool)
    {
        // Should handle true strings.
        std::vector<std::string> trueStrs = { "true", "True" };
        for (int i = 0; i < trueStrs.size(); i++) {
            Assert::IsTrue(utils::stringToBool(trueStrs[i]));
        }

        // Should handle false strings.
        std::vector<std::string> falseStrs = { "false", "False" };
        for (int i = 0; i < falseStrs.size(); i++) {
            Assert::IsFalse(utils::stringToBool(falseStrs[i]));
        }
    }

    TEST_METHOD(stringToWString)
    {
        // Both strings should have the same content.
        std::wstring expected = L"Hello world";
        std::string s = "Hello world";
        std::wstring actual = utils::stringToWString(s);
        for (int i = 0; i < expected.length(); i++) {
            Assert::AreEqual(expected[i], actual[i]);
        }
    }

    TEST_METHOD(trim)
    {
        // When given no argument for char
        // Should trim spaces on both sides.
        std::string s = "  Hello  ";
        std::string expected = "Hello";
        std::string actual = utils::trim(s);
        Assert::AreEqual(expected, actual);

        s = "  Hello";
        actual = utils::trim(s);
        Assert::AreEqual(expected, actual);

        s = "Hello  ";
        actual = utils::trim(s);
        Assert::AreEqual(expected, actual);

        // When given a char
        // Should trim it from both sides.
        char ch = 'a';
        s = "aaHelloaa";
        actual = utils::trim(s, ch);
        Assert::AreEqual(expected, actual);

        s = "aaHello";
        actual = utils::trim(s, ch);
        Assert::AreEqual(expected, actual);

        s = "Helloaa";
        actual = utils::trim(s, ch);
        Assert::AreEqual(expected, actual);
    }

    TEST_METHOD(wstringToString)
    {
        // Both strings should have the same content.
        std::wstring ws = L"Hello world";
        std::string expected = "Hello world";
        std::string actual = utils::wstringToString(ws);
        for (int i = 0; i < expected.length(); i++) {
            Assert::AreEqual(expected[i], actual[i]);
        }
    }
};

} // namespace tests
