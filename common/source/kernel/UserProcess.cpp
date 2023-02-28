#include "UserProcess.h"
#include "ArchMemory.h"
#include "ArchThreads.h"
#include "Console.h"
#include "File.h"
#include "Loader.h"
#include "PageManager.h"
#include "ProcessRegistry.h"
#include "VfsSyscall.h"
#include "kprintf.h"
#include "offsets.h"

//-----------------------------------------------------------------------------

UserProcess::UserProcess(ustl::string minixfs_filename,
                         FileSystemInfo *fs_info,
                         uint32 terminal_number)
  : fd_{VfsSyscall::open(minixfs_filename, O_RDONLY)},
    minixfs_filename_{minixfs_filename},
    fs_info_{fs_info}
{
  ProcessRegistry::instance()->processStart(); //should also be called if you fork a process

  if (fd_ >= 0)
    loader_ = ustl::make_shared<Loader>(fd_);

  if (loader_ == nullptr || !loader_->loadExecutableAndInitProcess())
  {
    debug(USERPROCESS, "Error: loading %s failed!\n", minixfs_filename.c_str());
    return;
  }

  threads_.emplace_back(ustl::make_shared<UserThread>(fs_info, minixfs_filename, loader_, this));
  debug(USERPROCESS, "ctor: Done loading %s\n", minixfs_filename.c_str());

  if (main_console->getTerminal(terminal_number))
    threads_.front()->setTerminal(main_console->getTerminal(terminal_number));

  threads_.front()->Run();
}

//-----------------------------------------------------------------------------

UserProcess::~UserProcess()
{
  assert(Scheduler::instance()->isCurrentlyCleaningUp());

  if (fd_ > 0)
    VfsSyscall::close(fd_);

  ProcessRegistry::instance()->processExit();
}

//-----------------------------------------------------------------------------

void UserProcess::Run()
{
  debug(USERPROCESS, "Run: Fail-safe kernel panic - you probably have forgotten to set switch_to_userspace_ = 1\n");
  assert(false);
}

//-----------------------------------------------------------------------------

UserThread* UserProcess::getMainThread() const
{
  return threads_.front().get();
}

//-----------------------------------------------------------------------------

ustl::shared_ptr<UserThread> UserProcess::createUserThread(const unsigned int *,
                                                           void *(*start_routine)(void *),
                                                           void *)
{
  threads_.emplace_back(ustl::make_shared<UserThread>(
      fs_info_, minixfs_filename_, loader_, (void*)start_routine, this));

  return threads_.back();
}
