#pragma once
#include "TString.h"
#include <thread>

using return_mode = enum class return_mode
{
    rm_regular,     // Nothing special
    rm_break,       // Break was detected
    rm_continue,    // Continue was detected
    rm_return,       // Return was detected
    rm_super_return, // Return was detected, and the caller should also return
    rm_yield        // Similar to Return, but function is expected to resume from the yield point
};

class _TREC_LIB_DLL ReturnObject
{
public:
    TString errorMessage;               // String Information
    TrecPointer<TVariable> errorObject; // A Variable Generated
    TrecPointer<TVariable> errorObject2;// Backup Variable
    USHORT returnCode;                  // 0 if working, error otherwise
    TDataArray<TString> stackTrace;     // stack trace info
    return_mode mode;                   // Mode, the context in which an operation ended
    USHORT nextCount;                   // How many Statements were part of a single Expression (statement and next statement)

    ReturnObject();

    ReturnObject(const ReturnObject& copy);

    ReturnObject& operator=(const ReturnObject& copy);

    const static USHORT ERR_NO_ERROR = 0;
    const static USHORT ERR_BRACKET_MISMATCH = 1;
    const static USHORT ERR_PARENTH_MISMATCH = 2;
    const static USHORT ERR_NOT_NUMBER = 3;
    const static USHORT ERR_TOO_FEW_PARMS = 4;
    const static USHORT ERR_EXISTING_VAR = 5;
    const static USHORT ERR_UNDEFINED = 6;
    const static USHORT ERR_BROKEN_REF = 7;
    const static USHORT ERR_INCOMPLETE_STATEMENT = 8;
    const static USHORT ERR_IMPROPER_TYPE = 9;
    const static USHORT ERR_IMPROPER_NAME = 10;
    const static USHORT ERR_UNEXPECTED_TOK = 11;
    const static USHORT ERR_UNSUPPORTED_OP = 12;
    const static USHORT ERR_INVALID_FILE_PARAM = 13;

    const static USHORT ERR_THROWN = 19;
    const static USHORT ERR_GENERIC_ERROR = 20;
    const static USHORT ERR_INTERNAL = 21;          // Indicates an Issue that the Interpretor has detected witin itself

};

class _TREC_LIB_DLL TcRunner :
    public TVariable
{
public:
    var_type GetVarType() override;

    TcRunner() = default;

    virtual bool Blocks(); // Whether calling Run on this Runner will block until the task is complete

    virtual void Run(ReturnObject& ret);

    virtual void Pause();

    virtual void Resume();

    virtual void Stop();

    UINT GetSize(void)override;
    TrecPointer<TVariable> ToString(TrecPointer<TVariable>)override;

    TrecPointer<TVariable> ToString()override;
    UINT Get4Value(void);
};

using thread_state = enum class thread_state
{
    pre_run,
    running,
    paused,
    finished,
    stopped
};

using tc_async_runner_state = enum class tc_async_runner_state
{
    inactive,
    running,
    paused
};

class _TREC_LIB_DLL TcAsyncRunner : public TcRunner
{
    friend class ThreadBridge;

public:
    static ULONG64 GetTimeMilli();

    ~TcAsyncRunner();

    class _TREC_LIB_DLL ThreadBridge : public TObject
    {
        friend class TcAsyncRunner;
        thread_state state;
        TrecPointer<TcAsyncRunner> runner;
        DWORD id;
        bool deletable;
    public:
        ThreadBridge();
        void Run();
        thread_state GetState();
        void SetState(thread_state state);
        bool CanDelete();
    };

    bool Blocks()override;

    void Run(ReturnObject& ret)override;

    void Pause()override;

    void Resume()override;

    void Stop()override;

    tc_async_runner_state GetCurrentState();
protected:
    // Should Return true once it is "done" or no more rounds are needed
    virtual bool RunRound() = 0;

    TrecPointer<ThreadBridge> bridge;
};


using procedure_type = enum class procedure_type
{
    object,
    binary,
    other
};

class _TREC_LIB_DLL TcProcedure : public TVariable
{
protected:
    TString name;
    TrecPointer<TVariable> stringReturn;
public:
    TcProcedure() = default;
    TcProcedure(const TString& name);
    
    var_type GetVarType() override;
    virtual procedure_type GetProcedureType() = 0;

    virtual TrecPointer<TVariable> ToString() override;

    virtual TrecPointer<TVariable> ToString(TrecPointer<TVariable> detail) override;

    virtual UINT Get4Value() override;

    virtual UINT GetSize() override;
};