// Copyright (c) 2011 by  Marco Túlio Ribeiro <marcotcr@gmail.com>,
//                        Tiago Alves Macambira <tmacam@burocrata.org>
// All rights reserved.
//
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// Definition of counters.

#ifndef AGGREGATOR_H_ //NOLINT
#define AGGREGATOR_H_
#include <string>
#include <tr1/unordered_map> //NOLINT
#include "./aggregator.pb.h"


class AbstractAggregatorFunction {
 public:
  AbstractAggregatorFunction(const std::string& name,
      const rendero::AggregatorValue& initial_value);

  virtual ~AbstractAggregatorFunction();

  const rendero::AggregatorValue& initial() const;

  // This must be defined by whoever extends the class. The idea is to provide a
  // commutative and associative function that reduces two values.
  virtual void reduce(const rendero::AggregatorValue& partial,
      const rendero::AggregatorValue& value, rendero::AggregatorValue* result)
      const = 0;


 // I need this in order to set the extension value properly.
 protected:
  rendero::AggregatorValue initial_;
 private:
  std::string name_;
};

class AggregatorManager;

class Aggregator {
 public:
  Aggregator();
  void Fill(const std::string& name, AggregatorManager* aggregator_manager);
  void Increment(const rendero::AggregatorValue& value);
  rendero::AggregatorValue value();
 private:
  std::string name_;
  AggregatorManager* aggregator_manager_;
};

class AggregatorManager {
 public:
  AggregatorManager();

  // Each aggregator must register his aggregation function.
  void RegisterAggregationFunction(const std::string& name,
  AbstractAggregatorFunction* function);

  void AddValueToAggregator(const std::string& name,
  const rendero::AggregatorValue& value);

  void OutputAndResetAllAggregators(rendero::AggregatorGroup* output_group);

  // Whoever calls this has ownership of the aggregator instance.
  void GetAggregator(const std::string& name, Aggregator* aggregator);

  void GetAggregatorValue(const std::string& name,
      rendero::AggregatorValue* value);

  private:
  const AbstractAggregatorFunction* aggregator_function_;

  std::tr1::unordered_map<std::string, AbstractAggregatorFunction*>
  reduce_functions_;
  std::tr1::unordered_map<std::string, rendero::AggregatorValue> aggregators_;
  // FIXME:This is not being used yet.
  std::tr1::unordered_map<std::string, rendero::AggregatorValue>
      previous_values_;
};

class AggregatorAggregator {
 public:
  AggregatorAggregator();

  void UpdateAggregators(
      const rendero::AggregatorPartials& aggregator_partials);

  void OutputAndReset(rendero::AggregatorGroup* output_group);

  void RegisterAggregationFunction(const std::string& name,
  AbstractAggregatorFunction* function);

 private:
  std::tr1::unordered_map<std::string, rendero::AggregatorValue> partials_;
  std::tr1::unordered_map<std::string, AbstractAggregatorFunction*>
  reduce_functions_;
};

#endif  // AGGREGATOR_H_
