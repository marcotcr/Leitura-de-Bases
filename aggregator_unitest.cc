// Copyright (c) 2011 by  Marco Túlio Ribeiro <marcotcr@gmail.com>,
//                        Tiago Alves Macambira <tmacam@burocrata.org>
// All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//
// Implementation of aggregators.

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
    initial_.set_int64_value(5);
  }
  virtual void reduce(const rendero::AggregatorValue& partial,
      const rendero::AggregatorValue& value, rendero::AggregatorValue* result)
      const {
    if (!partial.has_int64_value()) {
      result->set_int64_value(
          initial_.int64_value() +
          value.int64_value());
    }
    else {
      result->set_int64_value(
          partial.int64_value() +
          value.int64_value());
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
  value2.set_int64_value(3);
  rendero::AggregatorValue temp;
  // Test initial value.
  function.reduce(value1, value2, &temp);
  EXPECT_EQ(8, temp.int64_value());

  // Test simple reduction
  value1.set_int64_value(2);
  value2.set_int64_value(3);
  function.reduce(value1, value2, &temp);
  EXPECT_EQ(5, temp.int64_value());
}

TEST(AggregatorManagerTest, AddValueToAggregator) {
  AggregatorManager aggregator_manager;
  TestAggregatorFunction function;
  aggregator_manager.RegisterAggregationFunction("integer_sum", &function);
  rendero::AggregatorValue value;
  value.set_int64_value(2);
  aggregator_manager.AddValueToAggregator("integer_sum", value);
  rendero::AggregatorValue temp_value;
  aggregator_manager.GetAggregatorValue("integer_sum", &temp_value);
  EXPECT_EQ(7, temp_value.int64_value());

  value.set_int64_value(5);
  aggregator_manager.AddValueToAggregator("integer_sum", value);
  aggregator_manager.GetAggregatorValue("integer_sum", &temp_value);
  EXPECT_EQ(12, temp_value.int64_value());
}

TEST(AggregatorManagerTest, OutputAndResetAllAggregatorsTest) {
  AggregatorManager aggregator_manager;
  TestAggregatorFunction function;
  aggregator_manager.RegisterAggregationFunction("integer_sum", &function);
  aggregator_manager.RegisterAggregationFunction("integer_sum2", &function);
  rendero::AggregatorValue value;
  value.set_int64_value(2);
  value.SetExtension(extensions::name, "integer_sum");
  aggregator_manager.AddValueToAggregator("integer_sum", value);
  aggregator_manager.RegisterAggregationFunction("integer_sum2", &function);
  value.set_int64_value(3);
  value.SetExtension(extensions::name, "integer_sum2");
  aggregator_manager.AddValueToAggregator("integer_sum2", value);

  rendero::AggregatorGroup aggregator_group;

  aggregator_manager.OutputAndResetAllAggregators(&aggregator_group);

  // Test if aggregators were cleared
  aggregator_manager.GetAggregatorValue("integer_sum", &value);
  EXPECT_FALSE(value.has_int64_value());
  aggregator_manager.GetAggregatorValue("integer_sum2", &value);
  EXPECT_FALSE(value.has_int64_value());

  for (int i = 0; i < aggregator_group.aggregators_size(); ++i) {
    value = aggregator_group.aggregators(i);
    if (value.GetExtension(extensions::name) == "integer_sum") {
      EXPECT_EQ(7, value.int64_value());
    }
    else if (value.GetExtension(extensions::name) == "integer_sum2") {
      EXPECT_EQ(8, value.int64_value());
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
  value.set_int64_value(2);
  aggregator.Increment(value);
  EXPECT_EQ(7, aggregator.value().int64_value());

  // Try a filled aggregator
  value.set_int64_value(5);
  aggregator_manager.AddValueToAggregator("integer_sum", value);
  aggregator_manager.GetAggregator("integer_sum", &aggregator);
  EXPECT_EQ(10, aggregator.value().int64_value());
  aggregator.Increment(value);
  EXPECT_EQ(15, aggregator.value().int64_value());
}
