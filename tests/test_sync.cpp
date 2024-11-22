#include <iostream>
#include <string>
#include <thread>

using namespace std;


#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/web/protocol/http/outgoing/MultipartBody.hpp"
#include "oatpp/base/Log.hpp"

#include "oatpp/data/resource/Resource.hpp"
#include "oatpp/async/Executor.hpp"
#include "oatpp/async/ConditionVariable.hpp"

bool keepRunning = true;

oatpp::async::Lock globalLock;
oatpp::async::ConditionVariable globalCv;


struct Resource {
  v_int64 counter;
};

std::shared_ptr<Resource> resource(new Resource());

class TestCoroutineWait : public oatpp::async::Coroutine<TestCoroutineWait> {
private:
public:
  std::shared_ptr<Resource> m_resource;
  oatpp::async::LockGuard m_lockGuard;
  oatpp::async::ConditionVariable* m_cv;
public:

  TestCoroutineWait(std::shared_ptr<Resource> resource,
                    oatpp::async::Lock* lock,
                    oatpp::async::ConditionVariable* cv)
    : m_resource(resource)
    , m_lockGuard(lock)
    , m_cv(cv)
  {}

  Action act() override {
    return m_cv->wait(m_lockGuard, [this]() noexcept {
        cout << "bop " << m_resource->counter << endl;
        return m_resource->counter == 3;})
            .next(yieldTo(&TestCoroutineWait::onReady));
  }

  Action onReady() {
    OATPP_ASSERT(m_lockGuard.owns_lock())
    cout << "BOPPED" << endl;
    keepRunning = false;
    return finish();
  }

};


void timerFunc() 
{
    while (keepRunning) {
      std::this_thread::sleep_for(std::chrono::microseconds(1000 * 1000));

        cout << "beep." << endl;
        // increment sema
        {
            std::lock_guard<oatpp::async::Lock> guard(globalLock);
            resource->counter++;
        }
        globalCv.notifyAll();
    }
}

/**
 *  main
 */
int main(int argc, const char *argv[])
{
    std::thread timer(timerFunc);

    oatpp::async::Executor executor;

    // oatpp::async::Lock lock;
    // oatpp::async::ConditionVariable cv;
    executor.execute<TestCoroutineWait>(resource, &globalLock, &globalCv);


      executor.waitTasksFinished();
      executor.stop();
      executor.join();

    timer.join();                // pauses until first finishes

    return 0;
}