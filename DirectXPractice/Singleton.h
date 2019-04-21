#pragma once

//#####################
//Singleton - Documentation
//#####################
//Declare the class and add DECLARE_SINGLETON_SIMPLE(_Classname) macro inside. Creates one with default constructor desctructor. 
//More complex variants can be seen below. 
//Remember to define your constructor and destructor


//Declare singleton methods without constructor and destructor, but takes arguments for the constructor
//NC_ND- No constructor destructor
//CC - CallingConvention
#define DECLARE_SINGLETON_BASE_NC_ND_CC(_Classname, _CallConv, ...) \
public: \
	_Classname(const _Classname&) = delete; \
	_Classname& operator=(const _Classname&) = delete; \
	\
	_CallConv static _Classname& GetInstance() \
	{ \
		static _Classname instance(__VA_ARGS__); \
		return instance; \
		\
	} \
    _CallConv static _Classname* GetInstancePtr() \
	{ \
		return &GetInstance(); \
		\
	}

//Defines default private constructor and destructor for the singleton
//DPC - Default private constructor
//DPC - Default private desctructor
#define DECLARE_SINGLETON_DPC_DPD(_Classname) \
private: \
	_Classname() = default; \
	~_Classname() = default;

//Defines parameterized private constructor and default destructor for the singleton
//PPC - Parameterized private constructor
//DPC - Default private desctructor
#define DECLARE_SINGLETON_PPC_DPD(_Classname, ...) \
private: \
	_Classname(__VA_ARGS__); \
	~_Classname();

//Declares singleton with default private constructor + destructor. Allows you to specify the calling convention for it.
#define DECLARE_SINGLETON_SIMPLE_CC(_Classname, _CallConv) DECLARE_SINGLETON_BASE_NC_ND_CC(_Classname, _CallConv, ) DECLARE_SINGLETON_DPC_DPD(_Classname)

//Declares singleton with default private constructor + destructor
#define DECLARE_SINGLETON_SIMPLE(_Classname) DECLARE_SINGLETON_BASE_NC_ND_CC(_Classname, , ) DECLARE_SINGLETON_DPC_DPD(_Classname)