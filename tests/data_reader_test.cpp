#include "test_framework.hpp"
#include "../src/data_reader.hpp"
#include "../src/arg_parser.hpp"
#include <sstream>

using namespace tplt;

// Test header detection
bool test_header_detection() {
    // Create a data reader
    DataReader reader(',');
    
    // Create input with header
    std::string input = "col1,col2,col3\n1,2,3\n4,5,6";
    std::istringstream iss(input);
    std::streambuf* cinbuf = std::cin.rdbuf(); // Save old buf
    std::cin.rdbuf(iss.rdbuf());               // Redirect to iss
    
    // Parse options with auto-detection (default)
    Options options;
    options.delimiter = ',';
    options.x_field = FieldSpec(1);
    options.y_field = FieldSpec(2);
    options.header_mode = Options::HeaderMode::Auto;
    
    // Read data
    auto data_points = reader.read_data<double>(options);
    
    // Restore cin
    std::cin.rdbuf(cinbuf);
    
    // Verify header detection
    bool test1 = test::assert_true(reader.has_headers());
    bool test2 = test::assert_equal(reader.get_headers().size(), static_cast<size_t>(3));
    bool test3 = test::assert_equal(reader.get_headers()[0], std::string("col1"));
    bool test4 = test::assert_equal(reader.get_headers()[1], std::string("col2"));
    bool test5 = test::assert_equal(reader.get_headers()[2], std::string("col3"));
    
    // Verify data points
    bool test6 = test::assert_equal(data_points.size(), static_cast<size_t>(2));
    bool test7 = test::assert_equal(data_points[0].x, 1.0);
    bool test8 = test::assert_equal(data_points[0].y, 2.0);
    bool test9 = test::assert_equal(data_points[1].x, 4.0);
    bool test10 = test::assert_equal(data_points[1].y, 5.0);
    
    return test1 && test2 && test3 && test4 && test5 && 
           test6 && test7 && test8 && test9 && test10;
}

// Test field name lookup
bool test_field_name_lookup() {
    // Create a data reader
    DataReader reader(',');
    
    // Create input with header
    std::string input = "x_val,y_val,value\n1,2,3\n4,5,6";
    std::istringstream iss(input);
    std::streambuf* cinbuf = std::cin.rdbuf(); // Save old buf
    std::cin.rdbuf(iss.rdbuf());               // Redirect to iss
    
    // Parse options
    Options options;
    options.delimiter = ',';
    options.x_field = FieldSpec("x_val");
    options.y_field = FieldSpec("y_val");
    options.aggregation.function = AggregationSpec::Function::Sum;
    options.aggregation.field = FieldSpec("value");
    
    // Read data
    auto data_points = reader.read_data<double>(options);
    
    // Restore cin
    std::cin.rdbuf(cinbuf);
    
    // Verify header detection
    bool test1 = test::assert_true(reader.has_headers());
    
    // Verify data points
    bool test2 = test::assert_equal(data_points.size(), static_cast<size_t>(2));
    bool test3 = test::assert_equal(data_points[0].x, 1.0);
    bool test4 = test::assert_equal(data_points[0].y, 2.0);
    bool test5 = test::assert_true(data_points[0].value.has_value());
    bool test6 = test::assert_equal(data_points[0].value.value(), 3.0);
    
    bool test7 = test::assert_equal(data_points[1].x, 4.0);
    bool test8 = test::assert_equal(data_points[1].y, 5.0);
    bool test9 = test::assert_true(data_points[1].value.has_value());
    bool test10 = test::assert_equal(data_points[1].value.value(), 6.0);
    
    return test1 && test2 && test3 && test4 && test5 &&
           test6 && test7 && test8 && test9 && test10;
}

// Test forced header mode
bool test_forced_header_mode() {
    // Create a data reader
    DataReader reader(',');
    
    // Create input with numeric data that would not normally be detected as a header
    std::string input = "1,2,3\n4,5,6\n7,8,9";
    std::istringstream iss(input);
    std::streambuf* cinbuf = std::cin.rdbuf(); // Save old buf
    std::cin.rdbuf(iss.rdbuf());               // Redirect to iss
    
    // Parse options with forced header
    Options options;
    options.delimiter = ',';
    options.x_field = FieldSpec("1");  // Now treating "1", "2", "3" as field names
    options.y_field = FieldSpec("2");
    options.header_mode = Options::HeaderMode::ForceOn;
    
    // Read data
    auto data_points = reader.read_data<double>(options);
    
    // Restore cin
    std::cin.rdbuf(cinbuf);
    
    // Verify header detection
    bool test1 = test::assert_true(reader.has_headers());
    bool test2 = test::assert_equal(reader.get_headers().size(), static_cast<size_t>(3));
    bool test3 = test::assert_equal(reader.get_headers()[0], std::string("1"));
    bool test4 = test::assert_equal(reader.get_headers()[1], std::string("2"));
    bool test5 = test::assert_equal(reader.get_headers()[2], std::string("3"));
    
    // Verify data points
    bool test6 = test::assert_equal(data_points.size(), static_cast<size_t>(2));
    bool test7 = test::assert_equal(data_points[0].x, 4.0);
    bool test8 = test::assert_equal(data_points[0].y, 5.0);
    bool test9 = test::assert_equal(data_points[1].x, 7.0);
    bool test10 = test::assert_equal(data_points[1].y, 8.0);
    
    return test1 && test2 && test3 && test4 && test5 && 
           test6 && test7 && test8 && test9 && test10;
}

// Test forced no-header mode
bool test_forced_no_header_mode() {
    // Create a data reader
    DataReader reader(',');
    
    // Create input with only numeric data to avoid parsing errors
    std::string input = "1,2,3\n4,5,6\n7,8,9";
    std::istringstream iss(input);
    std::streambuf* cinbuf = std::cin.rdbuf(); // Save old buf
    std::cin.rdbuf(iss.rdbuf());               // Redirect to iss
    
    // Parse options with forced no-header
    Options options;
    options.delimiter = ',';
    options.x_field = FieldSpec(1);
    options.y_field = FieldSpec(2);
    options.header_mode = Options::HeaderMode::ForceOff;
    
    // Read data
    auto data_points = reader.read_data<double>(options);
    
    // Restore cin
    std::cin.rdbuf(cinbuf);
    
    // Verify no header detection
    bool test1 = test::assert_false(reader.has_headers());
    
    // Verify data points (should include all rows as data)
    bool test2 = test::assert_equal(data_points.size(), static_cast<size_t>(3));
    
    // Verify first data point (which would normally be a header)
    bool test3 = test::assert_equal(data_points[0].x, 1.0);
    bool test4 = test::assert_equal(data_points[0].y, 2.0);
    
    return test1 && test2 && test3 && test4;
}

// Test quoted numeric values with double quotes
bool test_quoted_numeric_values_double() {
    // Create a data reader
    DataReader reader(',');
    
    // Create input with quoted numeric values
    std::string input = "x,y,value\n\"1.5\",\"2.5\",\"3.5\"\n\"4.0\",\"5.0\",\"6.0\"";
    std::istringstream iss(input);
    std::streambuf* cinbuf = std::cin.rdbuf(); // Save old buf
    std::cin.rdbuf(iss.rdbuf());               // Redirect to iss
    
    // Parse options
    Options options;
    options.delimiter = ',';
    options.x_field = FieldSpec("x");
    options.y_field = FieldSpec("y");
    options.aggregation.function = AggregationSpec::Function::Sum;
    options.aggregation.field = FieldSpec("value");
    
    // Read data
    auto data_points = reader.read_data<double>(options);
    
    // Restore cin
    std::cin.rdbuf(cinbuf);
    
    // Verify header detection
    bool test1 = test::assert_true(reader.has_headers());
    
    // Verify data points
    bool test2 = test::assert_equal(data_points.size(), static_cast<size_t>(2));
    bool test3 = test::assert_equal(data_points[0].x, 1.5);
    bool test4 = test::assert_equal(data_points[0].y, 2.5);
    bool test5 = test::assert_true(data_points[0].value.has_value());
    bool test6 = test::assert_equal(data_points[0].value.value(), 3.5);
    
    bool test7 = test::assert_equal(data_points[1].x, 4.0);
    bool test8 = test::assert_equal(data_points[1].y, 5.0);
    bool test9 = test::assert_true(data_points[1].value.has_value());
    bool test10 = test::assert_equal(data_points[1].value.value(), 6.0);
    
    return test1 && test2 && test3 && test4 && test5 &&
           test6 && test7 && test8 && test9 && test10;
}

// Test quoted numeric values with single quotes
bool test_quoted_numeric_values_single() {
    // Create a data reader
    DataReader reader(',');
    
    // Create input with quoted numeric values using single quotes
    std::string input = "x,y\n'10','20'\n'30','40'";
    std::istringstream iss(input);
    std::streambuf* cinbuf = std::cin.rdbuf(); // Save old buf
    std::cin.rdbuf(iss.rdbuf());               // Redirect to iss
    
    // Parse options
    Options options;
    options.delimiter = ',';
    options.x_field = FieldSpec("x");
    options.y_field = FieldSpec("y");
    
    // Read data
    auto data_points = reader.read_data<double>(options);
    
    // Restore cin
    std::cin.rdbuf(cinbuf);
    
    // Verify header detection
    bool test1 = test::assert_true(reader.has_headers());
    
    // Verify data points
    bool test2 = test::assert_equal(data_points.size(), static_cast<size_t>(2));
    bool test3 = test::assert_equal(data_points[0].x, 10.0);
    bool test4 = test::assert_equal(data_points[0].y, 20.0);
    bool test5 = test::assert_equal(data_points[1].x, 30.0);
    bool test6 = test::assert_equal(data_points[1].y, 40.0);
    
    return test1 && test2 && test3 && test4 && test5 && test6;
}

// Test mixed quoted and unquoted values
bool test_mixed_quoted_unquoted_values() {
    // Create a data reader
    DataReader reader(',');
    
    // Create input with mixed quoted and unquoted values
    std::string input = "x,y,value\n\"1.1\",2.2,\"3.3\"\n4.4,\"5.5\",6.6";
    std::istringstream iss(input);
    std::streambuf* cinbuf = std::cin.rdbuf(); // Save old buf
    std::cin.rdbuf(iss.rdbuf());               // Redirect to iss
    
    // Parse options
    Options options;
    options.delimiter = ',';
    options.x_field = FieldSpec(1);
    options.y_field = FieldSpec(2);
    options.aggregation.function = AggregationSpec::Function::Avg;
    options.aggregation.field = FieldSpec(3);
    
    // Read data
    auto data_points = reader.read_data<double>(options);
    
    // Restore cin
    std::cin.rdbuf(cinbuf);
    
    // Verify header detection
    bool test1 = test::assert_true(reader.has_headers());
    
    // Verify data points
    bool test2 = test::assert_equal(data_points.size(), static_cast<size_t>(2));
    bool test3 = test::assert_equal(data_points[0].x, 1.1);
    bool test4 = test::assert_equal(data_points[0].y, 2.2);
    bool test5 = test::assert_true(data_points[0].value.has_value());
    bool test6 = test::assert_equal(data_points[0].value.value(), 3.3);
    
    bool test7 = test::assert_equal(data_points[1].x, 4.4);
    bool test8 = test::assert_equal(data_points[1].y, 5.5);
    bool test9 = test::assert_true(data_points[1].value.has_value());
    bool test10 = test::assert_equal(data_points[1].value.value(), 6.6);
    
    return test1 && test2 && test3 && test4 && test5 &&
           test6 && test7 && test8 && test9 && test10;
}

// Test quote stripping helper function directly
bool test_quote_stripping_function() {
    DataReader reader(',');
    
    // Test double quotes
    bool test1 = test::assert_equal(reader.strip_quotes("\"123\""), std::string("123"));
    
    // Test single quotes
    bool test2 = test::assert_equal(reader.strip_quotes("'456'"), std::string("456"));
    
    // Test no quotes
    bool test3 = test::assert_equal(reader.strip_quotes("789"), std::string("789"));
    
    // Test mismatched quotes (should not strip)
    bool test4 = test::assert_equal(reader.strip_quotes("\"123'"), std::string("\"123'"));
    
    // Test only one quote
    bool test5 = test::assert_equal(reader.strip_quotes("\""), std::string("\""));
    
    // Test empty quotes
    bool test6 = test::assert_equal(reader.strip_quotes("\"\""), std::string(""));
    bool test7 = test::assert_equal(reader.strip_quotes("''"), std::string(""));
    
    return test1 && test2 && test3 && test4 && test5 && test6 && test7;
}

// Main test function
int main() {
    test::TestSuite data_reader_tests("DataReader Tests");
    
    // Add test cases
    data_reader_tests.add_test("Header Detection", test_header_detection);
    data_reader_tests.add_test("Field Name Lookup", test_field_name_lookup);
    data_reader_tests.add_test("Forced Header Mode", test_forced_header_mode);
    data_reader_tests.add_test("Forced No-Header Mode", test_forced_no_header_mode);
    data_reader_tests.add_test("Quoted Numeric Values (Double Quotes)", test_quoted_numeric_values_double);
    data_reader_tests.add_test("Quoted Numeric Values (Single Quotes)", test_quoted_numeric_values_single);
    data_reader_tests.add_test("Mixed Quoted and Unquoted Values", test_mixed_quoted_unquoted_values);
    data_reader_tests.add_test("Quote Stripping Function", test_quote_stripping_function);
    
    // Run tests
    data_reader_tests.run();
    
    // Return 0 if all tests passed, 1 otherwise
    return data_reader_tests.all_passed() ? 0 : 1;
}