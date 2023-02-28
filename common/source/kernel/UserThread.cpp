#include "UserThread.h"
#include "ArchThreads.h"
#include "PageManager.h"
#include "UserProcess.h"

//-----------------------------------------------------------------------------

UserThread::UserThread(FileSystemInfo* working_dir,
                       ustl::string name,
                       ustl::shared_ptr<Loader> loader,
                       UserProcess *parent_process)
  : Thread{working_dir, name, Thread::TYPE::USER_THREAD},
    parent_process_{parent_process}
{
  loader_ = loader.get();

  init(loader_->getEntryFunction());
}

//-----------------------------------------------------------------------------

UserThread::UserThread(FileSystemInfo* working_dir,
                       ustl::string name,
                       ustl::shared_ptr<Loader> loader,
                       void *start_routine,
                       UserProcess *parent_process)
  : Thread{working_dir, name, Thread::TYPE::USER_THREAD},
    parent_process_{parent_process}
{
  loader_ = loader.get();

  init(start_routine);
}

//-----------------------------------------------------------------------------

UserThread::~UserThread()
{
  debug(THREAD, "~UserThread: %zu\n", getTID());

  delete working_dir_;
  working_dir_ = 0;
}

//-----------------------------------------------------------------------------

void UserThread::Run()
{
  switch_to_userspace_ = 1;
}

//-----------------------------------------------------------------------------

UserProcess* UserThread::getParentProcess() const
{
  return parent_process_;
}

//-----------------------------------------------------------------------------

void UserThread::init(void *start_routine)
{
  uint64 page_for_stack = PageManager::instance()->allocPPN();

  uint64 running_threads
    = Scheduler::instance()->getThreadCount() > 0 ? Scheduler::instance()->getThreadCount() : 1;

  debug(THREAD, "Running threads: %zu\n", running_threads);

  uint64 virtual_page_for_stack = (USER_BREAK / PAGE_SIZE) - (running_threads - 4);

  bool vpn_mapped = loader_->arch_memory_.mapPage(virtual_page_for_stack,
                                                  page_for_stack,
                                                  kAllowUserAccessToMemoryPage);

  assert(vpn_mapped && "Virtual page for stack was already mapped - this should never happen");

  void *user_stack_address = (void*)(USER_BREAK - (sizeof(pointer) * (running_threads - 4)));
  ArchThreads::createUserRegisters(user_registers_,
                                   start_routine,
                                   user_stack_address,
                                   getKernelStackStartPointer());

  ArchThreads::setAddressSpace(this, loader_->arch_memory_);

  setTID(running_threads);

  debug(THREAD,
        "Created main thread with PPN: %lx, VPN: %lx, user_stack_address: %p, TID: %zu\n",
        page_for_stack,
        virtual_page_for_stack,
        user_stack_address,
        getTID());
}
