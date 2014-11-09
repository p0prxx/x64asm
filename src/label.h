/*
Copyright 2013 eric schkufza

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef X64ASM_SRC_LABEL_H
#define X64ASM_SRC_LABEL_H

#include <cassert>
#include <iostream>
#include <map>
#include <string>

#include "src/operand.h"

namespace x64asm {

/** A symbolic representation of a Rel32. No Rel8 eqivalent is provided. */
class Label : public Operand {
  public:
    /** Creates a new, globally unique label. */
    Label() {
  		val_ = next_val_++;
		}
    /** Creates a named label. Repeated calls will produce identical results. */
    Label(const std::string& s) {
			auto itr = label2val_.find(s);
			if (itr == label2val_.end()) {
				val_ = next_val_++;
				label2val_[s] = val_;
				val2label_[val_] = s;
			} else {
				val_ = itr->second;
			}
		}

    /** Copy constructor. */
    Label(const Label& rhs) : Operand(0,0) {
  		val_ = rhs.val_;
		}
    /** Move constructor. */
    Label(Label&& rhs) {
			val_ = rhs.val_;
		}
    /** Copy assignment operator. */
    Label& operator=(const Label& rhs) {
			Label(rhs).swap(*this);
			return *this;
		}
    /** Move assignment operator. */
    Label& operator=(Label&& rhs) {
			Label(std::move(rhs)).swap(*this);
			return *this;
		}

    /** Returns true if this label is well-formed. */
    bool check() const {
			return val2label_.find(val_) != val2label_.end();
		}

		/** Returns the text value of this label. */
		const std::string& get_text() const {
			assert(check());
			return val2label_[val_];
		}

    /** Comparison based on label id. */
    bool operator<(const Label& rhs) const {
			return val_ < rhs.val_;
		}
    /** Comparison based on label id. */
    bool operator==(const Label& rhs) const {
			return val_ == rhs.val_;
		}
    /** Comparison based on label id. */
    bool operator!=(const Label& rhs) const {
			return !(*this == rhs);
		}

    /** Conversion based on label value. */
    operator uint64_t() const {
			return val_;
		}

    /** STL-compliant hash. */
    size_t hash() const {
			return val_;
		}
    /** STL-compliant swap. */
    void swap(Label& rhs) {
			std::swap(val_, rhs.val_);
		}

    /** Returns the type of this operand */
    Type type() const { return Type::LABEL; }

		/** @todo This method is undefined. */
		std::istream& read_att(std::istream& is) {
			is.setstate(std::ios::failbit);
			return is;
		}
    /** Writes this label to an ostream using at&t syntax. */
    std::ostream& write_att(std::ostream& os) const {
			assert(check());
			return (os << val2label_[val_]);
		}

  private:
    /** Global map from label text to values. */
    static std::map<std::string, uint64_t> label2val_;
		/** Global map from values back to label text. */
		static std::map<uint64_t, std::string> val2label_;
    /** The next previously unused label value. */
    static uint64_t next_val_;
};

} // namespace x64asm

namespace std {

/** STL hash specialization. */
template <>
struct hash<x64asm::Label> {
  size_t operator()(const x64asm::Label& l) const {
		return l.hash();
	}
};

/** STL swap overload. */
inline void swap(x64asm::Label& lhs, x64asm::Label& rhs) {
	lhs.swap(rhs);
}

/** iostream overload. */
inline istream& operator>>(istream& is, x64asm::Label& l) {
	return l.read_att(is);
}
/** iostream overload. */
inline ostream& operator<<(ostream& os, const x64asm::Label& l) {
	return l.write_att(os);
}

} // namespace std

#endif
