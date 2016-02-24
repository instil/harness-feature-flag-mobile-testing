
// Standard C++ Headers
#include <sstream>
  using std::ostringstream;
#include <stdexcept>
  using std::runtime_error;
#include <system_error>
  using std::system_error;
#include <utility>
  using std::move;

// TycoUtility headers
#include "Thread.h"
  using TycoUtility::Runnable;
  using TycoUtility::Thread;
  using std::thread;         // from included <thread>

Thread::~Thread()
{
    try
    {
        // Need to have !m_thread.joinable() so that m_thread
        // destructor can execute safely.

        // Assert !m_isRunning [precondition of this function]

        EnsureNotJoinable();

        // Assert !m_thread.joinable()
    }
    catch (...)
    {
        // Swallow this, as destructor must not throw exception.
    }
}

void Thread::Execute(Runnable& runnable)
{
    try
    {
        // Start thread executing in temporary thread object.
        thread t{[&]
                 { m_isRunning = true; runnable.Run(); m_isRunning = false; }};

        // Want to move execution from t to m_thread
        // (so can exit this function) but to do that
        // need to have !m_thread.joinable().

        // Assert !m_isRunning [precondition of this function]

        EnsureNotJoinable();

        // Assert !m_thread.joinable()

        m_thread = move(t);
    }
    catch (system_error e)
    {
        ostringstream message;
        message << "Thread::Execute(): failed: error code = " << e.code();
        throw runtime_error{message.str()};
    }
}

void Thread::WaitUntilStopped()
{
    try
    {
        if (m_thread.joinable())
        {
            // m_thread is an active thread of execution,
            // so wait for it to finish.
            m_thread.join();
        }
        // else
        //     m_thread is not associated with any thread of execution
        //     so no need to do anything.
    }
    catch (system_error e)
    {
        ostringstream message;
        message << "Thread::WaitUntilStopped(): failed: "
                   "error code = " << e.what();
        throw runtime_error{message.str()};
    }
}

void Thread::EnsureNotJoinable()
{
    // Assert !m_isRunning [precondition of this function]
    //
    // This implies that execution never started or if it 
    // did that it has finished.

    // Even though execution has finished m_thread might not
    // have been joined, in which case have m_thread.joinable().
    if (m_thread.joinable()) m_thread.join();

    // Assert !m_thread.joinable()

    // NOTE:
    // This function has the same code as WaitUntilStopped()
    // but I think it is simpler to consider them as distinct
    // functions because
    //    o The preconditions are different.
    //    o The postconditions are different.
}

