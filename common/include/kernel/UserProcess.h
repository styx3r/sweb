#pragma once

#include "FileSystemInfo.h"
#include "UserThread.h"

#include "ustl/umemory.h"
#include "ustl/ustring.h"
#include "ustl/uvector.h"
#include "ustl/ufunction.h"

class Loader;

class UserProcess
{
  public:
    /**
     * Constructor
     * @param minixfs_filename filename of the file in minixfs to execute
     * @param fs_info filesysteminfo-object to be used
     * @param terminal_number the terminal to run in (default 0)
     *
     */
    UserProcess(ustl::string minixfs_filename, FileSystemInfo *fs_info, uint32 terminal_number = 0);

    virtual ~UserProcess();

    virtual void Run(); // not used

    /**
     * Getter for the main thread of the process.
     * @return Pointer to the main thread.
     * NOTE: This RAW pointer is internally managed by a ustl::shared_ptr.
     */
    UserThread* getMainThread() const;

    ustl::shared_ptr<UserThread> createUserThread(const unsigned int *attr,
                                                  void *(*start_routine)(void *),
                                                  void *arg);

  private:
    int32 fd_;
    ustl::string minixfs_filename_;
    FileSystemInfo *fs_info_ = nullptr;

    ustl::vector<ustl::shared_ptr<UserThread>> threads_;
    ustl::shared_ptr<Loader> loader_;
};

