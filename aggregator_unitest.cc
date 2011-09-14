#include "aggregator.h"
#include "extensions.pb.h"
#include <limits.h>
#include <gtest/gtest.h>
#include <iostream>
#include <string.h>
using namespace std;

//std::string __function_name = "integer_sum";
rendero::AggregatorValue __initial;

// This is a class that adds integers, and the initial value is 5.
class TestAggregatorFunction : public AbstractAggregatorFunction {
 public:
  TestAggregatorFunction() :
  AbstractAggregatorFunction(std::string("integer_sum"), __initial) {
    initial_.SetExtension(extensions::value, 5);
  }
  virtual void reduce(const rendero::AggregatorValue& partial,
      const rendero::AggregatorValue& value, rendero::AggregatorValue* result)
      const {
    if (!partial.HasExtension(extensions::value)) {
      result->SetExtension(extensions::value,
          initial_.GetExtension(extensions::value) +
          value.GetExtension(extensions::value));
    }
    else {
      result->SetExtension(extensions::value,
          partial.GetExtension(extensions::value) +
          value.GetExtension(extensions::value));
    }
    if (value.HasExtension(extensions::name)) {
      result->SetExtension(extensions::name,
          value.GetExtension(extensions::name));
    }
  }
 private:
};

TEST(TestAggregatorFunctionTest, GeneralTests) {
  TestAggregatorFunction function;
  rendero::AggregatorValue value1;
  rendero::AggregatorValue value2;
  value2.SetExtension(extensions::value, 3);
  rendero::AggregatorValue temp;
  // Test initial value.
  function.reduce(value1, value2, &temp);
  EXPECT_EQ(8, temp.GetExtension(extensions::value));

  // Test simple reduction
  value1.SetExtension(extensions::value, 2);
  value2.SetExtension(extensions::value, 3);
  function.reduce(value1, value2, &temp);
  EXPECT_EQ(5, temp.GetExtension(extensions::value));
}

TEST(AggregatorManagerTest, AddValueToAggregator) {
  AggregatorManager aggregator_manager;
  TestAggregatorFunction function;
  aggregator_manager.RegisterAggregationFunction("integer_sum", &function);
  rendero::AggregatorValue value;
  value.SetExtension(extensions::value, 2);
  aggregator_manager.AddValueToAggregator("integer_sum", value);
  rendero::AggregatorValue temp_value;
  aggregator_manager.GetAggregatorValue("integer_sum", &temp_value);
  EXPECT_EQ(7, temp_value.GetExtension(extensions::value));

  value.SetExtension(extensions::value, 5);
  aggregator_manager.AddValueToAggregator("integer_sum", value);
  aggregator_manager.GetAggregatorValue("integer_sum", &temp_value);
  EXPECT_EQ(12, temp_value.GetExtension(extensions::value));
}

TEST(AggregatorManagerTest, OutputAndResetAllAggregatorsTest) {
  AggregatorManager aggregator_manager;
  TestAggregatorFunction function;
  aggregator_manager.RegisterAggregationFunction("integer_sum", &function);
  aggregator_manager.RegisterAggregationFunction("integer_sum2", &function);
  rendero::AggregatorValue value;
  value.SetExtension(extensions::value, 2);
  value.SetExtension(extensions::name, "integer_sum");
  aggregator_manager.AddValueToAggregator("integer_sum", value);
  aggregator_manager.RegisterAggregationFunction("integer_sum2", &function);
  value.SetExtension(extensions::value, 3);
  value.SetExtension(extensions::name, "integer_sum2");
  aggregator_manager.AddValueToAggregator("integer_sum2", value);

  rendero::AggregatorGroup aggregator_group;

  aggregator_manager.OutputAndResetAllAggregators(&aggregator_group);

  // Test if aggregators were cleared
  aggregator_manager.GetAggregatorValue("integer_sum", &value);
  EXPECT_FALSE(value.HasExtension(extensions::value));
  aggregator_manager.GetAggregatorValue("integer_sum2", &value);
  EXPECT_FALSE(value.HasExtension(extensions::value));
  
  for (int i = 0; i < aggregator_group.aggregators_size(); ++i) {
    value = aggregator_group.aggregators(i);
    if (value.GetExtension(extensions::name) == "integer_sum") {
      EXPECT_EQ(7, value.GetExtension(extensions::value));
    }
    else if (value.GetExtension(extensions::name) == "integer_sum2") {
      EXPECT_EQ(8, value.GetExtension(extensions::value));
    }
    else {
      EXPECT_EQ(1,0);
    }
  }
}

TEST(AggregatorTest, GeneralTests) {
  AggregatorManager aggregator_manager;
  TestAggregatorFunction function;
  aggregator_manager.RegisterAggregationFunction("integer_sum", &function);
  aggregator_manager.RegisterAggregationFunction("random", &function);
  Aggregator aggregator;

  // Try an empty aggregator
  aggregator_manager.GetAggregator("random", &aggregator);

  rendero::AggregatorValue value;
  value.SetExtension(extensions::value, 2);
  aggregator.Increment(value);
  EXPECT_EQ(7, aggregator.value().GetExtension(extensions::value));

  // Try a filled aggregator
  value.SetExtension(extensions::value, 5);
  aggregator_manager.AddValueToAggregator("integer_sum", value);
  aggregator_manager.GetAggregator("integer_sum", &aggregator);
  EXPECT_EQ(10, aggregator.value().GetExtension(extensions::value));
  aggregator.Increment(value);
  EXPECT_EQ(15, aggregator.value().GetExtension(extensions::value));
}
