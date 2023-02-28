#pragma once

#include "Thread.h"
#include "FileSystemInfo.h"
#include "Loader.h"
#include "ustl/ustring.h"
#include "ustl/umemory.h"

class UserProcess;

class UserThread : public Thread
{
  public:
    UserThread(FileSystemInfo* working_dir,
               ustl::string name,
               ustl::shared_ptr<Loader> loader,
               UserProcess *parent_process);

    UserThread(FileSystemInfo* working_dir,
               ustl::string name,
               ustl::shared_ptr<Loader> loader,
               void *start_routine,
               UserProcess *parent_process);

    virtual ~UserThread();

    void Run() override;

    UserProcess* getParentProcess() const;

  private:
    void init(void *start_routine);

    static constexpr uint64 kAllowUserAccessToMemoryPage = 1;

    UserProcess *parent_process_ = nullptr;
};
