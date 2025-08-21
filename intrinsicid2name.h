#ifndef DECOMPILER_INTRINSIC2NAME
#define DECOMPILER_INTRINSIC2NAME

namespace panda::compiler {
std::string GetIntrinsicOpcodeName(compiler::RuntimeInterface::IntrinsicId id) {
    switch(id) {
        case compiler::RuntimeInterface::IntrinsicId::LDUNDEFINED: {
            return "ldundefined";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDNULL: {
            return "ldnull";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDTRUE: {
            return "ldtrue";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDFALSE: {
            return "ldfalse";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEEMPTYOBJECT: {
            return "createemptyobject";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEEMPTYARRAY_IMM8: {
            return "createemptyarray";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEARRAYWITHBUFFER_IMM8_ID16: {
            return "createarraywithbuffer";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEOBJECTWITHBUFFER_IMM8_ID16: {
            return "createobjectwithbuffer";
        }
        case compiler::RuntimeInterface::IntrinsicId::NEWOBJRANGE_IMM8_IMM8_V8: {
            return "newobjrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::NEWLEXENV_IMM8: {
            return "newlexenv";
        }
        case compiler::RuntimeInterface::IntrinsicId::ADD2_IMM8_V8: {
            return "add2";
        }
        case compiler::RuntimeInterface::IntrinsicId::SUB2_IMM8_V8: {
            return "sub2";
        }
        case compiler::RuntimeInterface::IntrinsicId::MUL2_IMM8_V8: {
            return "mul2";
        }
        case compiler::RuntimeInterface::IntrinsicId::DIV2_IMM8_V8: {
            return "div2";
        }
        case compiler::RuntimeInterface::IntrinsicId::MOD2_IMM8_V8: {
            return "mod2";
        }
        case compiler::RuntimeInterface::IntrinsicId::EQ_IMM8_V8: {
            return "eq";
        }
        case compiler::RuntimeInterface::IntrinsicId::NOTEQ_IMM8_V8: {
            return "noteq";
        }
        case compiler::RuntimeInterface::IntrinsicId::LESS_IMM8_V8: {
            return "less";
        }
        case compiler::RuntimeInterface::IntrinsicId::LESSEQ_IMM8_V8: {
            return "lesseq";
        }
        case compiler::RuntimeInterface::IntrinsicId::GREATER_IMM8_V8: {
            return "greater";
        }
        case compiler::RuntimeInterface::IntrinsicId::GREATEREQ_IMM8_V8: {
            return "greatereq";
        }
        case compiler::RuntimeInterface::IntrinsicId::SHL2_IMM8_V8: {
            return "shl2";
        }
        case compiler::RuntimeInterface::IntrinsicId::SHR2_IMM8_V8: {
            return "shr2";
        }
        case compiler::RuntimeInterface::IntrinsicId::ASHR2_IMM8_V8: {
            return "ashr2";
        }
        case compiler::RuntimeInterface::IntrinsicId::AND2_IMM8_V8: {
            return "and2";
        }
        case compiler::RuntimeInterface::IntrinsicId::OR2_IMM8_V8: {
            return "or2";
        }
        case compiler::RuntimeInterface::IntrinsicId::XOR2_IMM8_V8: {
            return "xor2";
        }
        case compiler::RuntimeInterface::IntrinsicId::EXP_IMM8_V8: {
            return "exp";
        }
        case compiler::RuntimeInterface::IntrinsicId::TYPEOF_IMM8: {
            return "typeof";
        }
        case compiler::RuntimeInterface::IntrinsicId::TONUMBER_IMM8: {
            return "tonumber";
        }
        case compiler::RuntimeInterface::IntrinsicId::TONUMERIC_IMM8: {
            return "tonumeric";
        }
        case compiler::RuntimeInterface::IntrinsicId::NEG_IMM8: {
            return "neg";
        }
        case compiler::RuntimeInterface::IntrinsicId::NOT_IMM8: {
            return "not";
        }
        case compiler::RuntimeInterface::IntrinsicId::INC_IMM8: {
            return "inc";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEC_IMM8: {
            return "dec";
        }
        case compiler::RuntimeInterface::IntrinsicId::ISTRUE: {
            return "istrue";
        }
        case compiler::RuntimeInterface::IntrinsicId::ISFALSE: {
            return "isfalse";
        }
        case compiler::RuntimeInterface::IntrinsicId::ISIN_IMM8_V8: {
            return "isin";
        }
        case compiler::RuntimeInterface::IntrinsicId::INSTANCEOF_IMM8_V8: {
            return "instanceof";
        }
        case compiler::RuntimeInterface::IntrinsicId::STRICTNOTEQ_IMM8_V8: {
            return "strictnoteq";
        }
        case compiler::RuntimeInterface::IntrinsicId::STRICTEQ_IMM8_V8: {
            return "stricteq";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLARG0_IMM8: {
            return "callarg0";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLARG1_IMM8_V8: {
            return "callarg1";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLARGS2_IMM8_V8_V8: {
            return "callargs2";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLARGS3_IMM8_V8_V8_V8: {
            return "callargs3";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLTHIS0_IMM8_V8: {
            return "callthis0";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLTHIS1_IMM8_V8_V8: {
            return "callthis1";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLTHIS2_IMM8_V8_V8_V8: {
            return "callthis2";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLTHIS3_IMM8_V8_V8_V8_V8: {
            return "callthis3";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLTHISRANGE_IMM8_IMM8_V8: {
            return "callthisrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::SUPERCALLTHISRANGE_IMM8_IMM8_V8: {
            return "supercallthisrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM8_ID16_IMM8: {
            return "definefunc";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM8_ID16_IMM8: {
            return "definemethod";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM8_ID16_ID16_IMM16_V8: {
            return "defineclasswithbuffer";
        }
        case compiler::RuntimeInterface::IntrinsicId::GETNEXTPROPNAME_V8: {
            return "getnextpropname";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDOBJBYVALUE_IMM8_V8: {
            return "ldobjbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOBJBYVALUE_IMM8_V8_V8: {
            return "stobjbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYVALUE_IMM8_V8: {
            return "ldsuperbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDOBJBYINDEX_IMM8_IMM16: {
            return "ldobjbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOBJBYINDEX_IMM8_V8_IMM16: {
            return "stobjbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDLEXVAR_IMM4_IMM4: {
            return "ldlexvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::STLEXVAR_IMM4_IMM4: {
            return "stlexvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::TRYLDGLOBALBYNAME_IMM8_ID16: {
            return "tryldglobalbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::TRYSTGLOBALBYNAME_IMM8_ID16: {
            return "trystglobalbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDGLOBALVAR_IMM16_ID16: {
            return "ldglobalvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDOBJBYNAME_IMM8_ID16: {
            return "ldobjbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOBJBYNAME_IMM8_ID16_V8: {
            return "stobjbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYNAME_IMM8_ID16: {
            return "ldsuperbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::STCONSTTOGLOBALRECORD_IMM16_ID16: {
            return "stconsttoglobalrecord";
        }
        case compiler::RuntimeInterface::IntrinsicId::STTOGLOBALRECORD_IMM16_ID16: {
            return "sttoglobalrecord";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDTHISBYNAME_IMM8_ID16: {
            return "ldthisbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::STTHISBYNAME_IMM8_ID16: {
            return "stthisbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDTHISBYVALUE_IMM8: {
            return "ldthisbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::STTHISBYVALUE_IMM8_V8: {
            return "stthisbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::RETURN: {
            return "return";
        }
        case compiler::RuntimeInterface::IntrinsicId::RETURNUNDEFINED: {
            return "returnundefined";
        }
        case compiler::RuntimeInterface::IntrinsicId::GETPROPITERATOR: {
            return "getpropiterator";
        }
        case compiler::RuntimeInterface::IntrinsicId::GETITERATOR_IMM8: {
            return "getiterator";
        }
        case compiler::RuntimeInterface::IntrinsicId::CLOSEITERATOR_IMM8_V8: {
            return "closeiterator";
        }
        case compiler::RuntimeInterface::IntrinsicId::POPLEXENV: {
            return "poplexenv";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDNAN: {
            return "ldnan";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDINFINITY: {
            return "ldinfinity";
        }
        case compiler::RuntimeInterface::IntrinsicId::GETUNMAPPEDARGS: {
            return "getunmappedargs";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDGLOBAL: {
            return "ldglobal";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDNEWTARGET: {
            return "ldnewtarget";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDTHIS: {
            return "ldthis";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDHOLE: {
            return "ldhole";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEREGEXPWITHLITERAL_IMM8_ID16_IMM8: {
            return "createregexpwithliteral";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEREGEXPWITHLITERAL_IMM16_ID16_IMM8: {
            return "createregexpwithliteral";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRANGE_IMM8_IMM8_V8: {
            return "callrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM16_ID16_IMM8: {
            return "definefunc";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM16_ID16_ID16_IMM16_V8: {
            return "defineclasswithbuffer";
        }
        case compiler::RuntimeInterface::IntrinsicId::GETTEMPLATEOBJECT_IMM8: {
            return "gettemplateobject";
        }
        case compiler::RuntimeInterface::IntrinsicId::SETOBJECTWITHPROTO_IMM8_V8: {
            return "setobjectwithproto";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUE_IMM8_V8_V8: {
            return "stownbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYINDEX_IMM8_V8_IMM16: {
            return "stownbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAME_IMM8_ID16_V8: {
            return "stownbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::GETMODULENAMESPACE_IMM8: {
            return "getmodulenamespace";
        }
        case compiler::RuntimeInterface::IntrinsicId::STMODULEVAR_IMM8: {
            return "stmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDLOCALMODULEVAR_IMM8: {
            return "ldlocalmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDEXTERNALMODULEVAR_IMM8: {
            return "ldexternalmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::STGLOBALVAR_IMM16_ID16: {
            return "stglobalvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEEMPTYARRAY_IMM16: {
            return "createemptyarray";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEARRAYWITHBUFFER_IMM16_ID16: {
            return "createarraywithbuffer";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEOBJECTWITHBUFFER_IMM16_ID16: {
            return "createobjectwithbuffer";
        }
        case compiler::RuntimeInterface::IntrinsicId::NEWOBJRANGE_IMM16_IMM8_V8: {
            return "newobjrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::TYPEOF_IMM16: {
            return "typeof";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDOBJBYVALUE_IMM16_V8: {
            return "ldobjbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOBJBYVALUE_IMM16_V8_V8: {
            return "stobjbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYVALUE_IMM16_V8: {
            return "ldsuperbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDOBJBYINDEX_IMM16_IMM16: {
            return "ldobjbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOBJBYINDEX_IMM16_V8_IMM16: {
            return "stobjbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDLEXVAR_IMM8_IMM8: {
            return "ldlexvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::STLEXVAR_IMM8_IMM8: {
            return "stlexvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::TRYLDGLOBALBYNAME_IMM16_ID16: {
            return "tryldglobalbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::TRYSTGLOBALBYNAME_IMM16_ID16: {
            return "trystglobalbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAMEWITHNAMESET_IMM8_ID16_V8: {
            return "stownbynamewithnameset";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDOBJBYNAME_IMM16_ID16: {
            return "ldobjbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOBJBYNAME_IMM16_ID16_V8: {
            return "stobjbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYNAME_IMM16_ID16: {
            return "ldsuperbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDTHISBYNAME_IMM16_ID16: {
            return "ldthisbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::STTHISBYNAME_IMM16_ID16: {
            return "stthisbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDTHISBYVALUE_IMM16: {
            return "ldthisbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::STTHISBYVALUE_IMM16_V8: {
            return "stthisbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::ASYNCGENERATORREJECT_V8: {
            return "asyncgeneratorreject";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUEWITHNAMESET_IMM8_V8_V8: {
            return "stownbyvaluewithnameset";
        }
        case compiler::RuntimeInterface::IntrinsicId::GETITERATOR_IMM16: {
            return "getiterator";
        }
        case compiler::RuntimeInterface::IntrinsicId::CLOSEITERATOR_IMM16_V8: {
            return "closeiterator";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDSYMBOL: {
            return "ldsymbol";
        }
        case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONENTER: {
            return "asyncfunctionenter";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDFUNCTION: {
            return "ldfunction";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEBUGGER: {
            return "debugger";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEGENERATOROBJ_V8: {
            return "creategeneratorobj";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEITERRESULTOBJ_V8_V8: {
            return "createiterresultobj";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEOBJECTWITHEXCLUDEDKEYS_IMM8_V8_V8: {
            return "createobjectwithexcludedkeys";
        }
        case compiler::RuntimeInterface::IntrinsicId::NEWOBJAPPLY_IMM8_V8: {
            return "newobjapply";
        }
        case compiler::RuntimeInterface::IntrinsicId::NEWOBJAPPLY_IMM16_V8: {
            return "newobjapply";
        }
        case compiler::RuntimeInterface::IntrinsicId::NEWLEXENVWITHNAME_IMM8_ID16: {
            return "newlexenvwithname";
        }
        case compiler::RuntimeInterface::IntrinsicId::CREATEASYNCGENERATOROBJ_V8: {
            return "createasyncgeneratorobj";
        }
        case compiler::RuntimeInterface::IntrinsicId::ASYNCGENERATORRESOLVE_V8_V8_V8: {
            return "asyncgeneratorresolve";
        }
        case compiler::RuntimeInterface::IntrinsicId::SUPERCALLSPREAD_IMM8_V8: {
            return "supercallspread";
        }
        case compiler::RuntimeInterface::IntrinsicId::APPLY_IMM8_V8_V8: {
            return "apply";
        }
        case compiler::RuntimeInterface::IntrinsicId::SUPERCALLARROWRANGE_IMM8_IMM8_V8: {
            return "supercallarrowrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEFINEGETTERSETTERBYVALUE_V8_V8_V8_V8: {
            return "definegettersetterbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::DYNAMICIMPORT: {
            return "dynamicimport";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM16_ID16_IMM8: {
            return "definemethod";
        }
        case compiler::RuntimeInterface::IntrinsicId::RESUMEGENERATOR: {
            return "resumegenerator";
        }
        case compiler::RuntimeInterface::IntrinsicId::GETRESUMEMODE: {
            return "getresumemode";
        }
        case compiler::RuntimeInterface::IntrinsicId::GETTEMPLATEOBJECT_IMM16: {
            return "gettemplateobject";
        }
        case compiler::RuntimeInterface::IntrinsicId::DELOBJPROP_V8: {
            return "delobjprop";
        }
        case compiler::RuntimeInterface::IntrinsicId::SUSPENDGENERATOR_V8: {
            return "suspendgenerator";
        }
        case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONAWAITUNCAUGHT_V8: {
            return "asyncfunctionawaituncaught";
        }
        case compiler::RuntimeInterface::IntrinsicId::COPYDATAPROPERTIES_V8: {
            return "copydataproperties";
        }
        case compiler::RuntimeInterface::IntrinsicId::STARRAYSPREAD_V8_V8: {
            return "starrayspread";
        }
        case compiler::RuntimeInterface::IntrinsicId::SETOBJECTWITHPROTO_IMM16_V8: {
            return "setobjectwithproto";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUE_IMM16_V8_V8: {
            return "stownbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::STSUPERBYVALUE_IMM8_V8_V8: {
            return "stsuperbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::STSUPERBYVALUE_IMM16_V8_V8: {
            return "stsuperbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYINDEX_IMM16_V8_IMM16: {
            return "stownbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAME_IMM16_ID16_V8: {
            return "stownbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONRESOLVE_V8: {
            return "asyncfunctionresolve";
        }
        case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONREJECT_V8: {
            return "asyncfunctionreject";
        }
        case compiler::RuntimeInterface::IntrinsicId::COPYRESTARGS_IMM8: {
            return "copyrestargs";
        }
        case compiler::RuntimeInterface::IntrinsicId::STSUPERBYNAME_IMM8_ID16_V8: {
            return "stsuperbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::STSUPERBYNAME_IMM16_ID16_V8: {
            return "stsuperbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUEWITHNAMESET_IMM16_V8_V8: {
            return "stownbyvaluewithnameset";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDBIGINT_ID16: {
            return "ldbigint";
        }
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAMEWITHNAMESET_IMM16_ID16_V8: {
            return "stownbynamewithnameset";
        }
        case compiler::RuntimeInterface::IntrinsicId::SETGENERATORSTATE_IMM8: {
            return "setgeneratorstate";
        }
        case compiler::RuntimeInterface::IntrinsicId::GETASYNCITERATOR_IMM8: {
            return "getasynciterator";
        }
        case compiler::RuntimeInterface::IntrinsicId::LDPRIVATEPROPERTY_IMM8_IMM16_IMM16: {
            return "ldprivateproperty";
        }
        case compiler::RuntimeInterface::IntrinsicId::STPRIVATEPROPERTY_IMM8_IMM16_IMM16_V8: {
            return "stprivateproperty";
        }
        case compiler::RuntimeInterface::IntrinsicId::TESTIN_IMM8_IMM16_IMM16: {
            return "testin";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEFINEFIELDBYNAME_IMM8_ID16_V8: {
            return "definefieldbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEFINEPROPERTYBYNAME_IMM8_ID16_V8: {
            return "definepropertybyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_NOTIFYCONCURRENTRESULT_PREF_NONE: {
            return "callruntime.notifyconcurrentresult";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_LDLEXENV_PREF_NONE: {
            return "deprecated.ldlexenv";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_CREATEOBJECTWITHEXCLUDEDKEYS_PREF_IMM16_V8_V8: {
            return "wide.createobjectwithexcludedkeys";
        }
        case compiler::RuntimeInterface::IntrinsicId::THROW_PREF_NONE: {
            return "throw";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINEFIELDBYVALUE_PREF_IMM8_V8_V8: {
            return "callruntime.definefieldbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_POPLEXENV_PREF_NONE: {
            return "deprecated.poplexenv";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_NEWOBJRANGE_PREF_IMM16_V8: {
            return "wide.newobjrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::THROW_NOTEXISTS_PREF_NONE: {
            return "throw.notexists";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINEFIELDBYINDEX_PREF_IMM8_IMM32_V8: {
            return "callruntime.definefieldbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_GETITERATORNEXT_PREF_V8_V8: {
            return "deprecated.getiteratornext";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_NEWLEXENV_PREF_IMM16: {
            return "wide.newlexenv";
        }
        case compiler::RuntimeInterface::IntrinsicId::THROW_PATTERNNONCOERCIBLE_PREF_NONE: {
            return "throw.patternnoncoercible";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_TOPROPERTYKEY_PREF_NONE: {
            return "callruntime.topropertykey";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_CREATEARRAYWITHBUFFER_PREF_IMM16: {
            return "deprecated.createarraywithbuffer";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_NEWLEXENVWITHNAME_PREF_IMM16_ID16: {
            return "wide.newlexenvwithname";
        }
        case compiler::RuntimeInterface::IntrinsicId::THROW_DELETESUPERPROPERTY_PREF_NONE: {
            return "throw.deletesuperproperty";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_CREATEPRIVATEPROPERTY_PREF_IMM16_ID16: {
            return "callruntime.createprivateproperty";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_CREATEOBJECTWITHBUFFER_PREF_IMM16: {
            return "deprecated.createobjectwithbuffer";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_CALLRANGE_PREF_IMM16_V8: {
            return "wide.callrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::THROW_CONSTASSIGNMENT_PREF_V8: {
            return "throw.constassignment";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINEPRIVATEPROPERTY_PREF_IMM8_IMM16_IMM16_V8: {
            return "callruntime.defineprivateproperty";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_TONUMBER_PREF_V8: {
            return "deprecated.tonumber";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_CALLTHISRANGE_PREF_IMM16_V8: {
            return "wide.callthisrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::THROW_IFNOTOBJECT_PREF_V8: {
            return "throw.ifnotobject";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_CALLINIT_PREF_IMM8_V8: {
            return "callruntime.callinit";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_TONUMERIC_PREF_V8: {
            return "deprecated.tonumeric";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_SUPERCALLTHISRANGE_PREF_IMM16_V8: {
            return "wide.supercallthisrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::THROW_UNDEFINEDIFHOLE_PREF_V8_V8: {
            return "throw.undefinedifhole";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINESENDABLECLASS_PREF_IMM16_ID16_ID16_IMM16_V8: {
            return "callruntime.definesendableclass";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_NEG_PREF_V8: {
            return "deprecated.neg";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_SUPERCALLARROWRANGE_PREF_IMM16_V8: {
            return "wide.supercallarrowrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::THROW_IFSUPERNOTCORRECTCALL_PREF_IMM8: {
            return "throw.ifsupernotcorrectcall";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLECLASS_PREF_IMM16: {
            return "callruntime.ldsendableclass";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_NOT_PREF_V8: {
            return "deprecated.not";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_LDOBJBYINDEX_PREF_IMM32: {
            return "wide.ldobjbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::THROW_IFSUPERNOTCORRECTCALL_PREF_IMM16: {
            return "throw.ifsupernotcorrectcall";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLEEXTERNALMODULEVAR_PREF_IMM8: {
            return "callruntime.ldsendableexternalmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_INC_PREF_V8: {
            return "deprecated.inc";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_STOBJBYINDEX_PREF_V8_IMM32: {
            return "wide.stobjbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::THROW_UNDEFINEDIFHOLEWITHNAME_PREF_ID16: {
            return "throw.undefinedifholewithname";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDSENDABLEEXTERNALMODULEVAR_PREF_IMM16: {
            return "callruntime.wideldsendableexternalmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_DEC_PREF_V8: {
            return "deprecated.dec";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_STOWNBYINDEX_PREF_V8_IMM32: {
            return "wide.stownbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_NEWSENDABLEENV_PREF_IMM8: {
            return "callruntime.newsendableenv";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_CALLARG0_PREF_V8: {
            return "deprecated.callarg0";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_COPYRESTARGS_PREF_IMM16: {
            return "wide.copyrestargs";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDENEWSENDABLEENV_PREF_IMM16: {
            return "callruntime.widenewsendableenv";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_CALLARG1_PREF_V8_V8: {
            return "deprecated.callarg1";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_LDLEXVAR_PREF_IMM16_IMM16: {
            return "wide.ldlexvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_STSENDABLEVAR_PREF_IMM4_IMM4: {
            return "callruntime.stsendablevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_CALLARGS2_PREF_V8_V8_V8: {
            return "deprecated.callargs2";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_STLEXVAR_PREF_IMM16_IMM16: {
            return "wide.stlexvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_STSENDABLEVAR_PREF_IMM8_IMM8: {
            return "callruntime.stsendablevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_CALLARGS3_PREF_V8_V8_V8_V8: {
            return "deprecated.callargs3";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_GETMODULENAMESPACE_PREF_IMM16: {
            return "wide.getmodulenamespace";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDESTSENDABLEVAR_PREF_IMM16_IMM16: {
            return "callruntime.widestsendablevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_CALLRANGE_PREF_IMM16_V8: {
            return "deprecated.callrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_STMODULEVAR_PREF_IMM16: {
            return "wide.stmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLEVAR_PREF_IMM4_IMM4: {
            return "callruntime.ldsendablevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_CALLSPREAD_PREF_V8_V8_V8: {
            return "deprecated.callspread";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_LDLOCALMODULEVAR_PREF_IMM16: {
            return "wide.ldlocalmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLEVAR_PREF_IMM8_IMM8: {
            return "callruntime.ldsendablevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_CALLTHISRANGE_PREF_IMM16_V8: {
            return "deprecated.callthisrange";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_LDEXTERNALMODULEVAR_PREF_IMM16: {
            return "wide.ldexternalmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDSENDABLEVAR_PREF_IMM16_IMM16: {
            return "callruntime.wideldsendablevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_DEFINECLASSWITHBUFFER_PREF_ID16_IMM16_IMM16_V8_V8: {
            return "deprecated.defineclasswithbuffer";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_LDPATCHVAR_PREF_IMM16: {
            return "wide.ldpatchvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_ISTRUE_PREF_IMM8: {
            return "callruntime.istrue";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_RESUMEGENERATOR_PREF_V8: {
            return "deprecated.resumegenerator";
        }
        case compiler::RuntimeInterface::IntrinsicId::WIDE_STPATCHVAR_PREF_IMM16: {
            return "wide.stpatchvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_ISFALSE_PREF_IMM8: {
            return "callruntime.isfalse";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_GETRESUMEMODE_PREF_V8: {
            return "deprecated.getresumemode";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDLAZYMODULEVAR_PREF_IMM8: {
            return "callruntime.ldlazymodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_GETTEMPLATEOBJECT_PREF_V8: {
            return "deprecated.gettemplateobject";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDLAZYMODULEVAR_PREF_IMM16: {
            return "callruntime.wideldlazymodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_DELOBJPROP_PREF_V8_V8: {
            return "deprecated.delobjprop";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDLAZYSENDABLEMODULEVAR_PREF_IMM8: {
            return "callruntime.ldlazysendablemodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_SUSPENDGENERATOR_PREF_V8_V8: {
            return "deprecated.suspendgenerator";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDLAZYSENDABLEMODULEVAR_PREF_IMM16: {
            return "callruntime.wideldlazysendablemodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_ASYNCFUNCTIONAWAITUNCAUGHT_PREF_V8_V8: {
            return "deprecated.asyncfunctionawaituncaught";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_SUPERCALLFORWARDALLARGS_PREF_V8: {
            return "callruntime.supercallforwardallargs";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_COPYDATAPROPERTIES_PREF_V8_V8: {
            return "deprecated.copydataproperties";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLELOCALMODULEVAR_PREF_IMM8: {
            return "callruntime.ldsendablelocalmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_SETOBJECTWITHPROTO_PREF_V8_V8: {
            return "deprecated.setobjectwithproto";
        }
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDSENDABLELOCALMODULEVAR_PREF_IMM16: {
            return "callruntime.wideldsendablelocalmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_LDOBJBYVALUE_PREF_V8_V8: {
            return "deprecated.ldobjbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_LDSUPERBYVALUE_PREF_V8_V8: {
            return "deprecated.ldsuperbyvalue";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_LDOBJBYINDEX_PREF_V8_IMM32: {
            return "deprecated.ldobjbyindex";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_ASYNCFUNCTIONRESOLVE_PREF_V8_V8_V8: {
            return "deprecated.asyncfunctionresolve";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_ASYNCFUNCTIONREJECT_PREF_V8_V8_V8: {
            return "deprecated.asyncfunctionreject";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_STLEXVAR_PREF_IMM4_IMM4_V8: {
            return "deprecated.stlexvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_STLEXVAR_PREF_IMM8_IMM8_V8: {
            return "deprecated.stlexvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_STLEXVAR_PREF_IMM16_IMM16_V8: {
            return "deprecated.stlexvar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_GETMODULENAMESPACE_PREF_ID32: {
            return "deprecated.getmodulenamespace";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_STMODULEVAR_PREF_ID32: {
            return "deprecated.stmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_LDOBJBYNAME_PREF_ID32_V8: {
            return "deprecated.ldobjbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_LDSUPERBYNAME_PREF_ID32_V8: {
            return "deprecated.ldsuperbyname";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_LDMODULEVAR_PREF_ID32_IMM8: {
            return "deprecated.ldmodulevar";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_STCONSTTOGLOBALRECORD_PREF_ID32: {
            return "deprecated.stconsttoglobalrecord";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_STLETTOGLOBALRECORD_PREF_ID32: {
            return "deprecated.stlettoglobalrecord";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_STCLASSTOGLOBALRECORD_PREF_ID32: {
            return "deprecated.stclasstoglobalrecord";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_LDHOMEOBJECT_PREF_NONE: {
            return "deprecated.ldhomeobject";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_CREATEOBJECTHAVINGMETHOD_PREF_IMM16: {
            return "deprecated.createobjecthavingmethod";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_DYNAMICIMPORT_PREF_V8: {
            return "deprecated.dynamicimport";
        }
        case compiler::RuntimeInterface::IntrinsicId::DEPRECATED_ASYNCGENERATORREJECT_PREF_V8_V8: {
            return "deprecated.asyncgeneratorreject";
        }
        default: {
            return "";
        }
    }
}
}

#endif