// BSD 3-Clause License; see https://github.com/jpivarski/awkward-1.0/blob/master/LICENSE

#include <stdexcept>

#include "awkward/Identities.h"
#include "awkward/Index.h"
#include "awkward/array/IndexedArray.h"
#include "awkward/type/OptionType.h"

#include "awkward/builder/OptionBuilder.h"

namespace awkward {
  const BuilderPtr
  OptionBuilder::fromnulls(const ArrayBuilderOptions& options,
                           int64_t nullcount,
                           const BuilderPtr& content) {
    GrowableBuffer<int64_t> index = GrowableBuffer<int64_t>::full(options,
                                                                  -1,
                                                                  nullcount);
    BuilderPtr out = std::make_shared<OptionBuilder>(options,
                                                     index,
                                                     content);
    out.get()->setthat(out);
    return out;
  }

  const BuilderPtr
  OptionBuilder::fromvalids(const ArrayBuilderOptions& options,
                            const BuilderPtr& content) {
    GrowableBuffer<int64_t> index =
      GrowableBuffer<int64_t>::arange(options, content->length());
    BuilderPtr out = std::make_shared<OptionBuilder>(options,
                                                     index,
                                                     content);
    out.get()->setthat(out);
    return out;
  }
 
  OptionBuilder::OptionBuilder(const ArrayBuilderOptions& options,
                               const GrowableBuffer<int64_t>& index,
                               const BuilderPtr& content)
    : options_(options)
      , index_(index)
      , content_(content) { }

  const std::string
  OptionBuilder::classname() const {
    return "OptionBuilder";
  };

  int64_t
  OptionBuilder::length() const {
    return index_.length();
  }

  void
  OptionBuilder::clear() {
    index_.clear();
    content_.get()->clear();
  }

  const ContentPtr
  OptionBuilder::snapshot() const {
    Index64 index(index_.ptr(), 0, index_.length());
    return std::make_shared<IndexedOptionArray64>(Identities::none(),
                                                  util::Parameters(),
                                                  index,
                                                  content_.get()->snapshot());
  }

  bool
  OptionBuilder::active() const {
    return content_.get()->active();
  }

  const BuilderPtr
  OptionBuilder::null() {
    if (!content_.get()->active()) {
      index_.append(-1);
    }
    else {
      content_.get()->null();
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::boolean(bool x) {
    if (!content_.get()->active()) {
      int64_t length = content_.get()->length();
      maybeupdate(content_.get()->boolean(x));
      index_.append(length);
    }
    else {
      content_.get()->boolean(x);
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::integer(int64_t x) {
    if (!content_.get()->active()) {
      int64_t length = content_.get()->length();
      maybeupdate(content_.get()->integer(x));
      index_.append(length);
    }
    else {
      content_.get()->integer(x);
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::real(double x) {
    if (!content_.get()->active()) {
      int64_t length = content_.get()->length();
      maybeupdate(content_.get()->real(x));
      index_.append(length);
    }
    else {
      content_.get()->real(x);
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::string(const char* x, int64_t length, const char* encoding) {
    if (!content_.get()->active()) {
      int64_t len = content_.get()->length();
      maybeupdate(content_.get()->string(x, length, encoding));
      index_.append(len);
    }
    else {
      content_.get()->string(x, length, encoding);
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::beginlist() {
    if (!content_.get()->active()) {
      maybeupdate(content_.get()->beginlist());
    }
    else {
      content_.get()->beginlist();
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::endlist() {
    if (!content_.get()->active()) {
      throw std::invalid_argument(
        "called 'endlist' without 'beginlist' at the same level before it");
    }
    else {
      int64_t length = content_.get()->length();
      content_.get()->endlist();
      if (length != content_.get()->length()) {
        index_.append(length);
      }
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::begintuple(int64_t numfields) {
    if (!content_.get()->active()) {
      maybeupdate(content_.get()->begintuple(numfields));
    }
    else {
      content_.get()->begintuple(numfields);
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::index(int64_t index) {
    if (!content_.get()->active()) {
      throw std::invalid_argument(
        "called 'index' without 'begintuple' at the same level before it");
    }
    else {
      content_.get()->index(index);
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::endtuple() {
    if (!content_.get()->active()) {
      throw std::invalid_argument(
        "called 'endtuple' without 'begintuple' at the same level before it");
    }
    else {
      int64_t length = content_.get()->length();
      content_.get()->endtuple();
      if (length != content_.get()->length()) {
        index_.append(length);
      }
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::beginrecord(const char* name, bool check) {
    if (!content_.get()->active()) {
      maybeupdate(content_.get()->beginrecord(name, check));
    }
    else {
      content_.get()->beginrecord(name, check);
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::field(const char* key, bool check) {
    if (!content_.get()->active()) {
      throw std::invalid_argument(
        "called 'field' without 'beginrecord' at the same level before it");
    }
    else {
      content_.get()->field(key, check);
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::endrecord() {
    if (!content_.get()->active()) {
      throw std::invalid_argument(
        "called 'endrecord' without 'beginrecord' at the same level "
        "before it");
    }
    else {
      int64_t length = content_.get()->length();
      content_.get()->endrecord();
      if (length != content_.get()->length()) {
        index_.append(length);
      }
    }
    return that_;
  }

  const BuilderPtr
  OptionBuilder::append(const ContentPtr& array, int64_t at) {
    if (!content_.get()->active()) {
      int64_t length = content_.get()->length();
      maybeupdate(content_.get()->append(array, at));
      index_.append(length);
    }
    else {
      content_.get()->append(array, at);
    }
    return that_;
  }

  void
  OptionBuilder::maybeupdate(const BuilderPtr& tmp) {
    if (tmp.get() != content_.get()) {
      content_ = tmp;
    }
  }
}
