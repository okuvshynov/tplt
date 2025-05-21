#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cmath>

namespace test {

// Simple test case structure
struct TestCase {
    std::string name;
    std::function<bool()> test_func;
    
    TestCase(const std::string& n, std::function<bool()> tf) 
        : name(n), test_func(tf) {}
};

// Test suite class to manage test cases
class TestSuite {
private:
    std::string suite_name;
    std::vector<TestCase> test_cases;
    int passed = 0;
    int failed = 0;
    
public:
    TestSuite(const std::string& name) : suite_name(name) {}
    
    // Add a test case to the suite
    void add_test(const std::string& name, std::function<bool()> test_func) {
        test_cases.emplace_back(name, test_func);
    }
    
    // Run all test cases in the suite
    void run() {
        std::cout << "\n=== Running test suite: " << suite_name << " ===\n";
        
        for (const auto& test_case : test_cases) {
            std::cout << "Running test: " << test_case.name << "... ";
            bool result = false;
            
            try {
                result = test_case.test_func();
                if (result) {
                    std::cout << "PASSED\n";
                    passed++;
                } else {
                    std::cout << "FAILED\n";
                    failed++;
                }
            } catch (const std::exception& e) {
                std::cout << "FAILED (Exception: " << e.what() << ")\n";
                failed++;
            } catch (...) {
                std::cout << "FAILED (Unknown exception)\n";
                failed++;
            }
        }
        
        std::cout << "\nTest suite: " << suite_name << " completed.\n";
        std::cout << "Passed: " << passed << ", Failed: " << failed << "\n";
        std::cout << "========================================\n";
    }
    
    // Return true if all tests passed
    bool all_passed() const {
        return failed == 0;
    }
    
    // Get test summary
    std::pair<int, int> get_summary() const {
        return {passed, failed};
    }
};

// Assert functions for test cases
template<typename T, typename U>
bool assert_equal(const T& actual, const U& expected) {
    return actual == expected;
}

template<>
bool assert_equal<double, double>(const double& actual, const double& expected) {
    const double epsilon = 1e-6;
    return std::abs(actual - expected) < epsilon;
}

template<typename T, typename U>
bool assert_not_equal(const T& actual, const U& expected) {
    return actual != expected;
}

template<typename T>
bool assert_true(const T& value) {
    return value == true;
}

template<typename T>
bool assert_false(const T& value) {
    return value == false;
}

} // namespace test