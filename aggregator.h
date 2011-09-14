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
