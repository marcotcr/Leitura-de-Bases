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

// Implementation of aggregators.

#include "./aggregator.h"

AbstractAggregatorFunction::AbstractAggregatorFunction(const std::string& name,
    const rendero::AggregatorValue& initial_value): name_(name),
    initial_(initial_value) {
}

AbstractAggregatorFunction::~AbstractAggregatorFunction() {
}

const rendero::AggregatorValue& AbstractAggregatorFunction::initial() const {
  return initial_;
}


// AggregatorManager

AggregatorManager::AggregatorManager() {
}

void AggregatorManager::RegisterAggregationFunction(const std::string& name,
    AbstractAggregatorFunction* function) {
  reduce_functions_[name] = function;
}

void AggregatorManager::AddValueToAggregator(
    const std::string& name, const rendero::AggregatorValue& value) {

  if (reduce_functions_.count(name) == 0) {
    // FAIL SOMEHOW
    return;
  }
  rendero::AggregatorValue temporary_aggregator;
  reduce_functions_[name]->reduce(aggregators_[name], value,
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

void AggregatorManager::GetAggregator(const std::string& name,
    Aggregator* aggregator) {
  aggregator->Fill(name, this);
}

void AggregatorManager::GetAggregatorValue(const std::string& name,
    rendero::AggregatorValue* value) {
  if (aggregators_.count(name) <=0) {
    value = NULL;
  }
  else {
    (*value) = aggregators_[name];
  }
}

Aggregator::Aggregator() {
}

void Aggregator::Fill(const std::string& name,
    AggregatorManager* aggregator_manager) {
  name_ = name;
  aggregator_manager_ = aggregator_manager;
}
void Aggregator::Increment(const rendero::AggregatorValue& value) {
  aggregator_manager_->AddValueToAggregator(name_, value);
}

rendero::AggregatorValue Aggregator::value() {
  rendero::AggregatorValue temp_value;
  aggregator_manager_->GetAggregatorValue(name_, &temp_value);
  return temp_value;
  
}

// Aggregator_aggregator

AggregatorAggregator::AggregatorAggregator() {
}

void AggregatorAggregator::UpdateAggregators(
    const rendero::AggregatorPartials& aggregator_partials) {
  if (reduce_functions_.count(aggregator_partials.name()) == 0) {
    // FAIL SOMEHOW
    return;
  }
  rendero::AggregatorValue temporary_aggregator;
  for (int i = 0; i < aggregator_partials.values().aggregators_size(); ++i) {
    const rendero::AggregatorValue& value =
    aggregator_partials.values().aggregators(i);
    reduce_functions_[aggregator_partials.name()]->reduce(
    partials_[aggregator_partials.name()], value, &temporary_aggregator);
    partials_[aggregator_partials.name()] = temporary_aggregator;
  }
}

void AggregatorAggregator::OutputAndReset(
    rendero::AggregatorGroup* output_group) {
  std::tr1::unordered_map<std::string, rendero::AggregatorValue>::iterator it;
  rendero::AggregatorValue* temp_aggregator;
  for (it = partials_.begin(); it != partials_.end(); ++it) {
    temp_aggregator = output_group->add_aggregators();
    temp_aggregator->CopyFrom(it->second);
    partials_[it->first].Clear();
  }
}

void AggregatorAggregator::RegisterAggregationFunction(const std::string& name,
    AbstractAggregatorFunction* function) {
  reduce_functions_[name] = function;
}
