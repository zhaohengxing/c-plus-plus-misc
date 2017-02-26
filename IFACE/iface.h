/*
Copyright (c) 2017 Walter William Karas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
*/

#ifndef IFACE_20170223
#define IFACE_20170223

#define IFACE_NO_PARAMS(P) P(..., ) 

#define IFACE_DEF(IF_NAME, CV, FUNC_LIST) \
 \
struct IF_NAME \
  { \
    /* Do not access the contents of this directly */ \
 \
    struct Vstruct \
      { \
        FUNC_LIST(IFACE_IMPL_FUNC_POINTER, IFACE_IMPL_FUNC_NR_POINTER) \
 \
        Iface_impl::Class_id class_id; \
      }; \
 \
    void CV * const this_; \
 \
    const Vstruct * const vptr; \
 \
    IF_NAME(void *t, const Vstruct *vptr_) : this_(t), vptr(vptr_) { } \
  };

#define IFACE_ENABLE(IF_SPEC, FUNC_LIST, CLS_SPEC) \
 \
namespace Iface_impl \
{ \
 \
template <> \
class Enable<IF_SPEC, CLS_SPEC> \
  { \
  private: \
 \
    using Cls = CLS_SPEC; \
 \
    FUNC_LIST(IFACE_IMPL_THUNK, IFACE_IMPL_THUNK_NR) \
 \
  public: \
 \
    static const IF_SPEC::Vstruct * vptr() \
      { \
        static const IF_SPEC::Vstruct v = \
          { \
            FUNC_LIST(IFACE_IMPL_ENB_FUNC_ADDR, \
                      IFACE_IMPL_ENB_FUNC_ADDR_NR) \
 \
            id<CLS_SPEC>() \
          }; \
 \
        return(&v); \
      } \
  }; \
 \
}

#define IFACE_CONVERSION(DEST_IF_SPEC, DEST_FUNC_LIST, SRC_IF_SPEC) \
 \
namespace Iface_impl \
{ \
 \
template <> \
struct Convert<DEST_IF_SPEC, SRC_IF_SPEC> \
  { \
  public: \
 \
    static const DEST_IF_SPEC::Vstruct * vptr( \
      const SRC_IF_SPEC::Vstruct *src_vptr) \
      { \
        Convert_key k(id<DEST_IF_SPEC>(), src_vptr->class_id); \
 \
        auto map_iter = convert_map().find(k); \
 \
        if (map_iter != convert_map().end()) \
          return(static_cast<const DEST_IF_SPEC::Vstruct *>( \
            map_iter->second)); \
        else \
          { \
            DEST_IF_SPEC::Vstruct *vp = new DEST_IF_SPEC::Vstruct; \
 \
            DEST_FUNC_LIST(IFACE_IMPL_CONVERT, IFACE_IMPL_CONVERT_NR) \
 \
            vp->class_id = src_vptr->class_id; \
 \
            convert_map()[k] = vp; \
 \
            return(vp); \
          }; \
      } \
  }; \
 \
}

#define IFACE_CALL(IF_INST, NAME, ...) \
(IF_INST).vptr->NAME((IF_INST).this_, __VA_ARGS__)

#define IFACE_CALL_NP(IF_INST, NAME) (IF_INST).vptr->NAME((IF_INST).this_)

namespace Iface_impl
{

template <class Iface, class Target>
class Enable;

template <class Dest_iface, class Src_iface>
class Convert;

} // end namespace Iface_impl

template <class Iface, class Target>
Iface iface_factory(Target &t)
  { return(Iface(&t, Iface_impl::Enable<Iface, Target>::vptr())); }

template <class Dest_iface, class Src_iface>
Dest_iface iface_convert(Src_iface src_if)
  {
    return(
      Dest_iface(
        src_if->this_,
        Iface_impl::Convert<Dest_iface, Src_iface>::vptr(src_if.vptr)));
  }

// ----- Private macros (don't use directly) --------------------------

#define IFACE_IMPL_FUNC_POINTER(TYPE, NAME, CV, PARAMS) \
TYPE (*NAME) (CV void *this_, PARAMS(IFACE_IMPL_FULL_PARAM) );

#define IFACE_IMPL_FUNC_NR_POINTER(NAME, CV, PARAMS) \
IFACE_IMPL_FUNC_POINTER(void, NAME, CV, PARAMS)

#define IFACE_IMPL_THUNK(TYPE, NAME, CV, PARAMS) \
static TYPE NAME(CV void *this_, PARAMS(IFACE_IMPL_FULL_PARAM) ) \
  { \
    return( \
      static_cast<CV Cls *>(this_)->NAME( PARAMS(IFACE_IMPL_PARAM_NAME) )); \
  }

#define IFACE_IMPL_THUNK_NR(NAME, CV, PARAMS) \
static void NAME(void *this_, PARAMS(IFACE_IMPL_FULL_PARAM) ) \
  { \
    static_cast<CV Cls *>(this_)->NAME( PARAMS(IFACE_IMPL_PARAM_NAME) ); \
  }

#define IFACE_IMPL_CONVERT(TYPE, NAME, CV, PARAMS) \
vp->NAME = src_vptr->NAME;

#define IFACE_IMPL_CONVERT_NR(NAME, CV, PARAMS) \
IFACE_IMPL_CONVERT(, NAME, , PARAMS)

#define IFACE_IMPL_ENB_FUNC_ADDR(TYPE, NAME, CV, PARAMS) NAME,

#define IFACE_IMPL_ENB_FUNC_ADDR_NR(NAME, CV, PARAMS) NAME,

#define IFACE_IMPL_FULL_PARAM(TYPE, NAME) TYPE NAME

#define IFACE_IMPL_PARAM_NAME(TYPE, NAME) NAME

#include <functional>
#include <unordered_map>

namespace Iface_impl
{

template <class C>
int * id()
  {
    // Dummy, purpose is to provide a unique address for each class C.
    static int i;

    return(&i);
  }

using Class_id = int *;

using Iface_id = int *;

class Convert_key
  {
  private:

    // ID of interface resulting from interface conversion.
    const Iface_id dest_iface_id;

    // Class of instance interfaced to.
    const Class_id class_id;

  public:

    Convert_key(Iface_id i, Class_id c) : dest_iface_id(i), class_id(c) { }

    // Hopefully never a loss of nominal or actual precision.
    long long hash_me() const { return(dest_iface_id - class_id); }

    friend bool operator == (Convert_key a, Convert_key b)
      { return((a.dest_iface_id == b.dest_iface_id) and
               (a.class_id == b.class_id)); }
  };

} // end namespace Iface_impl

namespace std
{

template<>
struct hash<Iface_impl::Convert_key>
  {
    using argument_type = Iface_impl::Convert_key;

    using result_type = std::size_t;

    result_type operator()(const argument_type &k) const
      {
        return(std::hash<long long>()(k.hash_me()));
      }
  };

} // end namespace std

namespace Iface_impl
{

using Convert_map = std::unordered_map<Convert_key, const void *>;

// Map to get vpointer in interface conversion.
inline Convert_map & convert_map()
  {
    static Convert_map m;

    return(m);
  }

} // end namespace Iface_impl

#endif // Include once.
