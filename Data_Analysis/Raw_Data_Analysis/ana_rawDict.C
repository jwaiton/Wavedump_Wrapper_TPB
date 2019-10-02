// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME ana_rawDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "../../Data_Processing/Cooking/TCooker.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *TCooker_Dictionary();
   static void TCooker_TClassManip(TClass*);
   static void *new_TCooker(void *p = 0);
   static void *newArray_TCooker(Long_t size, void *p);
   static void delete_TCooker(void *p);
   static void deleteArray_TCooker(void *p);
   static void destruct_TCooker(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TCooker*)
   {
      ::TCooker *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TCooker));
      static ::ROOT::TGenericClassInfo 
         instance("TCooker", "TCooker.h", 18,
                  typeid(::TCooker), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TCooker_Dictionary, isa_proxy, 0,
                  sizeof(::TCooker) );
      instance.SetNew(&new_TCooker);
      instance.SetNewArray(&newArray_TCooker);
      instance.SetDelete(&delete_TCooker);
      instance.SetDeleteArray(&deleteArray_TCooker);
      instance.SetDestructor(&destruct_TCooker);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TCooker*)
   {
      return GenerateInitInstanceLocal((::TCooker*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TCooker*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TCooker_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TCooker*)0x0)->GetClass();
      TCooker_TClassManip(theClass);
   return theClass;
   }

   static void TCooker_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_TCooker(void *p) {
      return  p ? new(p) ::TCooker : new ::TCooker;
   }
   static void *newArray_TCooker(Long_t nElements, void *p) {
      return p ? new(p) ::TCooker[nElements] : new ::TCooker[nElements];
   }
   // Wrapper around operator delete
   static void delete_TCooker(void *p) {
      delete ((::TCooker*)p);
   }
   static void deleteArray_TCooker(void *p) {
      delete [] ((::TCooker*)p);
   }
   static void destruct_TCooker(void *p) {
      typedef ::TCooker current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TCooker

namespace {
  void TriggerDictionaryInitialization_ana_rawDict_Impl() {
    static const char* headers[] = {
"../../Data_Processing/Cooking/TCooker.h",
0
    };
    static const char* includePaths[] = {
"/usr/local/include",
"/Applications/root_v6.13.02-build/include",
"/Users/gsmith23/Desktop/Watchman/Testing/Wavedump_Wrapper/Data_Analysis/Raw_Data_Analysis/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "ana_rawDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$../../Data_Processing/Cooking/TCooker.h")))  TCooker;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "ana_rawDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "../../Data_Processing/Cooking/TCooker.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"TCooker", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("ana_rawDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_ana_rawDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_ana_rawDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_ana_rawDict() {
  TriggerDictionaryInitialization_ana_rawDict_Impl();
}
