// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME analyse_rawDict
#define R__NO_DEPRECATION

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

// The generated code does not explicitly qualifies STL entities
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "TRawAnalyser.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *TRawAnalyser_Dictionary();
   static void TRawAnalyser_TClassManip(TClass*);
   static void *new_TRawAnalyser(void *p = 0);
   static void *newArray_TRawAnalyser(Long_t size, void *p);
   static void delete_TRawAnalyser(void *p);
   static void deleteArray_TRawAnalyser(void *p);
   static void destruct_TRawAnalyser(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TRawAnalyser*)
   {
      ::TRawAnalyser *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TRawAnalyser));
      static ::ROOT::TGenericClassInfo 
         instance("TRawAnalyser", "TRawAnalyser.h", 18,
                  typeid(::TRawAnalyser), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TRawAnalyser_Dictionary, isa_proxy, 0,
                  sizeof(::TRawAnalyser) );
      instance.SetNew(&new_TRawAnalyser);
      instance.SetNewArray(&newArray_TRawAnalyser);
      instance.SetDelete(&delete_TRawAnalyser);
      instance.SetDeleteArray(&deleteArray_TRawAnalyser);
      instance.SetDestructor(&destruct_TRawAnalyser);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TRawAnalyser*)
   {
      return GenerateInitInstanceLocal((::TRawAnalyser*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TRawAnalyser*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TRawAnalyser_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TRawAnalyser*)0x0)->GetClass();
      TRawAnalyser_TClassManip(theClass);
   return theClass;
   }

   static void TRawAnalyser_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_TRawAnalyser(void *p) {
      return  p ? new(p) ::TRawAnalyser : new ::TRawAnalyser;
   }
   static void *newArray_TRawAnalyser(Long_t nElements, void *p) {
      return p ? new(p) ::TRawAnalyser[nElements] : new ::TRawAnalyser[nElements];
   }
   // Wrapper around operator delete
   static void delete_TRawAnalyser(void *p) {
      delete ((::TRawAnalyser*)p);
   }
   static void deleteArray_TRawAnalyser(void *p) {
      delete [] ((::TRawAnalyser*)p);
   }
   static void destruct_TRawAnalyser(void *p) {
      typedef ::TRawAnalyser current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TRawAnalyser

namespace {
  void TriggerDictionaryInitialization_analyse_rawDict_Impl() {
    static const char* headers[] = {
"TRawAnalyser.h",
0
    };
    static const char* includePaths[] = {
"/usr/include/root",
"/usr/include/root",
"/scratch/Gary/Wavedump_Wrapper/Data_Analysis/Analyse_Raw/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "analyse_rawDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$TRawAnalyser.h")))  TRawAnalyser;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "analyse_rawDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "TRawAnalyser.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"TRawAnalyser", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("analyse_rawDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_analyse_rawDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_analyse_rawDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_analyse_rawDict() {
  TriggerDictionaryInitialization_analyse_rawDict_Impl();
}
