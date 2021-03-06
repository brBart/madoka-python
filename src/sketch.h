// Copyright (c) 2012, Susumu Yata
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.

#ifndef MADOKA_SKETCH_H
#define MADOKA_SKETCH_H

#include "approx.h"
#include "file.h"
#include "hash.h"
#include "header.h"
#include "random.h"

typedef madoka_uint64 (*madoka_sketch_filter)(madoka_uint64);

typedef enum {
  MADOKA_SKETCH_EXACT_MODE,
  MADOKA_SKETCH_APPROX_MODE
} madoka_sketch_mode;

typedef struct madoka_sketch_ madoka_sketch;


#ifdef __cplusplus
namespace madoka {

typedef madoka_sketch_filter SketchFilter;

enum SketchMode {
  SKETCH_EXACT_MODE  = MADOKA_SKETCH_EXACT_MODE,
  SKETCH_APPROX_MODE = MADOKA_SKETCH_APPROX_MODE
};

const UInt64 SKETCH_ID_SIZE           = 128 / 3;
const UInt64 SKETCH_MAX_ID            = (1ULL << SKETCH_ID_SIZE) - 1;
const UInt64 SKETCH_ID_MASK           = SKETCH_MAX_ID;

const UInt64 SKETCH_MIN_WIDTH         = 1;
const UInt64 SKETCH_MAX_WIDTH         = SKETCH_MAX_ID + 1;
const UInt64 SKETCH_DEFAULT_WIDTH     = 1ULL << 20;

const UInt64 SKETCH_MAX_MAX_VALUE     = APPROX_MAX_VALUE;
const UInt64 SKETCH_DEFAULT_MAX_VALUE = SKETCH_MAX_MAX_VALUE;

const UInt64 SKETCH_DEPTH             = 3;

const UInt64 SKETCH_APPROX_VALUE_SIZE = APPROX_VALUE_SIZE;

const UInt64 SKETCH_OWNER_OFFSET      = APPROX_SIZE * 3;
const UInt64 SKETCH_OWNER_MASK        = 0x3FULL << SKETCH_OWNER_OFFSET;

class Sketch {
 public:
  typedef SketchFilter Filter;
  typedef SketchMode Mode;

  Sketch() throw();
  ~Sketch() throw();

  void create(UInt64 width = 0, UInt64 max_value = 0,
              const char *path = NULL, int flags = 0,
              UInt64 seed = 0) throw(Exception);
  void open(const char *path, int flags = 0) throw(Exception);
  void close() throw();

  void load(const char *path, int flags = 0) throw(Exception);
  void save(const char *path, int flags = 0) const throw(Exception);

  UInt64 width() const throw() {
    return header().width();
  }
  UInt64 width_mask() const throw() {
    return header().width_mask();
  }
  UInt64 depth() const throw() {
    return SKETCH_DEPTH;
  }
  UInt64 max_value() const throw() {
    return header().max_value();
  }
  UInt64 value_mask() const throw() {
    return header().max_value();
  }
  UInt64 value_size() const throw() {
    return header().value_size();
  }
  UInt64 seed() const throw() {
    return header().seed();
  }
  UInt64 table_size() const throw() {
    return header().table_size();
  }
  UInt64 file_size() const throw() {
    return header().file_size();
  }
  int flags() const throw() {
    return file_.flags();
  }
  Mode mode() const throw() {
    return (value_size() == SKETCH_APPROX_VALUE_SIZE) ?
        SKETCH_APPROX_MODE : SKETCH_EXACT_MODE;
  }

  UInt64 get(const void *key_addr, std::size_t key_size) const throw();
  void set(const void *key_addr, std::size_t key_size, UInt64 value) throw();
  UInt64 inc(const void *key_addr, std::size_t key_size) throw();
  UInt64 add(const void *key_addr, std::size_t key_size, UInt64 value) throw();

  void clear() throw();

  void copy(const Sketch &src, const char *path = NULL,
            int flags = 0) throw(Exception);

  void filter(Filter filter) throw();

  void shrink(const Sketch &src, UInt64 width = 0,
              UInt64 max_value = 0, Filter filter = NULL,
              const char *path = NULL, int flags = 0) throw(Exception);

  void merge(const Sketch &rhs, Filter lhs_filter = NULL,
             Filter rhs_filter = NULL) throw(Exception);

  void swap(Sketch *sketch) throw();

  double inner_product(const Sketch &rhs, double *lhs_square_length = NULL,
      double *rhs_square_length = NULL) const throw(Exception);

  UInt64 get__(UInt64 table_id, UInt64 cell_id) const throw();
  void set__(UInt64 table_id, UInt64 cell_id, UInt64 value) throw();

 private:
  File file_;
  Header *header_;
  Random *random_;
  UInt64 *table_;

  const Header &header() const throw() {
    return *header_;
  }
  Header &header() throw() {
    return *header_;
  }

  void create_(UInt64 width, UInt64 max_value, const char *path,
               int flags, UInt64 seed) throw(Exception);
  void open_(const char *path, int flags) throw(Exception);

  void load_(const char *path, int flags) throw(Exception);

  void check_header() const throw(Exception);

  inline UInt64 get_(UInt64 table_id, UInt64 cell_id) const throw();
  inline void set_(UInt64 table_id, UInt64 cell_id, UInt64 value) throw();

  UInt64 exact_get(const UInt64 cell_ids[3]) const throw();
  void exact_set(const UInt64 cell_ids[3], UInt64 value) throw();
  UInt64 exact_inc(const UInt64 cell_ids[3]) throw();
  UInt64 exact_add(const UInt64 cell_ids[3], UInt64 value) throw();

  inline UInt64 exact_get_(UInt64 cell_id) const throw();
  inline void exact_set_(UInt64 cell_id, UInt64 value) throw();
  inline void exact_set_floor_(UInt64 cell_id, UInt64 value) throw();

  UInt64 approx_get(const UInt64 cell_ids[3]) const throw();
  void approx_set(const UInt64 cell_ids[3], UInt64 value) throw();
  UInt64 approx_inc(const UInt64 cell_ids[3]) throw();
  UInt64 approx_add(const UInt64 cell_ids[3], UInt64 value) throw();

  inline UInt64 approx_get_(UInt64 table_id, UInt64 cell_id) const throw();
  inline void approx_set_(UInt64 table_id, UInt64 cell_id,
                          UInt64 approx) throw();
  inline void approx_set_(UInt64 table_id, UInt64 cell_id,
                          UInt64 approx, UInt64 mask) throw();

  inline void hash(const void *key_addr, std::size_t key_size,
                   UInt64 cell_ids[3]) const throw();

  void copy_(const Sketch &src, const char *path,
             int flags) throw(Exception);

  void exact_merge_(const Sketch &rhs, Filter lhs_filter,
                    Filter rhs_filter) throw();
  void approx_merge_(const Sketch &rhs, Filter lhs_filter,
                     Filter rhs_filter) throw();
  void approx_merge_(const Sketch &rhs) throw();

  void shrink_(const Sketch &src, UInt64 width, UInt64 max_value,
               Filter filter, const char *path, int flags) throw(Exception);

  // Disallows copy and assignment.
  Sketch(const Sketch &);
  Sketch &operator=(const Sketch &);
};

}  // namespace madoka
#endif  // __cplusplus

#endif  // MADOKA_SKETCH_H
