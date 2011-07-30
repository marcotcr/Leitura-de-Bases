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

// Implementation of counters.

#include "aggregator.h"

AbstractAggregatorFunction::AbstractAggregatorFunction(std::string name,
rendero::AggregatorValue initial_value): name_(name), initial_(initial_value) {
}

rendero::AggregatorValue AbstractAggregatorFunction::initial() {
  return initial_;
}


AggregatorManager::AggregatorManager(const AbstractAggregatorFunction*
aggregator_function): aggregator_function_(aggregator_function) {
}

void AggregatorManager::AddValueToAggregator(const std::string& name,
const rendero::AggregatorValue& value) {
  rendero::AggregatorValue temporary_aggregator;
  aggregator_function_->reduce(aggregators_[name], value,
  &temporary_aggregator);
  aggregators_[name] = temporary_aggregator;
}

void AggregatorManager::OutputAndResetAllAggregators(
rendero::AggregatorGroup* output_group) {
  std::tr1::unordered_map<std::string, rendero::AggregatorValue>::iterator it;
  rendero::AggregatorValue* temp_aggregator; 
  for (it = aggregators_.begin(); it != aggregators_.end(); ++it) {
    temp_aggregator = output_group->add_aggregators();
    temp_aggregator->CopyFrom(it->second);
    aggregators_[it->first].Clear();
  }
}

void AggregatorManager::GetAggregator(std::string name,
Aggregator* aggregator) {
  aggregator->Fill(name, this);
}

void Aggregator::Fill(std::string name, AggregatorManager* aggregator_manager) {
  name_ = name;
  aggregator_manager_ = aggregator_manager;
}
void Aggregator::Increment(const rendero::AggregatorValue& value) {
  aggregator_manager_->AddValueToAggregator(name_, value);
}
