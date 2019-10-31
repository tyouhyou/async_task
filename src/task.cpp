#include "task.hpp"

using namespace zb;

thread_pool::ptr task::pool_ = thread_pool::pool_ptr();